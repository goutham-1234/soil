#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
char* ssid = "Hari2002";
char* password =  "12345678";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
 
WiFiClient espClient;
PubSubClient client(espClient);


unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
int f;
float val; // also works with double. 
char buff2[10];

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D1,INPUT);
  delay(1000);
   WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  
  client.setServer(mqttServer, mqttPort);
  Serial.println(WiFi.localIP());
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP")) {
 
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
}
 void loop(){
 int moisture = digitalRead(D1);
for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(A0);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*3.3/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
 
  JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = "Ph";
  JsonArray& values = JSONencoder.createNestedArray("moisture");
 
  values.add(phValue);
  values.add(moisture);
 
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);
 
  if (client.publish("agriproduct", JSONmessageBuffer) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
 
  client.loop();
  Serial.println("-------------");

 }
