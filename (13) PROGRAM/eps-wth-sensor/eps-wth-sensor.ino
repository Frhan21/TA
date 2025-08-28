#include <Wire.h>

#define relayPin 25 
#define buttonPin 18 
#define sensorPin 34

int buttonState;
int lastButtonState = LOW;
int relayState = HIGH;
bool buttonPressed = false;
unsigned long relayOnTime;
const unsigned relayTime = 2500;
bool sensorActive = false;
const int num_readings = 10;

bool measurementStarted = false;  // Penanda untuk menambahkan delay awal pengukuran

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  digitalWrite(relayPin, relayState);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // Jika tombol ditekan
  if (buttonState == LOW && lastButtonState == HIGH && !buttonPressed) {
    buttonPressed = true;

    Serial.println("Button pressed. Relay ON");

    relayState = LOW;
    digitalWrite(relayPin, relayState);
    relayOnTime = millis();

    Serial.println("LCD updated: Relay Hidup");
    sensorActive = true;
    measurementStarted = false;  // Reset flag setiap kali tombol ditekan
  }

  // Jika tombol dilepas
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
  }

  // Matikan relay setelah durasi tertentu
  if (relayState == LOW && (millis() - relayOnTime >= relayTime)) {
    Serial.println("Relay OFF");
    relayState = HIGH;
    digitalWrite(relayPin, relayState);
    sensorActive = false;

    Serial.println("LCD updated: Relay Mati");
  }

  lastButtonState = buttonState;

  // Mulai pengukuran setelah delay 0.5 detik dari waktu relay dihidupkan
  if (sensorActive && !measurementStarted && (millis() - relayOnTime >= 500)) {
    measurementStarted = true;  // Agar hanya dilakukan sekali per siklus aktif relay

    Serial.println("Mulai pengukuran sensor setelah delay 0.5 detik");

    float vTotal = 0;
    float analogTotal = 0;

    for (int i = 0; i < num_readings; i++) {
      int sensorValue = analogRead(sensorPin);
      float voltage = sensorValue * (3.3 / 4095.0);

      vTotal += voltage;
      analogTotal += sensorValue;

      Serial.print("Data ke-");
      Serial.print(i + 1);
      Serial.print(" : Nilai Analog = ");
      Serial.print(sensorValue);
      Serial.print(" | Tegangan = ");
      Serial.println(voltage, 4);

      delay(200);
    }

    float vAvg = vTotal / num_readings;
    float analogAvg = analogTotal / num_readings;
    
    Serial.print(" | Rata-rata Nilai Analog = ");
    Serial.print(analogAvg);
    Serial.print(" | Rata-rata Tegangan = ");
    Serial.println(vAvg, 4);
  }
}
