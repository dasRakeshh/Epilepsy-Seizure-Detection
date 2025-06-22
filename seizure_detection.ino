#include <Wire.h>
#include "model.h" // Include the exported model

Eloquent::ML::Port::RandomForest model;

// Constants
const int MPU_ADDR = 0x68;
const int windowSize = 10;
float features[30]; // 10 * 3-axis = 30 features
int featureIndex = 0;

// Previous accelerometer values
int16_t prev_ax = 0, prev_ay = 0, prev_az = 0;

unsigned long lastReadTime = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  // Wake up MPU6050 (initially in sleep mode)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Serial.println("MPU6050 initialized");
}

void loop()
{
  unsigned long now = millis();
  if (now - lastReadTime < 400)
    return; // Read every 400ms
  lastReadTime = now;

  int16_t ax, ay, az;

  // Read accelerometer data
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Starting register for Accel data
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true); // Request 6 bytes

  ax = Wire.read() << 8 | Wire.read(); // Accel X
  ay = Wire.read() << 8 | Wire.read(); // Accel Y
  az = Wire.read() << 8 | Wire.read(); // Accel Z

  // Calculate delta
  float dax = ax - prev_ax;
  float day = ay - prev_ay;
  float daz = az - prev_az;

  prev_ax = ax;
  prev_ay = ay;
  prev_az = az;

  // Store deltas in the features array
  features[featureIndex++] = dax;
  features[featureIndex++] = day;
  features[featureIndex++] = daz;
  Serial.print(dax);
  Serial.print(day);
  Serial.println(daz);

  // If we've collected 10 samples (30 values), run the model
  if (featureIndex >= 30)
  {
    uint8_t prediction = model.predict(features);

    if (prediction == 0)
    {
      Serial.println("Normal");
    }
    else
    {
      Serial.println("Seizure Detected!");
    }

    featureIndex = 0; // Reset for next window
  }
}
