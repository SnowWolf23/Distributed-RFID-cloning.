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
  Serial.begin(9600);
  Serial1.begin(9600);   // Initialize Serial2 communication
  SPI.begin();           // Initialize SPI bus
  mfrc522.PCD_Init(); 
  pinMode(buzzerPin, OUTPUT);   // Initialize MFRC522 module
}

void loop() {
  // Look for new RFID cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    char uidString[10];
    sprintf(uidString, "%02X%02X%02X%02X", mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1], mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]);

    // Authenticate to Sector 1 using key A (Default key)
    // Modify the authentication key if needed
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;   // Default key (for demonstration purposes only)
    }

    int sector = 1;
    int block = 4;

    // Authenticate using Key A to read from the block
    if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)) == MFRC522::STATUS_OK) {
      byte buffer[18];
      byte size = sizeof(buffer);

      // Read the data from Sector 1 Block 4
      if (mfrc522.MIFARE_Read(block, buffer, &size) == MFRC522::STATUS_OK) {
        buffer[size] = '\0'; // Null-terminate the data

        // Convert the buffer to a single string
        char bufferString[size * 2 + 1]; // Each byte in buffer takes 2 characters (1 byte = 2 HEX characters)
        for (byte i = 0; i < size; i++) {
          sprintf(bufferString + i * 2, "%02X", buffer[i]);
        }

        // Send card data to the ESP32 via Serial2 port
        Serial.print("UID sent : ");
        Serial.println(uidString);
        Serial1.println(uidString);
        Serial.print("Data sent : ");
        Serial.println(bufferString);
        Serial1.println(bufferString);
      } else {
        Serial.println("Reading block failed.");
        delay(5000);
      }
      String data = Serial1.readStringUntil('\n');
      Serial.println("Received data from ESP32 : " + data);
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