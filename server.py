from ast import If
from fastapi import FastAPI
from pydantic import BaseModel
from datetime import datetime, timedelta
import asyncio
import csv

app = FastAPI()


class IMUData(BaseModel):
    ax: int
    ay: int
    az: int
    gx: int
    gy: int
    gz: int


data_buffer = []
last_received_time = datetime.utcnow()
DISCONNECT_TIMEOUT = timedelta(seconds=10)


async def monitor_disconnect():
    global data_buffer, last_received_time
    while True:
        await asyncio.sleep(5)
        if data_buffer and datetime.utcnow() - last_received_time > DISCONNECT_TIMEOUT:
            filename = datetime.utcnow().strftime("imu_data_%Y%m%d_%H%M%S.csv")
            with open(filename, mode="w", newline="") as file:
                writer = csv.writer(file)
                writer.writerow(["timestamp", "ax", "ay", "az", "gx", "gy", "gz"])
                writer.writerows(data_buffer)
            print(f"[INFO] Saved CSV: {filename}")
            data_buffer.clear()


@app.on_event("startup")
async def startup_event():
    asyncio.create_task(monitor_disconnect())


@app.post("/imu")
async def receive_imu(data: IMUData):
    global last_received_time
    last_received_time = datetime.utcnow()
    timestamp = last_received_time.isoformat()
    data_buffer.append(
        [timestamp, data.ax, data.ay, data.az, data.gx, data.gy, data.gz]
    )
    return {"status": "received"}


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app="server:app", host="192.168.238.150", reload=True)
