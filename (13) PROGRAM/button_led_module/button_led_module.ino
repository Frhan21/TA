#define BUTTON_PIN 18  // GPIO connected to the button
#define RELAY_PIN 25   // GPIO connected to the relay module

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Enable internal pull-up resistor
    pinMode(RELAY_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {  // If button is pressed
        Serial.println("Button Pressed! Relay ON");
        digitalWrite(RELAY_PIN, LOW);  // Turn relay ON
        delay(2000);  // Keep relay ON for 2 seconds
        digitalWrite(RELAY_PIN, HIGH);  // Turn relay OFF
        Serial.println("Relay OFF");
    }
}
