#include <Wire.h>

// MPU6050 I2C address
const int MPU6050_ADDR = 0x68;
const int PWR_MGMT_1 = 0x6B;
const int GYRO_XOUT_H = 0x43;

// Variables to store raw gyro values
int16_t gyroX, gyroY, gyroZ;

// Variables to store processed gyro values
float yaw = 0;
float gyroXoffset = 0;

// Time management
unsigned long previousTime = 0;
float elapsedTime;

// Sensitivity scale factor for MPU6050 (250 degrees/second)
const float GYRO_SENSITIVITY = 131.0;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // Wake up the MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission(true);

  // Calibrate gyro (getting offsets)
  calibrateGyro();
}

void loop() {
  unsigned long currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0; // Convert to seconds
  previousTime = currentTime;

  // Read raw gyro values
  readGyro();

  // Process gyro data to get yaw
  yaw += (gyroZ / GYRO_SENSITIVITY) * elapsedTime; // Integrate gyro to get yaw

  Serial.print("Yaw: ");
  Serial.println(yaw);
  delay(100); // Adjust as needed
}

void readGyro() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);

  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();

  // Adjusting gyro readings by subtracting the offset
  gyroZ -= gyroXoffset;
}

void calibrateGyro() {
  int16_t rawZ;
  const int numSamples = 1000;
  long sum = 0;

  Serial.println("Calibrating gyro...");
  for (int i = 0; i < numSamples; i++) {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(GYRO_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 6, true);

    rawZ = Wire.read() << 8 | Wire.read();
    sum += rawZ;
    delay(3); // Allow time for sensor to stabilize
  }

  gyroXoffset = sum / numSamples;
  Serial.println("Gyro calibration completed.");
}
