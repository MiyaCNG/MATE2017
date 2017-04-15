/* YourDuino SoftwareSerialExample1
   - Connect to another Arduino running "YD_SoftwareSerialExampleRS485_1Remote"
   - Connect this unit Pins 10, 11, Gnd
   - Pin 3 used for RS485 direction control
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Open Serial Monitor, type in top window. 
   - Should see same characters echoed back from remote Arduino

   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 12   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW


/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
String byteReceived;
String byteSend;
byte srcaddr = 6;
byte destaddr = 5;
byte type = 1;
void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(38400);
  Serial.println("Use Serial Monitor, type in upper window, ENTER");
   
  pinMode(SSerialTxControl, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   
  
  // Start the software serial port, to another device
  RS485Serial.begin(115200);   // set the data rate 

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  if (Serial.available())
  {
    byteReceived=Serial.readStringUntil('\r');
    Serial.print("New TX: ");
    Serial.print(byteReceived);
    Serial.print("\tNew TX Len: ");
    Serial.println(byteReceived.length());
    
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit
    //delay(1);
    byte len = byte(byteReceived.length());
    
    byte bytestring[255];
    byteReceived.getBytes(bytestring,255);
    RS485Serial.write(len);
    RS485Serial.write(CRC8(bytestring,len));
    RS485Serial.write(srcaddr);     
    RS485Serial.write(destaddr);   
    RS485Serial.write(type);   
    RS485Serial.print(byteReceived);          // Send byte to Remote Arduino
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
  }
  
  if (RS485Serial.available())  //Look for data from other Arduino
   {
    byte num[5];
    RS485Serial.readBytes(num,5);
    
    int len = num[0];

    if (len!=0){
    char bytes[255];
    memset(bytes,0,sizeof(bytes));
    RS485Serial.readBytes(bytes,len);    // Read received byte
    
    Serial.print("RX Len: ");
    Serial.print(len);
    Serial.print("\tCRC8: ");
    Serial.print(num[1]);
    Serial.print("\tSrc Addr: ");
    Serial.print(num[2]);
    Serial.print("\tDest Addr: ");
    Serial.print(num[3]);
    Serial.print("\tType: ");
    Serial.print(num[4]);
    Serial.print("\tMsg: ");
    Serial.println(bytes);        // Show on Serial Monitor
    Serial.print("Computed CRC8: ");
    Serial.println(CRC8(bytes,len));
    delay(10);
    }
   }  

}//--(end main loop )---
//http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/
//CRC-8 - based on the CRC8 formulas by Dallas/Maxim
//code released under the therms of the GNU GPL 3.0 license
byte CRC8(const byte *data, byte len) {
  byte crc = 0x00;
  while (len--) {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}
/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********

