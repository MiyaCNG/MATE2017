#include "settings.h"
#include "debug.h"
#include "messenger.h"

#include <Servo.h>
#include <SoftwareSerial.h>

long last_toggle;
int last_value;

Servo servoa;
Servo servob;
Servo servoc;

void setup() {
  initDebug();

  servoa.attach(5);
  servob.attach(6);
  servoc.attach(7);

  servoa.write(90);
  servob.write(90);
  servoc.write(90);
  
  pinMode(8, OUTPUT); //LED 0
  pinMode(9, OUTPUT); //LED 1
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

  pinMode(2, OUTPUT); // RS485 direction
  digitalWrite(2, LOW);

  Serial.begin(115200);
  
  logInfo("Panopticon Board Atmega initialized.");
}

void loop() {
  if (millis() - last_toggle > 100) {
    last_value = !last_value;
    digitalWrite(8, last_value);
    last_toggle = millis();
  }
  char data[255];
  char key[255];
  struct Message message;
  message.data = data;
  message.key = key;
  
  if (receiveMessage(&message, 0)){
    if (message.key[0] == 'a' && message.key[1] == '2') {
      data[0] = 0xFF;
      struct Message response = {'r', message.key, message.keyLen, data, 1};
      sendMessage(&response, 0);
    }
    else if (message.key[0] == 'p') {
      if (message.key[1] == '0')
        servoa.write(*(int *)message.data);
      else if (message.key[1] == '1')
        servob.write(*(int *)message.data);
      else if (message.key[1] == '2')
        servoc.write(*(int *)message.data);
    }
    else {
      logInfo("No idea what we have here...");
    }
  }   
}
