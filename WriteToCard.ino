#include <SPI.h>
#include <MFRC522.h>
#include <pitches.h>

#define SS_PIN 10   // Arduino pin connected to MFRC522's SS pin
#define RST_PIN 9   // Arduino pin connected to MFRC522's RST pin

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create an instance of the MFRC522 library

const int buzzerPin = 8; // Change this to the pin connected to the buzzer
const int buzzerMelody[] = {
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5
};

const int melodyDuration[] = {
  4, 4, 4, 8
};
void setup() {
  Serial.begin(9600);   // Initialize serial communication
  Serial1.begin(9600);
  SPI.begin();          // Initialize SPI bus
  mfrc522.PCD_Init();   // Initialize MFRC522 module
}
void loop(){
  String hexString = Serial1.readStringUntil('\n');
  Serial.println("The data received is :- "+hexString);
  Serial.println("Bring the card closer");
  // Look for new RFID cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Authenticate using the correct key for Sector 1 (use your actual key here)
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;   // Replace this with your actual key
    }
    byte sector = 2;
    byte block = 8;

    // Authenticate using Key A to write to the block
    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
      //byte newData[16];  // Replace this with your new data
      char cString[hexString.length() + 1]; // Add 1 for null-terminator
      hexString.toCharArray(cString, sizeof(cString));
      size_t arrayLength = 16;
      byte byteArray[arrayLength];
      for (size_t i = 0; i < arrayLength; i++) {
        char hex[3] = { hexString[i * 2], hexString[i * 2 + 1], '\0' };
        byteArray[i] = (byte)strtoul(hex, NULL, 16);
        Serial.println(byteArray[i]);
      }
      /*for (byte i = 0; i < 16; i++) {
        Serial.println(i,HEX);
        newData[i] = i;  // For example, fill the block with 0, 1, 2, ..., 15
      }*/

      // Write the new data to the block
      if (mfrc522.MIFARE_Write(block, byteArray, 16) == MFRC522::STATUS_OK) {
        Serial.println("Data successfully written to block!\n");
      } else {
        Serial.println("Writing block failed.");
      }
      playBuzzerMelody(2000);
      // Halt PICC (card) and stop encryption on PCD (reader)
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    } else {
      Serial.println("Authentication failed.");
    }
  }
}
void playBuzzerMelody(unsigned long duration) {
  for (int i = 0; i < sizeof(buzzerMelody) / sizeof(buzzerMelody[0]); i++) {
    int noteDurationMs = 1000 / melodyDuration[i];
    tone(buzzerPin, buzzerMelody[i], noteDurationMs);
    delay(noteDurationMs);
    noTone(buzzerPin);
    delay(50);
  }

  delay(duration);
}
