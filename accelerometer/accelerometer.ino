/* A simple sketch that reads data from the provided IMU (LIS2DH12) via I2C protocol
 * 
 * See documentation for pin configuration of the xCore: https://github.com/domino4com/CWV
 * xCore sensor information: https://github.com/domino4com/xChips
 * Sparkfun LIS2DH12 library: https://github.com/sparkfun/SparkFun_LIS2DH12_Arduino_Library
 * Unmodified example code from library: https://github.com/sparkfun/SparkFun_LIS2DH12_Arduino_Library/tree/master/examples/Example1_BasicReadings
 * 
 * !!!NB!!!
 * The accelerometer is connected to the same pins that are used to program the core
 * Therefore, the sketch cannot be uploaded while the accelerometer is plugged in
 * Rather, take the accelerometer off, then plug it in while sketch is running and outputting "Accelerometer not detected" to serial monitor
 * Or, unplug the satellite, attach the accelerometer, plug it in and out twice after uploading the sketch. Acceleration values should start appearing on the serial monitor
 * 
 */

#include <Wire.h>         
#include "SparkFun_LIS2DH12.h"   

SPARKFUN_LIS2DH12 accel;    

#define I2C_SDA 26  
#define I2C_SCL 27

void setup() {
  Serial.begin(115200);
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();

  // Try to initialise accelerometer  
  while (accel.begin(0x19) == false)
  {
    // Plug accelerometer in while this code is running    
    Serial.println("Accelerometer not detected");
    delay(100);
  }
}

void loop() {
 if (accel.available()) {
    float accelX = accel.getX();
    float accelY = accel.getY();
    float accelZ = accel.getZ();
    float tempC = accel.getTemperature();

    Serial.print("Acc [mg]: ");
    Serial.print(accelX, 1);
    Serial.print(" x, ");
    Serial.print(accelY, 1);
    Serial.print(" y, ");
    Serial.print(accelZ, 1);
    Serial.print(" z, ");
    Serial.print(tempC, 1);
    Serial.print(" C.");
    Serial.println();
  }
}
