/* A simple hello world program, only prints to COM port
 * Run with no peripherals
 */

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Must include a small delay betwwen serial initialisation and first serial print  
  delay(500);
  
  Serial.println("Communcation started");
}

void loop() {
  delay(500); 
  Serial.println("0.5 seconds have passed");
}
