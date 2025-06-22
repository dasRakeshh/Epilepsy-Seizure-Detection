import pandas as pd
import matplotlib.pyplot as plt

# Load the datasets
normal_df = pd.read_csv("normal.csv")
fall_df = pd.read_csv("fall.csv")

# Plot ax, ay, az for normal data
plt.figure(figsize=(15, 6))
plt.subplot(1, 2, 1)
plt.plot(normal_df["ax"], label="ax", color="blue")
plt.plot(normal_df["ay"], label="ay", color="green")
plt.plot(normal_df["az"], label="az", color="red")
plt.title("Normal Data")
plt.xlabel("Time Step")
plt.ylabel("Acceleration")
plt.legend()
plt.grid(True)

# Plot ax, ay, az for fall data
plt.subplot(1, 2, 2)
plt.plot(fall_df["ax"], label="ax", color="blue")
plt.plot(fall_df["ay"], label="ay", color="green")
plt.plot(fall_df["az"], label="az", color="red")
plt.title("Fall Data")
plt.xlabel("Time Step")
plt.ylabel("Acceleration")
plt.legend()
plt.grid(True)

# Show the plot
plt.tight_layout()
plt.show()
