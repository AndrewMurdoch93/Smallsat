/* A program that writes some data to the SD card
 * 
 * See the links below for extra resources:
 * SD card tutorial using FS library: https://randomnerdtutorials.com/esp32-microsd-card-arduino/
 * SD Library information: https://github.com/arduino-libraries/SD, https://docs.arduino.cc/learn/programming/sd-guide
 * See documentation for pin configuration of the xCore: https://github.com/domino4com/CWV
 * 
 * SD card functions cannot be written inside another sketch, otherwise the chip "bricks"
 * SD card functions cannot be called from inside the loop function, otherwise the chip "bricks"
 * Therefore, the entire code is written inside the setup function
 * 
 * If the xCore "bricks", i.e., data packet fails to return when uploading a sketch, do the following:
 * Install esptool via a terminal using "pip install esptool" 
 * Remove the satellite from usb, remove sd card, reinsert satellite into usb, then immediately run "python -m esptool erase_flash" in terminal
 */

#include "SD.h"
#include "SPI.h"
#include "string.h"

// Pins for SD card
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5


void setup() {

  Serial.begin(115200);
  delay(1000);
  
  // Initialise SD card  
  SPIClass spi = SPIClass(HSPI);
  spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  if (!SD.begin(SD_CS, spi)) {
    Serial.println("Failed to initialise SD");
    return;
  }

  listCardInfo();
  
  // Create a file, if it does not exist
  listDir("/");
  String filePath = "/myfile.txt";
  if(SD.exists(filePath)) {
   Serial.print("File exists");
  } else {
    filePath = makeFile(filePath);
  }
  listDir("/");

  // Append some data to the file  
  String message = "";
  for (int i = 0; i < 10; i++){
    message = String(i) + ", ";
    writeFile(filePath, message);
    Serial.println(message);
  }

  // Read the contents of the file  
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
    file.print(message);
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

void listCardInfo(){
  /* Prints SD card information to COM port
   *  Inputs: None
   *  Returns: None
   *  
   *  Notes:
   *  Function cannot be used inside of void loop()
   */

  uint8_t cardType = SD.cardType();
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  //  Serial.printf("SD Card Size: %lluMB\n", SD.cardSize());
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}
