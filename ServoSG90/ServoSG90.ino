#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <Servo.h>


const char ssid[] = "VincentW";
const char pass[] = "270699161097110762";
int tmpPos = 0;
int speeds = 0;

WiFiClient net;
MQTTClient client;
Servo servo;
//unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("Broker_IoT", "vincent_wijaya27", "vincent123")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/servo");
  // client.unsubscribe("/hello");
}



void setup() {
  Serial.begin(115200);
  servo.attach(3);
  WiFi.begin(ssid, pass);

  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  //  if (millis() - lastMillis > 1000) {
  //    lastMillis = millis();
  //    client.publish("/hello", "world");
  //  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  String posisi = payload;
//  Serial.println(posisi);
  int pos = posisi.toInt();

  //setting the servo's speed
  if (pos < 0) {
    speeds = pos;
    speeds = ((speeds - speeds) - speeds);
  }


  //setting Servo's position
  if (pos > tmpPos && pos >= 0) {
    for (int i = tmpPos ; i <= pos; i++) {
      servo.write(i);
      delay(speeds);
    }
    tmpPos = pos;
    Serial.println("Current Position: ");
    Serial.println(tmpPos);
    Serial.println("Current Speed :");
    Serial.println(speeds);
  } else if (pos < tmpPos && pos >= 0) {
    for (int j = tmpPos ; j >= pos ; j--) {
      servo.write(j);
      delay(speeds);
    }
    tmpPos = pos;
    Serial.println("Current Position: ");
    Serial.println(tmpPos);
    Serial.println("Current Speed :");
    Serial.println(speeds);
  } else if (tmpPos == pos) {
    servo.write(pos); //stand still
  }
  else {}
}
