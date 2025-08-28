#define SENSOR_PIN 25
#define BUTTON_PIN 26

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Menggunakan internal pull-up resistor
}

void loop() {
  // Baca status tombol
  if (digitalRead(BUTTON_PIN) == HIGH) { // Tombol ditekan (LOW karena menggunakan pull-up)
    delay(20); // Debouncing
    if (digitalRead(BUTTON_PIN) == LOW) { // Pastikan tombol masih ditekan setelah debouncing
      Serial.println("Tombol ditekan, mulai pengukuran...");

      float totalVoltage = 0;
      float totalAnaloge = 0;
      int dataCount = 10;
      int delayTime = 200; // 2 detik / 10 data = 200 ms per data

      for (int i = 0; i < dataCount; i++) {
        int analogValue = analogRead(SENSOR_PIN);
        float voltage = analogValue * (3.3 / 4095.0);
        totalVoltage += voltage;
        totalAnaloge += analogValue;

        Serial.print("Data ke-");
        Serial.print(i + 1);
        Serial.print(": Analog Value = ");
        Serial.print(analogValue);
        Serial.print(" | Voltage = ");
        Serial.println(voltage, 4);

        delay(delayTime);
      }

      float averageVoltage = totalVoltage / dataCount;
      float averageAnaloge = totalAnaloge / dataCount;
      Serial.print("Rata-rata Voltage = ");
      Serial.println(averageVoltage, 4);
      Serial.print("Rata-rata nilai Analog = ");
      Serial.println(averageAnaloge, 4);
      Serial.println("Pengukuran selesai.");
      delay(1000); // Delay untuk menghindari multiple deteksi tombol
    }
  }
}