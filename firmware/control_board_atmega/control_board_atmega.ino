
#include "settings.h"
#include "debug.h"
#include "messenger.h"
#include <Wire.h>

long last_toggle;
int last_value;

void setup() {
  initDebug();

  pinMode(70, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(71, OUTPUT);
  pinMode(72, OUTPUT);
  pinMode(73, OUTPUT);
  pinMode(6, OUTPUT);
  
  last_value = 0;
  last_toggle = millis();
  pinMode(62, OUTPUT); // Error led
  pinMode(63, OUTPUT); // Power led
  pinMode(64, OUTPUT); // Debug led
  digitalWrite(62, LOW); // Error led low
  digitalWrite(63, HIGH); // Power led high
  digitalWrite(64, LOW); // Debug led low

  // Set pullups on 485 UART rx lines
  pinMode(0, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);

  // Set all RS485 tranceivers to receive.
  configureBus(0, 0);
  configureBus(0, 1);
  configureBus(0, 2);

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);  
  Serial3.begin(115200);

  Wire.begin(); // i2c bus

  logInfo("Control Board Atmega initialized.");
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
    if (receiveMessage(&message, i)){

      if (message.prefix == 'd') {
        resendDebugMessage(&message);
        continue;
      }
      
      //logDebug("Got message: here it is!");
      //reportMessage(&message);
      
      if (message.prefix == 'r') { // Return value to forward
        sendMessage(&message, 3);
      }
      else if (message.key[0] == 'a') { // Get alive message.
        if (message.key[1] == '0') { // Master
          data[0] = 0xFF;
          struct Message response = {'r', message.key, message.keyLen, data, 1};
          sendMessage(&response, 3);
        }
        else if (message.key[1] == '1'){ // Slave 1
          //logInfo("Got a message!");
          sendMessage(&message, 0);
        }
        else if (message.key[1] == '2') // Slave 2
          sendMessage(&message, 1);
        else if (message.key[1] == '3') // Slave 3
          sendMessage(&message, 2);
      }
      else if (message.key[0] == 't' || message.key[0] == 's' || message.key[0] == 'r') { // Set thruster, switch, or reset motor controllers
        sendMessage(&message, 0);
      }
      else if (message.key[0] == 'p') { // Set panopticon
        sendMessage(&message, 1);
      }
      else if (message.key[0] == 'v') { // Get voltage
        struct Volts {
          int v_48v = analogRead(A0);
          int v_24vA = analogRead(A1);
          int v_24vB = analogRead(A2);
          int v_5vMain = analogRead(A3);
          int v_5vPeriph = analogRead(A4);
        } volts;
        struct Message response = {'r', message.key, message.keyLen, (char *) &volts, sizeof(volts)};
        sendMessage(&response, 3);
      }
      else if (message.key[0] == 'h') { // Get Humidity and Temperature
        unsigned int H_dat;
        unsigned int T_dat;
        fetch_humidity_temperature(&H_dat, &T_dat);
        struct HandT {
          float RH;
          float T_C;
        } data;
        data.RH = (float) H_dat * 6.10e-3;
        data.T_C = (float) T_dat * 1.007e-2 - 40.0;
        logInfo("H: " + String(data.RH));
        logInfo("T: " + String(data.T_C));
        struct Message response = {'r', message.key, message.keyLen, (char *) &data, sizeof(data)};
        logDebug("About to send a response: here it is!");
        reportMessage(&response);
        sendMessage(&response, 3);
      }
      else { // Unknown request
        logInfo("Wat. No idea what we have here...");
        reportMessage(&message);
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
      return(_status);
}
