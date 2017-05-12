#include <Arduino.h>
#include "settings.h"
#include "messenger.h"

#ifndef Debug
#define Debug

#define msgDEBUG 0
#define msgINFO 1
#define msgWARN 2

#ifdef DEBUG_OVER_SERIAL
#include <SoftwareSerial.h>
static SoftwareSerial *debugSerial;
#endif

void initDebug();
void resendDebugMessage(struct Message*);
void sendDebugMessage(String, int);
void logDebug(String);
void logInfo(String);
void logWarn(String);

#endif

