#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define TCA_ADDRESS 0x70
#define BNO055_ADDRESS 0x28
#define TCA_CHANNEL 0

Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS);

float yawOffset = 0, pitchOffset = 0, rollOffset = 0;
bool calibrated = false;

void tca_select(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDRESS);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void setup() {
  Wire.begin(2, 3); // SDA, SCL
  Serial.begin(115200);
  delay(1000);

  tca_select(TCA_CHANNEL);
  delay(10);

  if (!bno.begin()) {
    Serial.println("BNO055 ist nicht gefunden.");
    while (1);
  }

  bno.setExtCrystalUse(true);

  // Automatische reset von Koordinaten auf 0,0,0
  sensors_event_t event;
  bno.getEvent(&event);  // Erhalten von ersten Daten
  yawOffset = event.orientation.x;
  pitchOffset = event.orientation.y;
  rollOffset = event.orientation.z;
  
  calibrated = true;  // Wenn die Kalibrierung ist erfolgreich
  
  Serial.println("READY");  // 
  Serial.println("Orientation initialisiert mit 0,0,0");

  // Ausgabe von Koordinaten
  Serial.print("Yaw: "); Serial.print(event.orientation.x); 
  Serial.print(" Pitch: "); Serial.print(event.orientation.y); 
  Serial.print(" Roll: "); Serial.println(event.orientation.z);
}

void loop() {
  tca_select(TCA_CHANNEL);
  sensors_event_t event;
  bno.getEvent(&event);

  if(Serial.available())
  {
    char c = Serial.read();
    if(c == 'r')
    {
      yawOffset = event.orientation.x;
      pitchOffset = event.orientation.y;
      rollOffset = event.orientation.z;
      calibrated = true;
      Serial.println("Reset");
    }
  }
  // Вerechnungen von Ecken mit Orientation-reset
  float yaw = event.orientation.x - yawOffset;
  float pitch = event.orientation.y - pitchOffset;
  float roll = event.orientation.z - rollOffset;

  Serial.print(yaw); Serial.print(",");
  Serial.print(pitch); Serial.print(",");
  Serial.println(roll);

  delay(20); // ~100 Hz
}
