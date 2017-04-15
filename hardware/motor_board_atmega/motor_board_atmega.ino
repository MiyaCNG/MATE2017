#define MC_Board

#include <SoftwareSerial.h>
#include "messenger.h"



SoftwareSerial mcSerial = SoftwareSerial(3, A4);

long last_toggle;
int last_value;

// required to allow motors to move
// must be called when controller restarts and after any error
void exitSafeStart()
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

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
  mcSerial.begin(9600);
  Serial.begin(115200);
 
  // the Simple Motor Controller must be running for at least 1 ms
  // before we try to send serial data, so we delay here for 5 ms
  delay(5);
  
  exitSafeStart();  // clear the safe-start violation and let the motor run
  
  setMotorSpeed(1, 0);
  setMotorSpeed(2, 0);
  setMotorSpeed(3, 0);
  setMotorSpeed(4, 0);
  setMotorSpeed(5, 0);
  setMotorSpeed(6, 0);
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

  char data[255];
  char key[255];
  struct Message message;
  message.data = data;
  message.key = key;
  
  if (receiveMessage(&message, 0)){
    if (message.key[0] == 't') {
      int mc_id = message.key[1] - '0';
      setMotorSpeed(mc_id, *(int *)message.data);
    }

    else {
      //DebugSerial.println("No idea what we have here...");
    }
  }   
}
