import pandas as pd
import numpy as np
import os

# Parameters
WINDOW_SIZE = 25
STEP_SIZE = 5


# Function to create overlapping windows from raw data (no delta)
def create_windows(raw_df, label):
    windows = []
    labels = []
    for i in range(0, len(raw_df) - WINDOW_SIZE + 1, STEP_SIZE):
        window = raw_df.iloc[
            i : i + WINDOW_SIZE
        ].values.flatten()  # Flatten 25 rows of [ax, ay, az] into one
        windows.append(window)
        labels.append(label)
    return windows, labels


# File list
filenames = ["normal.csv", "fall.csv"]

# Initialize storage
all_windows = []
all_labels = []

# Process each file
for file in filenames:
    df = pd.read_csv(file)

    # Keep only accelerometer data
    df = df[["ax", "ay", "az"]]

    # Extract label from filename (e.g., 'normal.csv' -> 'normal')
    label = os.path.splitext(os.path.basename(file))[0]

    # Create windows and labels from raw data
    windows, labels = create_windows(df, label)

    # Append to dataset
    all_windows.extend(windows)
    all_labels.extend(labels)

# Create DataFrame
X = pd.DataFrame(all_windows)
X["label"] = all_labels

# Save to CSV
X.to_csv("final_fall_dataset.csv", index=False)

print("✅ Dataset saved to final_fall_dataset.csv")
print("Shape:", X.shape)
