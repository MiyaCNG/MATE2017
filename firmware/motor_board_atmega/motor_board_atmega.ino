#include "settings.h"
#include "debug.h"
#include "messenger.h"

#include <SoftwareSerial.h>


SoftwareSerial mcSerial = SoftwareSerial(3, A4);

long last_toggle;
int last_value;

// required to allow motors to move
// must be called when controller restarts and after any error
void clearErrors()
{
  mcSerial.write(0x83);
}
 
// speed should be a number from -3200 to 3200
void setMotorSpeed(int id, int speed)
{
  mcSerial.write(0xAA);
  mcSerial.write(id);
  if (speed < 0)
  {
    mcSerial.write(0x06);  // motor reverse command
    speed = -speed;  // make speed positive
  }
  else
  {
    mcSerial.write(0x05);  // motor forward command
  }
  mcSerial.write(speed & 0x1F);
  mcSerial.write(speed >> 5);
}

void setLowSide(int index, int value) {
  switch (index) {
    case 0:
      digitalWrite(LS_0, value);
      break;    
    case 1:
      digitalWrite(LS_1, value);
      break;    
    case 2:
      digitalWrite(LS_2, value);
      break;
    case 3:
      digitalWrite(LS_3, value);
      break;
    case 4:
      digitalWrite(LS_4, value);
      break;
    case 5:
      digitalWrite(LS_5, value);
      break;
    case 6:
      digitalWrite(LS_6, value);
      break;
    case 7:
      digitalWrite(LS_7, value);
      break;
  }
}

void setup() {
  initDebug();
  
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  pinMode(LS_0, OUTPUT);
  pinMode(LS_1, OUTPUT);
  pinMode(LS_2, OUTPUT);
  pinMode(LS_3, OUTPUT);
  pinMode(LS_4, OUTPUT);
  pinMode(LS_5, OUTPUT);
  pinMode(LS_6, OUTPUT);
  pinMode(LS_7, OUTPUT);
  digitalWrite(LS_0, LOW);
  digitalWrite(LS_1, LOW);
  digitalWrite(LS_2, LOW);
  digitalWrite(LS_3, LOW);
  digitalWrite(LS_4, LOW);
  digitalWrite(LS_5, LOW);
  digitalWrite(LS_6, LOW);
  digitalWrite(LS_7, LOW);
  
  mcSerial.begin(115200);
  Serial.begin(115200);
 
  // the Simple Motor Controller must be running for at least 1 ms
  // before we try to send serial data, so we delay here for 5 ms
  delay(5);
    
  setMotorSpeed(1, 0);
  setMotorSpeed(2, 0);
  setMotorSpeed(3, 0);
  setMotorSpeed(4, 0);
  setMotorSpeed(5, 0);
  setMotorSpeed(6, 0);

  logInfo("Motor Board Atmega initialized.");
}

void loop() {
//  if (millis() - last_toggle > 1000){
//    last_value = !last_value;
//    if (last_value)
//      setMotorSpeed(4, 100);
//    else
//      setMotorSpeed(4, 0);
//    last_toggle = millis();
//  }
  //logInfo("Hi!");

  char data[255];
  char key[255];
  struct Message message;
  message.data = data;
  message.key = key;
  
  if (receiveMessage(&message, 0)){
    //logDebug("Got message: here it is!");
    //reportMessage(&message);
    if (message.key[0] == 'a' && message.key[1] == '1') {
      data[0] = 0xFF;
      struct Message response = {'r', message.key, message.keyLen, data, 1};
      sendMessage(&response, 0);
    } else if (message.key[0] == 't') {
      int mc_id = message.key[1] - '0';
      setMotorSpeed(mc_id, *(int *)message.data);
    }
    else if (message.key[0] == 's') {
      int s_id = message.key[1] - '0';
      setLowSide(s_id, *(int *)message.data);
      //logInfo("Set the low side switch!!");
    }
    else if (message.key[0] == 'r') {
      clearErrors();
    }
    else {
      logInfo("No idea what we have here...");
      reportMessage(&message);
    }
  }   
}
