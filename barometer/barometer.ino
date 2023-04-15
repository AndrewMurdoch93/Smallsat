/*  Barometer (SPL06) sends data to COM port. Code is modified from SPL06 library example.
 *  Only connect barometer, no other peripherals (e.g., SD card)
 *  
 *  SPL06 documentation: https://datasheet.lcsc.com/szlcsc/1912111437_Goertek-SPL06-007_C233787.pdf
 *  See documentation for pin configuration of the xCore: https://github.com/domino4com/CWV
 *  SPL06 library must be installed: https://github.com/rv701/SPL06-007
 *  Unmodified example code from SPL library: https://github.com/rv701/SPL06-007/tree/master/examples/Pressure_Info
 *  
 *  See SDG kit sensors information: https://github.com/domino4com/xChips
 *  I2C address of barometer is 0x77
 */

#include <xCore.h>                //This library allows the ESP32 to function properly
#include <xVersion.h>             //This library tells the script how to work with the ESP32 chipset
#include <Wire.h>                 //This library provides the neccessary code for I2C protocol
#include <SPL06-007.h>            //This library provides the neccessary code for the SPL06 peripheral

#define I2C_SDA 26    // See I2C configuration of the xCore, https://github.com/domino4com/xChips 
#define I2C_SCL 27

void setup() {
  Serial.begin(115200);       // begin Serial
  delay(500);                 // Must include a small delay betwwen serial initialisation and first serial print  
  Serial.println("Setup start");
  
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();     // begin Wire(I2C)
  
  SPL_init(0x77);   // Setup initial SPL chip registers - default i2c address 0x76  
  
  Serial.println("Setup complete");
}

void loop() {

  // ---- Register Values ----------------
  Serial.print("ID: ");
  Serial.println(get_spl_id());

  Serial.print("PRS_CFG: ");
  Serial.println(get_spl_prs_cfg(),BIN);

  Serial.print("TMP_CFG: ");
  Serial.println(get_spl_tmp_cfg(),BIN);

  Serial.print("MEAS_CFG: ");
  Serial.println(get_spl_meas_cfg(),BIN);

  Serial.print("CFG_REG: ");
  Serial.println(get_spl_cfg_reg(),BIN);

  Serial.print("INT_STS: ");
  Serial.println(get_spl_int_sts(),BIN);

  Serial.print("FIFO_STS: ");
  Serial.println(get_spl_fifo_sts(),BIN);


  // ---- Coefficients ----------------
  Serial.print("c0: ");
  Serial.println(get_c0());

  Serial.print("c1: ");
  Serial.println(get_c1());

  Serial.print("c00: ");
  Serial.println(get_c00());

  Serial.print("c10: ");
  Serial.println(get_c10());

  Serial.print("c01: ");
  Serial.println(get_c01());

  Serial.print("c11: ");
  Serial.println(get_c11());
  
  Serial.print("c20: ");
  Serial.println(get_c20());
  
  Serial.print("c21: ");
  Serial.println(get_c21());

  Serial.print("c30: ");
  Serial.println(get_c30());


  // ---- Temperature Values ----------------  
  Serial.print("traw: ");
  Serial.println(get_traw());

  Serial.print("traw_sc: ");
  Serial.println(get_traw_sc(),3);
  
  Serial.print("Temperature: ");
  Serial.print(get_temp_c());
  Serial.println(" C");
  
  Serial.print("Temperature: ");
  Serial.print(get_temp_f());
  Serial.println(" F");
  

  // ---- Pressure Values ----------------
  Serial.print("praw: ");
  Serial.println(get_praw());
  
  Serial.print("praw_sc: ");
  Serial.println(get_praw_sc(),3);

  Serial.print("pcomp: ");
  Serial.println(get_pcomp(),2);

  Serial.print("Measured Air Pressure: ");
  Serial.print(get_pressure(),2);
  Serial.println(" mb");


  // ---- Altitude Values ----------------
  double local_pressure = 1011.3; // Look up local sea level pressure on google // Local pressure from airport website 8/22
  Serial.print("Local Airport Sea Level Pressure: ");
  Serial.print(local_pressure,2);
  Serial.println(" mb");
  
  Serial.print("altitude: ");
  Serial.print(get_altitude(get_pressure(),local_pressure),1);
  Serial.println(" m");

  Serial.print("altitude: ");
  Serial.print(get_altitude_f(get_pressure(),local_pressure),1); // convert from meters to feet
  Serial.println(" ft");



  Serial.println("\n");
  delay(2000);
}
