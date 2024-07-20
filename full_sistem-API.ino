#include <ML8511.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const int num_readings = 10;

const int relayPin = D3;                // Pin relay terhubung
const int buttonPin = D4;               // Pin tombol terhubung
int buttonState;                        // Variabel untuk membaca status tombol
int lastButtonState = LOW;              // Status tombol sebelumnya
int relayState = HIGH;                  // Status awal relay (mati)
bool buttonPressed = false;             // Variabel untuk melacak apakah tombol ditekan
unsigned long relayOnTime;              // Waktu saat relay dihidupkan
const unsigned long onDuration = 2000;  // Durasi relay tetap hidup (5 detik)
bool sensorActive = false;

#define pinSensor A0 
#define ENPIN D7

const char* ssid = "Homee";         // Ganti dengan SSID jaringan Wi-Fi Anda
const char* password = "1sampaisekian"; // Ganti dengan password jaringan Wi-Fi Anda

const char* uvDataServerUrl = "https://formd-research.000webhostapp.com/sensor"; // Ganti dengan URL endpoint API Anda untuk UV data
const char* sampleDataServerUrl = "https://formd-research.000webhostapp.com/samples"; // Ganti dengan URL endpoint API Anda untuk sample data

LiquidCrystal_I2C lcd(0x27, 16, 2);
ML8511 light(pinSensor, ENPIN);

WiFiClientSecure wifiClient;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);           // Atur pin relay sebagai output
  pinMode(buttonPin, INPUT);           // Atur pin tombol sebagai input
  digitalWrite(relayPin, relayState);  // Matikan relay pada awalnya
  
  light.enable();
  light.setDUVfactor(1.80);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set the certificate for HTTPS
  wifiClient.setInsecure(); // Gunakan ini jika Anda ingin melewati verifikasi sertifikat (tidak disarankan untuk produksi)
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
    float UV_values[num_readings];
    float voltage_values[num_readings];
    for (int i = 0; i < num_readings; i++) {
      int sensorValue = analogRead(pinSensor);
      float voltage = sensorValue * (3.3 / 1023.0);  // Konversi nilai ADC menjadi tegangan (0-3.3V)
      float UV = light.getUV();

      UV_values[i] = UV;
      voltage_values[i] = voltage;

      delay(200);  // Delay antara pengukuran
    }

    float UV_total = 0;
    float V_total = 0;
    for (int i = 0; i < num_readings; i++) {
      UV_total += UV_values[i];
      V_total += voltage_values[i];
    }

    float UV_average = UV_total / num_readings;
    float V_average = V_total / num_readings;
    float UV_start = 5.5
    float absorbance = calculateAbsorbance(UV_average); // Ganti dengan perhitungan absorbansi yang sesuai

    Serial.print("Rata-rata UV: ");
    Serial.println(UV_average, 4);
    Serial.print("Rata-rata Tegangan: ");
    Serial.println(V_average, 4);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UV Avg: ");
    lcd.print(UV_average, 3);
    lcd.print(" mW/cm^2");
    lcd.setCursor(0, 1);
    lcd.print("V Avg: ");
    lcd.print(V_average, 3);
    lcd.print(" V");

    // Kirim data sampel ke API
    sendSampleDataToAPI(UV_average, V_average, absorbance);

    // Kirim data UV dan Tegangan ke API
    sendUVDataToAPI(UV_values, voltage_values, num_readings);
  }
}

void sendUVDataToAPI(float UV_values[], float voltage_values[], int length) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, uvDataServerUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"uv_reading\": [";
    for (int i = 0; i < length; i++) {
      jsonData += String(UV_values[i], 4);
      if (i < length - 1) {
        jsonData += ",";
      }
    }
    jsonData += "], \"v_reading\": [";
    for (int i = 0; i < length; i++) {
      jsonData += String(voltage_values[i], 4);
      if (i < length - 1) {
        jsonData += ",";
      }
    }
    jsonData += "]}";

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error in sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void sendSampleDataToAPI(float UV_average, float V_average, float absorbance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, sampleDataServerUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"average_intensity\": " + String(UV_average, 4) + ", \"average_voltage\": " + String(V_average, 4) + ", \"absorbance\": " + String(absorbance, 4) + "}";

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error in sending POST: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

float calculateAbsorbance(float UV_average) {
  return log10( UV_start / UV_average);
}
