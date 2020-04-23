#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5         
#define SS_PIN          4        

#define FIREBASE_HOST "miniproject8-iot.firebaseio.com/"
#define FIREBASE_AUTH "tACkCnETiaLKE8texLNpyJQ8OcFtgrHeBrjLkGLb"
#define WIFI_SSID "VincentW"
#define WIFI_PASSWORD "270699161097110762"

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

FirebaseData fbData;

String path = "/Node1";
String RFIDcode = " ";  //sent to firebase
int temp[12]; // store code temporarily

/////////////////////////
int waterPin = A0; 
int nilaiKetinggian = 0; // water value
/////////////////////////

void setup() {
  initWifi();
	Serial.begin(9600);		
	while (!Serial);		
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
   initWifi();
	delay(4);				
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) { // (true, if RFID tag/card is present ) PICC = Proximity Integrated Circuit Card
    if(mfrc522.PICC_ReadCardSerial()) { // true, if RFID tag/card was read
      Serial.print("RFID TAG ID:");
      
      for (byte i = 0; i < mfrc522.uid.size; ++i) { // read id (in parts)
        temp[i] = mfrc522.uid.uidByte[i];
        Serial.print(temp[i]); // print id as hex values
        Serial.print(" "); // add space between hex blocks to increase readability
        
        
      }
      for(int i = 0; i < mfrc522.uid.size ; i++){
        RFIDcode = RFIDcode + (String)temp[i] + " ";
      }
      Serial.println(); // Print out of id is complete.
       Firebase.setString(fbData,path + "/RFIDcode",RFIDcode);
       RFIDcode = "";
    }
  }
  
  //Water Sensor Coding
    int ketinggian = readSensor();
    Firebase.setInt(fbData, path + "/WaterHeight",ketinggian);
//  Serial.print("Ketinggian Air:");
//  Serial.println(ketinggian);
//  
//  delay(1000);


}

int readSensor(){
  nilaiKetinggian = analogRead(waterPin);
  return nilaiKetinggian;
}

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //set database read timeout to 1 minute (max 15 minutes )
  Firebase.setReadTimeout(fbData,1000 * 60);
  Firebase.setwriteSizeLimit(fbData, "tiny");
}
