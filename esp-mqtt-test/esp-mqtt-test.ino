#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "wazir";
const char* password =  "mizhkor007";

const char* mqttServer = "postman.cloudmqtt.com";
const int mqttPort = 17277;
const char* mqttUser = "jcposcds";
const char* mqttPassword = "ovRwrbhyUs57";
int ledPin = 2;
String led_state = "OFF";
int timer_val = 0;
unsigned long start_time = millis();

WiFiClient espClient;
PubSubClient client(espClient);


void setup() { 
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("esp/test", "Hello from ESP8266");
  client.subscribe("esp/test");
//  client.publish("esp/timer",String(timer_val));

  pinMode(ledPin, OUTPUT);
 
}

void callback(char* topic, byte* payload, unsigned int length) {
//  led_state = "";
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);  
  
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
//  Serial.println(doc);  
  
  const char* sensor = doc["source"];
  const char* target = doc["target"];
  Serial.println(sensor);  
  Serial.println(target);  
  

  if (strcmp(sensor,"phone") == 0) {
    if (strcmp(target,"timer") == 0) {
      Serial.print("Got in");
      int value = doc["value"];
      Serial.println(value);
      if (value > 0) {
        led_state = "ON";      
        start_time = millis();
        timer_val = value;
      } else {
        led_state = "OFF";
        timer_val = 0;
      }
      
      Serial.print("Setting timer to ");
      Serial.println(timer_val);
    }
  }
  
// 
//  led_state = "";
//  Serial.print("Message arrived in topic: ");
//  Serial.println(topic);
//
// 
//  Serial.print("Message:");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//    led_state += (char)payload[i];
//  }
//  Serial.println();
  Serial.println("-----------------------");
//  Serial.println(value);
  Serial.println(led_state);
  Serial.println("-----------------------");
 
 
}
 
void loop() {
  Serial.print("LED state:");
  Serial.println(led_state);
  
  if (led_state == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("turning off");
    delay(20);
  } else {
    digitalWrite(ledPin, HIGH);
    delay(20);
  }
  unsigned long current_time = millis();
  unsigned long elapsed_time = (current_time - start_time)/1000;
  Serial.print("Elapsed time: ");
  Serial.println(elapsed_time);

  int remaining_timer = timer_val - elapsed_time;
  Serial.print("remaining time: ");
  Serial.println(remaining_timer);
  
//  if (remaining_timer < 0) {
//    remaining_timer = 0;
//  }

  if (remaining_timer < 0) {
    Serial.println("remaining seconds need to replenish .............. XXX");
    remaining_timer = 0;
    led_state = "OFF";
  }

  StaticJsonDocument<300> JSONbuffer;
  JsonObject JSONencoder = JSONbuffer.to<JsonObject>(); 
  JSONencoder["source"] = "motor";
  JSONencoder["target"] = "phone";
  JSONencoder["value"] = remaining_timer;

  char output[256];
//  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  serializeJson(JSONbuffer, output, sizeof(output));
  
  Serial.println(output);
 
  if (client.publish("esp/test", output) == true) {
      Serial.println("Success sending message");
  } else {
      Serial.println("Error sending message");
  }

  
  delay(1000);
  client.loop();
//  const int capacity = JSON_OBJECT_SIZE(3);
//  StaticJsonDocument<capacity> pub;
//  pub["source"] = 42;
//  pub["target"] = 48.748010;
//  pub["value"] = remaining_timer;
//
//  // Convert the document to an object
////  JsonObject obj = pub.to<JsonObject>();
//  char output[512];
//
//  serializeJson(pub, output, sizeof(output));
//  
//  client.publish("esp/test",output);
//  
  // client.publish("esp/timer",String(timer_val));

  
}
