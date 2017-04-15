#include <Arduino.h>
#include "settings.h"
#ifndef Messenger
#define Messenger

struct Message {
  char prefix;
  char * key;
  char keyLen;
  char * data;
  char dataLen;
};

void msgSetDebug(SoftwareSerial*);
void reportMessage(struct Message *);
void configureBus(int, int);
void writeToBus(char *, int, int);
int availableFromBus(int);
int readBytesFromBus(char *, int, int);
void sendMessage(struct Message *, int busId);
int receiveMessage(struct Message *, int busId);

#endif

