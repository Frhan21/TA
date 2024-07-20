// #define pinLED D0
#define pinButton D8
#define pinRelay D0 

int buttonState;
int oldButton = 0;
int state = 0;
unsigned long lastButtonPressTime = 0;
const unsigned long autoOffDelay = 5000; // Waktu tunggu sebelum lampu mati otomatis (dalam milidetik)

void setup() {
  Serial.begin(9600);
  // pinMode(pinLED, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(pinButton);

  // Saat tombol ditekan
  if (!buttonState && !oldButton) {
    if (state == 0) {
      // Hidupkan lampu
      // digitalWrite(pinLED, HIGH);
      digitalWrite(pinRelay, HIGH);
      Serial.println("LED ON");
      Serial.println("Sensor ON");
      state = 1;
      // Simpan waktu terakhir tombol ditekan
      lastButtonPressTime = millis();
    }
    oldButton = 1;
  } else if (buttonState && oldButton) {
    oldButton = 0;
  }

  // Matikan lampu secara otomatis setelah 5 detik
  if (state == 1 && millis() - lastButtonPressTime >= autoOffDelay) {
    // Matikan lampu
    // digitalWrite(pinLED, LOW);
    digitalWrite(pinRelay, LOW);
    Serial.println("LED OFF (Auto-off)");
    Serial.println("Sensor OFF");
    state = 0;
  }
}
