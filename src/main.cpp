#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>

// Set to 1 on the transmitting Nano and 0 on the receiving Nano.
#define NRF24_TRANSMITTER 1

constexpr uint8_t CE_PIN = 9;
constexpr uint8_t CSN_PIN = 10;
const byte RADIO_ADDRESS[6] = "NRF01";

RF24 radio(CE_PIN, CSN_PIN);

struct Message {
  uint8_t buttonPressed;
};

void setup() {
  Serial.begin(9600);

#if NRF24_TRANSMITTER
  pinMode(2, INPUT_PULLUP);
#else
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
#endif

  if (!radio.begin()) {
    Serial.println(F("NRF24 tidak terdeteksi. Periksa wiring dan catu daya."));
    while (true) {
      delay(1000);
    }
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);

#if NRF24_TRANSMITTER
  radio.openWritingPipe(RADIO_ADDRESS);
  radio.stopListening();
  Serial.println(F("Mode pengirim siap."));
#else
  radio.openReadingPipe(1, RADIO_ADDRESS);
  radio.startListening();
  Serial.println(F("Mode penerima siap."));
#endif
}

void loop() {
#if NRF24_TRANSMITTER
  Message message = {
      static_cast<uint8_t>(digitalRead(2) == LOW ? 1 : 0)};
  radio.write(&message, sizeof(message));
  delay(50);
#else
  if (radio.available()) {
    Message message;
    radio.read(&message, sizeof(message));
    digitalWrite(3, message.buttonPressed ? HIGH : LOW);
  }
#endif
}
