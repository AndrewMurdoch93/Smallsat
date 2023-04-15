/* A program that reads acceleration data from the IMU (LIS2DH12) to the SD card
 * 
 * See the links below for extra resources:
 * Pin configuration of the xCore: https://github.com/domino4com/CWV
 * xCore sensor information: https://github.com/domino4com/xChips
 * SD Library information: https://github.com/arduino-libraries/SD, https://docs.arduino.cc/learn/programming/sd-guide
 * Sparkfun LIS2DH12 library: https://github.com/sparkfun/SparkFun_LIS2DH12_Arduino_Library 
 * 
 * !!!NB!!!
 * The accelerometer is connected to the same pins that are used to program the core
 * Therefore, the sketch cannot be uploaded while the accelerometer is plugged in
 * Rather, take the accelerometer off, then plug it in while sketch is running and outputting "Accelerometer not detected" to serial monitor
 * Or, unplug the satellite, attach the accelerometer, plug it in and out twice after uploading the sketch. Acceleration values should start appearing on the serial monitor
 * 
 * SD card functions cannot be written inside another sketch, otherwise the chip "bricks"
 * SD card functions cannot be called from inside the loop function, otherwise the chip "bricks"
 * Therefore, the entire code is written inside the setup function
 * 
 * If the xCore "bricks", i.e., data packet fails to return when uploading a sketch, do the following:
 * Install esptool via a terminal using "pip install esptool" 
 * Remove the satellite from usb, remove sd card, reinsert satellite into usb, then immediately run "python -m esptool erase_flash" in terminal
 */



#include "SD.h"                   // This library provides the neccessary code for SD card interface
#include "SPI.h"                  // This library provides the neccessary code for SPI protocol used for SD card communication
#include "string.h"               // This library provides the neccessary code to work with Strings
#include "SparkFun_LIS2DH12.h"

#include <Wire.h>                 // This library provides the neccessary code for I2C protocol

SPARKFUN_LIS2DH12 accel;

// Pins for SD card, specified at https://github.com/domino4com/CWV
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5

// Pins for I2C protocol, specified at https://github.com/domino4com/CWV
#define I2C_SDA 26    
#define I2C_SCL 27


void setup() {

  Serial.begin(115200);
  delay(5000);
  Serial.println("Setup start");

  Wire.setPins(I2C_SDA, I2C_SCL); 
  Wire.begin();     // begin Wire(I2C)

  // Try to initialise accelerometer  
  while (accel.begin(0x19) == false)
  {
    // Plug accelerometer in while this code is running    
    Serial.println("Accelerometer not detected");
    delay(100);
  }
  
  // Initialise SD card  
  SPIClass spi = SPIClass(HSPI);
  spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  if (!SD.begin(SD_CS, spi)) {
  // If SD fails to mount, stop script    
    Serial.println("Failed to initialise SD");
    return;
  }

  Serial.println("Setup complete");
  
  // Create a file to record data to, if it does not yet exist
  listDir("/");
  String filePath = "/acceleration.txt";
  if(SD.exists(filePath)) {
   Serial.print("File exists");
  } else {
    filePath = makeFile(filePath);
  }
  listDir("/");

  
  // Append some acceleration data to the file  
  String message = "";
  for (int i = 0; i < 60; i++){
    message = String(micros()) + String(accel.getX()) + String(accel.getY()) + accel.getZ();
    writeFile(filePath, message);
    Serial.println(message);
  }
  
  // Read the contents of the file via COM port 
  readFile(filePath);

  // Delete file 
  //  SD.remove(filePath);
  listDir("/");
  
}

void loop() {
  // Loop cannot include any SD card functions  
  delay(1000);
}

void writeFile(String filepath, String message){
  /*  Appends data to a file
   *  Input: The filepath, e.g. /myfile.txt, message to be written into file
   *  Returns: none
   *  
   *  Notes:
   *  Filepath must include "/" for home directory and file extension 
   *  Function cannot be used inside void loop()
   */

  File file = SD.open(filepath, FILE_APPEND);
  if(file) {
    file.println(message);
  } else {
    Serial.println("Failed to write data");
  }
  file.close();
}

void readFile(String filePath){
  /*  Reads the entire contents of a file, from front to back. Outputs file contents to COM port
   *  Input: The filepath, e.g. /myfile.txt
   *  Returns: none
   *  
   *  Notes:
   *  Filepath must include "/" for home directory and file extension 
   *  Function cannot be used inside void loop()
   */
  
  File file = SD.open(filePath, FILE_READ);
  if(file) {
    while (file.available()) {
      Serial.write(file.read());
    }
  } else {
    Serial.println("Failed to read data");
  }
  Serial.print("\n");
  file.close();
}

String makeFile(String filePath){
  /*  Creates a file
   *  If file exists, a file is create with the number "filPath_number.extension"  
   *  Input: The filepath, e.g. /myfile.txt
   *  Returns: The filepath of the file created, e.g. /myfile_1.txt
   *  
   *  Notes:
   *  Filepath must include "/" for home directory and file extension 
   *  Function cannot be used inside void loop()
   */
  
  int i=0;
  char *token;
  const char delim[2] = ".";
  char str[80];
  filePath.toCharArray(str, filePath.length()+1);
  String extension;
  String newFilePath = "";
  String originalFilePath = "";

  // Get filename and extension  
  originalFilePath = strtok(str, delim);
  extension = strtok(NULL, delim);  

  // Create new filepath if file already exists  
  newFilePath = originalFilePath + "." + extension;
  while(SD.exists(newFilePath)){
    newFilePath = originalFilePath + "_" + String(i) + "." + extension;
    i++;
    delay(200);
  }
  
  // Create and close file  
  File file = SD.open(newFilePath, FILE_WRITE);
  file.close();
  
  String message = "Created file: \"" + newFilePath + "\"";
  Serial.println(message);

  return newFilePath;
}

void listDir(String directory) {
  /*  Prints the files in a specified directory to the COM port
   *  Input: Directory name, e.g "/" for home
   *  Returns: None
   *  
   *  Notes:
   *  Directoy name must include "/"
   *  Function cannot be used inside void loop()
   *  Uses printDirectory()
   */
  
  Serial.println("");
  File dir = SD.open(directory);
  printDirectory(dir, 0);
  dir.close();
  Serial.println("");
}

void printDirectory(File dir, int numTabs) {
  /*  Function utilised by listDir to print files in a directory to COM port
   *  
   *  Notes:
   *  Do not call printDirectory, rather use listDir
   *  Function cannot be used inside of void loop()
   */
  
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
