#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SENSOR_PIN 34  // Sesuaikan dengan pin ADC yang digunakan
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Gunakan -1 jika tidak ada pin reset

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Inisialisasi OLED
  // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Alamat I2C 0x3C biasanya default
  //   Serial.println(F("OLED tidak ditemukan!"));
  //   while (true)
  //     ;  // Berhenti di sini jika OLED tidak ditemukan
  // }

  // display.clearDisplay();
  // display.setTextSize(1);  // Ukuran teks
  // display.setTextColor(SSD1306_WHITE);
  // display.setCursor(0, 0);
  // display.println("Inisialisasi...");
  // display.display();
  // delay(1000);
}

void loop() {
  int adcValue = analogRead(SENSOR_PIN);
  float voltage = (adcValue / 1024.0) * 3.3;

  // Tampilkan ke Serial Monitor
  Serial.print("ADC Value: ");
  Serial.print(adcValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.println(" V");

  // // Tampilkan ke OLED
  // display.clearDisplay();
  // display.setCursor(0, 0);
  // display.setTextSize(1);
  // display.print("ADC Value: ");
  // display.println(adcValue);
  // display.print("Voltage  : ");
  // display.print(voltage, 3);
  // display.println(" V");
  // display.display();

  delay(500);
}
