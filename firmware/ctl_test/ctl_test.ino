#include <SoftwareSerial.h>
SoftwareSerial DebugSerial(MISO, MOSI);

void setup() {
  pinMode(62, OUTPUT);
  pinMode(63, OUTPUT);
  pinMode(64, OUTPUT);
  Serial3.begin(115200);
  DebugSerial.begin(115200);
  DebugSerial.println("ctl_test script!!!");
}

void loop() {
  digitalWrite(62, HIGH);
  digitalWrite(63, HIGH);
  digitalWrite(64, HIGH);
  Serial3.println("high");
  DebugSerial.println("high");
  delay(1000);
  digitalWrite(62, LOW);
  digitalWrite(63, LOW);
  digitalWrite(64, LOW);
  Serial3.println("low");
  DebugSerial.println("low");
  delay(1000);
}
