#include "BluetoothSerial.h"
#include <SPI.h>
#include <MFRC522.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

#define RST_PIN         22
#define SS_PIN          21
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

/////////////////////////
String message = "";
//String RFIDcode = "";
int ledGranted = 12;
int ledDenied = 14;
String RFID;
String authorized = "56 4A F0 2B ";
void setup() {

  Serial.begin(9600);
  SerialBT.begin("ESP32"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");


  pinMode(ledGranted, OUTPUT);
  pinMode(ledDenied, OUTPUT);

  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  //Terima Data dari Bluetooth
  if (SerialBT.available()) {
    char receivedMessage = SerialBT.read();
    if (receivedMessage != '\n') {
      message = message + String(receivedMessage);
    }
    else {
      message = "";
    }
    Serial.write(receivedMessage);
  }


  if (message == "Granted") {
    digitalWrite(ledGranted, HIGH);
    digitalWrite(ledDenied, LOW);
    delay(2000);
    digitalWrite(ledGranted, LOW);
  } else if (message == "Denied") {
    digitalWrite(ledGranted, LOW);
    digitalWrite(ledDenied, HIGH);
    delay(2000);
    digitalWrite(ledDenied, LOW);
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

//RFID
  Serial.print("RFID: :");
  for (byte i = 0; i < mfrc522.uid.size; ++i) { // read id (in parts)
    RFID = RFID + String(mfrc522.uid.uidByte[i], HEX) + " ";
//    Serial.print(RFID);
  }
  Serial.println(); // Print out of id is complete.
  Serial.println("RFID setelah di proses: " + RFID);
  RFID.toUpperCase();

  if (RFID == authorized) {
    digitalWrite(ledGranted, HIGH);
    digitalWrite(ledDenied, LOW);
    SerialBT.println("RFID: " + RFID);
    SerialBT.println("Access Granted");
    delay(3000);
    digitalWrite(ledGranted, LOW);
  }
  else {
    digitalWrite(ledDenied, HIGH);
    digitalWrite(ledGranted, LOW);
    SerialBT.println("RFID: " + RFID);
    SerialBT.println("Access Denied");
    delay(3000);
    digitalWrite(ledDenied, LOW);
  }


  RFID = "";
  mfrc522.PICC_HaltA();

}
