#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

Servo servo;

#define WIFISSID "3oLabs"                                // Put your WifiSSID here
#define PASSWORD "pamu.1998"                                 // Put your wifi password here
#define TOKEN ""         // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "myecgsensor"                       // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                                            //it should be a random and unique ascii string and different from all other devices
 
/****************************************
 * Define Constants
 ****************************************/
#define VARIABLE_LABEL "myecg" // Assing the variable label
#define VARIABLE_LABEL2 "vibration" 
#define DEVICE_LABEL "esp8266" // Assig the device label
 
#define SENSOR A0 // Set the A0 as SENSOR
 
char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[100];
char payload2[100];
char topic[150];
// Space to store values to send
char str_sensor[10];
char str_sensor2[10];
int shock = 5; //vibration sensor pin declare

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);
 
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.write(payload, length);
  Serial.println(topic);
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
 
/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as INPUT 
  pinMode(SENSOR, INPUT);
 pinMode(shock,INPUT);
 
  Serial.println();
  Serial.print("Waiting for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(800);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);  
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label

  sprintf(payload2, "%s", ""); // Cleans the payload
  sprintf(payload2, "{\"%s\":", VARIABLE_LABEL2); // Adds the variable label
  
  float myecg = analogRead(SENSOR); 
  long out = pulseIn(shock, HIGH);

  /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
  dtostrf(myecg, 4, 2, str_sensor);
  dtostrf(out, 4, 2, str_sensor2);
  
  
  sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
  sprintf(payload2, "%s {\"value\": %s}}", payload2, str_sensor2); // Adds the value
  Serial.println(myecg);
  //Serial.println(out);
  client.publish(topic, payload);
  client.publish(topic, payload2);
  

  client.loop();
  delay(100);
}
