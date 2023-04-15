/* Communicates with the barometer via custom I2C functions
 * Only connect barometer, no other peripherals (e.g., SD card)
 * SPL06 documentation: https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
 * An I2C example: https://docs.arduino.cc/learn/communication/wire
 * See SDG kit sensors information: https://github.com/domino4com/xChips
 * I2C address of barometer is 0x77
 * This example is mofified from SPL06 library source code: https://github.com/rv701/SPL06-007/tree/master/src
 */

#include <xCore.h>                //This library allows the ESP32 to function properly
#include <xVersion.h>             //This library tells the script how to work with the ESP32 chipset
#include <Wire.h>                 //This library provides the neccessary code for I2C protocol
       
#define I2C_SDA 26    // See I2C configuration of the xCore, https://github.com/domino4com/xChips 
#define I2C_SCL 27

uint8_t rdata=0xFF;
uint8_t device_address = 0x77;

void setup() {
  Serial.begin(115200);
  delay(500);                       // Must include a small delay betwwen serial initialisation and first serial print  
  Serial.println("Setup start");
  
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();                     // begin Wire(I2C)
  
  SPL_init(0x77);                   // Setup initial SPL chip registers - default i2c address 0x76
  
  Serial.println("Setup complete");

}

void loop() {
  Serial.println("");
  display_temp_config();
  delay(1000);                
}


void display_temp_config(){
  /* Reads the byte of memory containing the temperature sensing configuration of the SPL06 via I2C protocol
   * Input: None
   * Returns: A byte containing the config info
   *  
   * Notes
   * See pages 22 and 23 of SPL06 documentation, https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
   * Called from display_temp_config()
   */ 
   
  uint8_t rdata = get_temp_config();
  uint8_t TMP_EXT =  bitRead(rdata, 7);
  uint8_t TMP_RATE = bitRead(rdata, 4)+bitRead(rdata, 5)*2+bitRead(rdata, 6)*4; 
  uint8_t TMP_PRC = bitRead(rdata, 0)+bitRead(rdata, 1)*2+bitRead(rdata, 2)*4;
  
  Serial.print("TMP_EXT: ");
  Serial.println(TMP_EXT);
  
  Serial.print("TMP_RATE: ");
  Serial.println(TMP_RATE);
  
  Serial.print("TMP_PRC: ");
  Serial.println(TMP_PRC);
}


uint8_t get_temp_config(){
  /* Reads the byte of memory containing the temperature sensing configuration of the SPL06 via I2C protocol
   * Input: None
   * Returns: A byte containing the config info
   *  
   * Notes
   * See pages 22 and 23 of SPL06 documentation, https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
   * Called from display_temp_config()
   */ 
  
  uint8_t rdata = 0xFF;
  uint8_t deviceaddress = 0x77;
  uint8_t eeaddress = 0x07;
  rdata = readi2c(deviceaddress, eeaddress);
  return rdata;
}

void SPL_init(uint8_t spl_address)
{
  /*  Configures SPL device by writing into registers
   *  Input: Address of SPL device (0x77 for xCore)
   *  Return: None
   *  
   *  Notes
   *  See pages 17 to 23 of SPL06 documentation: https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
   */
   
  writei2c(device_address, 0X06, 0x03);     // Pressure 8x oversampling
  writei2c(device_address, 0X07, 0X83);     // Temperature 8x oversampling
  writei2c(device_address, 0X08, 0B0111);   // continuous temp and pressure measurement
  writei2c(device_address, 0X09, 0X00);     // FIFO Pressure measurement  
}

void writei2c(uint8_t deviceaddress, uint8_t eeaddress, uint8_t wdata){
  /* Writes data to memory of a peripheral device via I2C protocol
   *  Input: Device address (0x77 for xCore), eeaddress is memory address, wdata is a byte of data to be written
   *  Return: None
   *  
   *  Notes
   *  See page 9 of SPL06 documentation, https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
   */
   
    uint8_t write_data = wdata;
    delay(5); // Make sure to delay log enough for EEPROM I2C refresh time
    Wire.beginTransmission(deviceaddress);
    Wire.write((uint8_t)(eeaddress));
    Wire.write(write_data);
    Wire.endTransmission();
}

uint8_t readi2c(uint8_t deviceaddress, uint8_t eeaddress){
   /* Reads a byte of data from memory of a peripheral device via I2C protocol
   *  Input: Device address (0x77 for xCore), eeaddress is memory address
   *  Return: The byte that was read from memory
   *  
   *  Notes
   *  See page 9 of SPL06 documentation, https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
   */
  
  uint8_t rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write(eeaddress); 
  Wire.endTransmission(false); // false to not release the line
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) { 
    rdata = Wire.read();
  }
  return rdata;
}
