#include "debug.h"

void initDebug() {
  #ifdef DEBUG_OVER_SERIAL
  debugSerial = new SoftwareSerial(DEBUG_TX, DEBUG_RX);
  debugSerial->begin(DEBUG_BAUD);
  #endif
}

void resendDebugMessage(struct Message* message) {
  #ifdef DEBUG_OVER_SERIAL
  char data[255];
  memcpy(data, (char *) "Message from ", 13);
  int m_len = 13;
  memcpy(data+m_len, message->key, message->keyLen);
  m_len += message->keyLen;
  memcpy(data+m_len, (char *) ": ", 2);
  m_len += 2;
  memcpy(data+m_len, message->data, message->dataLen);
  m_len += message->dataLen;
  memcpy(data+m_len, (char *) "\n\r\0", 3);
  m_len += 3;
  debugSerial->write(data, m_len);
  #endif
  
  #ifdef DEBUG_OVER_MESSENGER
  sendMessage(message, DEBUG_OVER_MESSENGER_BUS);
  #endif
}

void sendDebugMessage(String msg, int level) {
  if (level >= DEBUG_LEVEL) {
    #ifdef DEBUG_OVER_SERIAL
    debugSerial->println(msg);
    #endif
    #ifdef DEBUG_OVER_MESSENGER
    struct Message message = {'d', BOARD_NAME, strlen(BOARD_NAME), (char *) msg.c_str(), msg.length()};
    sendMessage(&message, DEBUG_OVER_MESSENGER_BUS);
    #endif
  }
}

void logDebug(String msg) {
  sendDebugMessage(msg, msgDEBUG);
}

void logInfo(String msg) {
  sendDebugMessage(msg, msgINFO);
}

void logWarn(String msg) {
  sendDebugMessage(msg, msgWARN);
}
