#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <MQTT.h>
#include <PubSubClient.h>
WiFiClient net;
MQTTClient clients;
#define RST_PIN         5
#define SS_PIN          4
#define RELAY           2
//const char ssid[] = "Password";
//const char pass[] = "sensor";
const char ssid[] = "VincentW";
const char pass[] = "270699161097110762";



MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
//PubSubClient client("broker.shiftr.io", 1883, messageReceived, net);
//////////////////////////////////////////////////////////////////////// USED VARIABLES
String message = "";
int buzzer = 10;
int ledGranted = 16;
int ledDenied = 3;
String registered = "";
String authorized = "56 4A F0 2B "; // AUTHORIZED ID
String tempId = "";
int ldrPin = A0;
int ldrVal = 0;
int ldr = 0;
int ldrOn = 0; //CHECKING LDR PURPOSE
////////////////////////////////////////////////////////////////////////

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!clients.connect("Broker_IoT", "vincent_wijaya27", "vincent123")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");




  clients.subscribe("/smarthome");

  // client.unsubscribe("/smarthome");
}

void setup() {

  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(ledGranted, OUTPUT);
  pinMode(ledDenied, OUTPUT);


  WiFi.begin(ssid, pass);

  clients.begin("broker.shiftr.io", net);
  connect();
  digitalWrite(buzzer, LOW);
  digitalWrite(RELAY, HIGH);// KEEP TURNING OFF THE RELAY FROM THE BEGINNING

  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);
  Serial.println(F("Scan Your Activation Card"));
  clients.onMessage(messageReceived);
}

void loop() {
  //  client.loop();
  clients.loop();
  delay(10);

  ////////////////////////////////////////////////////////////////////////
  //LDR SYSTEM ( TURNED ON IF GOT THE MESSAGE FROM USER )
  ldr = ldrFunc();
  if (ldrOn == 1) {
    if (ldr > 20) {//CHECK THE BRIGHTNESS OF THE ENVIRONMENT
      digitalWrite(buzzer, HIGH);//TURNED OFF WHEN GOT A PARTICULAR MESSAGE FROM USER
    }
  }
  ////////////////////////////////////////////////////////////////////////
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (!clients.connected()) {
    connect();
  }
  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  //ID CARD CONVERT ( TO UPPERCASE )
  Serial.print("RFID: :");
  for (byte i = 0; i < mfrc522.uid.size; ++i) { // read id (in parts)
    tempId = tempId + String(mfrc522.uid.uidByte[i], HEX) + " ";
    //    Serial.print(RFID);
  }
  Serial.println(); // Print out of id is complete.
  Serial.println("Welcome Vincent. Your ID is:  " + tempId);
  tempId.toUpperCase();
  ////////////////////////////////////////////////////////////////////////

  //PROCESS OF TURN ON AND OFF THE SYSTEMS
  if (tempId != registered) { // CHECK IF EVER TAP THE CARD ( IF NEVER TAP THE CARD )
    //SAVE THE CARD ID AND SYSTEMS CAN BE RAN
    registered = tempId;
    tempId = "";
  }
  else { // IF EVER TAP THE CARD, THE SYSTEMS ARE TURNED OFF
    registered = "";
    tempId = "";
  }


  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if ((payload == "1" || payload == "0") && registered == authorized) { // IF THE CARD HAS BEEN TAPED AND ID SAVED
    //TURN ON THE GRANTED LED AND BUZZER FOR 500MS AND THE BUZZER TURNED OFF
    digitalWrite(ledGranted, HIGH);
    digitalWrite(ledDenied, LOW);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    //TURN ON THE RELAY(OPEN THE DOOR) FOR 5S AND GRANTED LED, RELAY TURNED OFF(CLOSE THE DOOR)
    digitalWrite(RELAY, LOW);
    Serial.println("RELAY ACTIVATED");
    delay(5000);
    digitalWrite(ledGranted, LOW);
    digitalWrite(RELAY, HIGH);
    Serial.println("RELAY DEACTIVATED");
    Serial.flush();
  }
  //COMMANDS THAT USERS DO TO INTERACT WITH THE SYSTEM
  else if (payload == "on") { // FOR SECURITY REASON AND CAN BE AS AN ALARM
    digitalWrite(buzzer, HIGH);
    //    client.publish("smarthome", "Buzzer On");
  }
  else if (payload == "off") { // FOR SECURITY REASON AND TO TURN OFF THE BUZZER
    digitalWrite(buzzer, LOW);
    //    client.publish("smarthome", "Buzzer Off");
  }
  else if (payload == "ldron") { // TO TURN ON THE LDR SYSTEM ( NEVER STOP BUZZING UNTIL "OFF" MESSAGE RECEIVED WHEN THE LDR ENVIRONMENT HAS < 20 OF BRIGHTNESS
    ldrOn = 1;
    //    client.publish("smarthome", "Ldr System On");
  }
  else if (payload == "ldroff") { // LDR SYSTEM CAN ONLY NE TURNED OFF WHEN THE BUZZING SOUND DISAPPEARED
    ldrOn = 0;
    //    client.publish("smarthome", "Ldr System Off");
  }
  else { // OTHER UNKNOWN COMMANDS DO THESE BELOW ACTIVITY
    Serial.println("Input or ID maybe be UNKNOWN");
    digitalWrite(ledGranted, LOW);
    digitalWrite(ledDenied, HIGH);
    delay(2000);
    digitalWrite(ledDenied, LOW);
    Serial.flush();

  }
}
// THE FUNCTION OF GETTING THE LEVEL OF BRIGHTNESS
int ldrFunc() {
  ldrVal = analogRead(ldrPin);
  delay(100);
  return ldrVal;
}
