#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "V2029";
const char* password = "hotspot1";
const char* serverURL = "http://192.168.238.150:8000/imu"; // Adjust IP & port

int16_t ax, ay, az;
int16_t gx, gy, gz;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wake up MPU6050
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  // Request accelerometer and gyroscope data
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // Temperature - ignored
  gx = Wire.read() << 8 | Wire.read();
  gy = Wire.read() << 8 | Wire.read();
  gz = Wire.read() << 8 | Wire.read();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, serverURL); // ✅ Updated API usage
    http.addHeader("Content-Type", "application/json");

    String json = "{\"ax\":" + String(ax) + ",\"ay\":" + String(ay) + ",\"az\":" + String(az) +
                  ",\"gx\":" + String(gx) + ",\"gy\":" + String(gy) + ",\"gz\":" + String(gz) + "}";

    int httpResponseCode = http.POST(json);

    if (httpResponseCode > 0) {
      Serial.println("Data sent");
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(400);
}
