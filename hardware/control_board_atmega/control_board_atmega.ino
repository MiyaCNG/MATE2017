#define CTL_Board

#include <SoftwareSerial.h>
#include <Wire.h>
#include "messenger.h"

SoftwareSerial DebugSerial(MISO, MOSI);

//int B0_RE = 70;  //Serial Receive pin
//int B0_TE = 5;  //Serial Transmit pin
//int B1_RE = 71;  //Serial Receive pin
//int B1_TE = 72;  //Serial Transmit pin
//int B2_RE = 73; //Serial Receive pin
//int B2_TE = 6;  //Serial Transmit pin
long last_toggle;
int last_value;

void setup() {
  last_value = 0;
  last_toggle = millis();
  pinMode(62, OUTPUT); // Error led
  pinMode(63, OUTPUT); // Power led
  pinMode(64, OUTPUT); // Debug led
  digitalWrite(62, LOW); // Error led low
  digitalWrite(63, HIGH); // Power led high
  digitalWrite(64, LOW); // Debug led low

  // Set pullups on 485 UART rx lines
  digitalWrite(0, HIGH);
  digitalWrite(19, HIGH);
  digitalWrite(17, HIGH);

  // Set all RS485 tranceivers to receive.
  configureBus(0, 0);
  configureBus(0, 1);
  configureBus(0, 2);

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);  
  Serial3.begin(115200);

  Wire.begin(); // i2c bus
  
  DebugSerial.begin(115200);
  msgSetDebug(&DebugSerial);
  DebugSerial.println("Control Board Atmega initialized.");
}

void loop() {
  if (millis() - last_toggle > 100){
    last_value = !last_value;
    digitalWrite(63, last_value);
    last_toggle = millis();
  }
  char data[255];
  char key[255];
  struct Message message;
  message.data = data;
  message.key = key;
  for (int i = 0; i < 4; i++) {
    //DebugSerial.println("Awaiting message on " +String(i));
    if (receiveMessage(&message, i)){
      //DebugSerial.println("Received message on bus " + String(i));
      //reportMessage(&message);
      if (message.prefix == 'r') {
        DebugSerial.println("Forwarding return value");
        sendMessage(&message, 3);
      }
      else if (message.key[0] == 't') {
        //DebugSerial.println("Setting thruster " + String((int)(message.key[1] - '0')) + " to " + String(*(int*)message.data));
        sendMessage(&message, 0);
      }
      else if (message.key[0] == 'h') {
        DebugSerial.println("Getting humidity");
        unsigned int H_dat;
        unsigned int T_dat;
        fetch_humidity_temperature(&H_dat, &T_dat);
        struct HandT {
          float RH;
          float T_C;
        } data;
        data.RH = (float) H_dat * 6.10e-3;
        data.T_C = (float) T_dat * 1.007e-2 - 40.0;
        DebugSerial.println("H: " + String(data.RH));
        DebugSerial.println("T: " + String(data.T_C));
        struct Message response = {'r', message.key, message.keyLen, (char *) &data, sizeof(data)};
        //DebugSerial.println("Sending message: here it is!");
        //reportMessage(&response);
        sendMessage(&response, 3);
        //delay(10);
      }
      else {
        DebugSerial.println("No idea what we have here...");
      }
    }   
  }
}

byte fetch_humidity_temperature(unsigned int *p_H_dat, unsigned int *p_T_dat)
{
      byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
      unsigned int H_dat, T_dat;
      address = 0x27;
      Wire.beginTransmission(address); 
      Wire.endTransmission();
      
      //delay(100); // By not waiting we will always get slightly old data, but won't lock up the processor when fetching it.
      
      Wire.requestFrom((int)address, (int) 4);
      Hum_H = Wire.read();
      Hum_L = Wire.read();
      Temp_H = Wire.read();
      Temp_L = Wire.read();
      Wire.endTransmission();
      
      _status = (Hum_H >> 6) & 0x03;
      Hum_H = Hum_H & 0x3f;
      H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
      T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
      T_dat = T_dat / 4;
      *p_H_dat = H_dat;
      *p_T_dat = T_dat;
      DebugSerial.println("Got humidity!");
      return(_status);
}
