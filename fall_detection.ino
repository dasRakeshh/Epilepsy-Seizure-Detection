#include <Wire.h>
#include "model.h"  // Exported from micromlgen

#define MPU_ADDR 0x68
#define WINDOW_SIZE 25

float ax, ay, az;
float window[75];
int sample_count = 0;

String prediction_buffer[6] = {"normal", "normal", "normal", "normal", "normal", "normal"};

Eloquent::ML::Port::RandomForest clf;  // Class name based on micromlgen export

// Timer for 10-second system state check
unsigned long lastStateCheckTime = 0;
const unsigned long stateCheckInterval = 30000;  // 10 seconds

// === MPU6050 SETUP ===
void setupMPU6050() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // Power management register
  Wire.write(0);     // Wake up MPU6050
  Wire.endTransmission(true);
}

// === READ MPU6050 RAW DATA ===
void readMPU6050() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Starting register for ax
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  int16_t raw_ax = Wire.read() << 8 | Wire.read();
  int16_t raw_ay = Wire.read() << 8 | Wire.read();
  int16_t raw_az = Wire.read() << 8 | Wire.read();
  ax = raw_ax;
  ay = raw_ay;
  az = raw_az;
  // Serial.print("ax: ");
  // Serial.print(ax);
  // Serial.print("ay: ");
  // Serial.print(ay);
  // Serial.print("az: ");
  // Serial.println(az);
}

// === SHIFT PREDICTION BUFFER ===
void updatePredictionBuffer(String new_prediction) {
  for (int i = 0; i < 5; i++) {
    prediction_buffer[i] = prediction_buffer[i + 1];
  }
  prediction_buffer[5] = new_prediction;
  Serial.print("Prediction: ");
  Serial.println(new_prediction);
}

// === DETECT SYSTEM STATE ===
String detectSystemState() {
  bool all_normal = true;
  bool fall_detected = false;
  bool normal_after_fall = false;

  for (int i = 0; i < 6; i++) {
    if (prediction_buffer[i] != "normal") {
      all_normal = false;
    }
  }

  for (int i = 1; i < 6; i++) {
    if (prediction_buffer[i - 1] == "normal" && prediction_buffer[i] == "fall") {
      fall_detected = true;
    }
  }

  for (int i = 0; i < 5; i++) {
    if (prediction_buffer[i] == "fall" && prediction_buffer[i + 1] == "normal") {
      normal_after_fall = true;
    }
  }

  if (all_normal) return "Normal";
  if (fall_detected && !normal_after_fall) return "Fall";
  if (fall_detected && normal_after_fall) return "FallToNormal";
  if (!fall_detected && !normal_after_fall) return "Sleeping";
  if (!fall_detected && normal_after_fall) return "FallToNormal";

  return "Unknown";
}

// === PREDICTION USING model.h ===
String predictFall(float input[75]) {
  uint8_t result = clf.predict(input);
  return result == 0 ? "fall" : "normal";
}

void setup() {
  Serial.begin(115200);
  setupMPU6050();
  delay(100);
}

void loop() {
  readMPU6050();

  // Add to 75-feature window
  window[sample_count * 3 + 0] = ax;
  window[sample_count * 3 + 1] = ay;
  window[sample_count * 3 + 2] = az;
  sample_count++;

  // When 25 samples collected, predict
  if (sample_count == WINDOW_SIZE) {
    String result = predictFall(window);
    updatePredictionBuffer(result);

    // Serial.print("Prediction: ");
    // Serial.println(result);

    sample_count = 0;
  }

  // Every 10 seconds, evaluate system state
  if (millis() - lastStateCheckTime >= stateCheckInterval) {
    String state = detectSystemState();
    Serial.print("[10s Check] System State: ");
    Serial.println(state);
    lastStateCheckTime = millis();
  }

  delay(200);  // ~25Hz sampling rate
}
