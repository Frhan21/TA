#include <ML8511.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const int num_readings = 10;

const int relayPin = D3;                // Pin relay terhubung
const int buttonPin = D4;               // Pin tombol terhubung
int buttonState;                        // Variabel untuk membaca status tombol
int lastButtonState = LOW;              // Status tombol sebelumnya
int relayState = HIGH;                   // Status awal relay (mati)
bool buttonPressed = false;             // Variabel untuk melacak apakah tombol ditekan
unsigned long relayOnTime;              // Waktu saat relay dihidupkan
const unsigned long onDuration = 2000;  // Durasi relay tetap hidup (5 detik)
bool sensorActive = false;

#define pinSensor A0 
#define ENPIN D7


LiquidCrystal_I2C lcd(0x27, 16, 2);
ML8511 light(pinSensor, ENPIN);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(relayPin, OUTPUT);           // Atur pin relay sebagai output
  pinMode(buttonPin, INPUT);           // Atur pin tombol sebagai input
  digitalWrite(relayPin, relayState);  // Matikan relay pada awalnya

  
  light.enable();
  light.setDUVfactor(1.80);
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Formalin Detection");
  delay(2000);
  lcd.clear();
}

void loop() {
  buttonState = digitalRead(buttonPin);  // Baca status tombol

  // Jika tombol berubah dari dilepaskan ke ditekan
  if (buttonState == HIGH && lastButtonState == LOW && !buttonPressed) {
    buttonPressed = true;

    Serial.println("Button pressed. Relay ON");

    relayState = LOW;                   // Hidupkan relay
    digitalWrite(relayPin, relayState);  // Aktifkan relay
    relayOnTime = millis();              // Simpan waktu saat relay dihidupkan

    Serial.println("LCD updated: Relay Hidup, LED Hidup");
    sensorActive = true;
  }

  // Jika tombol berubah dari ditekan ke dilepaskan
  if (buttonState == LOW && buttonPressed) {
    buttonPressed = false;
  }

  // Matikan relay setelah durasi tertentu
  if (relayState == LOW && (millis() - relayOnTime >= onDuration)) {

    Serial.println("Relay OFF");
    relayState = HIGH;                    // Matikan relay
    digitalWrite(relayPin, relayState);  // Nonaktifkan relay
    sensorActive = false;

    Serial.println("LCD updated: Relay Mati, LED Mati");
  }

  lastButtonState = buttonState;  // Simpan status tombol sebelumnya

  if (sensorActive) {
    // Lakukan pengukuran jika sensor aktif
    float UV_total = 0;
    float V_total = 0;
    for (int i = 0; i < num_readings; i++) {
      int sensorValue = analogRead(pinSensor);
      float voltage = sensorValue * (3.3 / 1023.0);  // Konversi nilai ADC menjadi tegangan (0-3.3V)
      float UV = light.getUV();

      UV_total += UV;
      V_total += voltage;

      delay(200);  // Delay antara pengukuran
    }

    float UV_average = UV_total / num_readings;
    float V_average = V_total / num_readings;

    // Convert Voltage to Concentration and Absorbance Formaldehyde
    float concentration = (V_average - 1.1095) / -0.0002; 
    float absorbance = (-5.8477*V_average) + 6.6826;
    Serial.print("Rata-rata UV: ");
    Serial.println(UV_average, 4);
    Serial.print("Rata-rata Tegangan: ");
    Serial.println(V_average, 4);
    Serial.print("Konsentrasi Formalin: ");
    Serial.print(concentration, 3);
    Serial.print(" ppm, ");
    Serial.print("Absorbansi sampel: ");
    Serial.println(absorbance, 3);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UV Avg: ");
    lcd.print(UV_average, 3);
    lcd.print(" mW/cm^2");
    lcd.setCursor(0, 1);
    lcd.print("V Avg: ");
    lcd.print(V_average, 3);
    lcd.print(" V");
    delay(2000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Konsentrasi: ");
    lcd.print(concentration, 3);
    lcd.setCursor(0, 1);
    lcd.print("ABS: ");
    lcd.print(absorbance, 3);

  }
}
