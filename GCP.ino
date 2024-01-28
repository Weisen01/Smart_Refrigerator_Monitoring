// #include <VOneMqttClient.h>
// #include <vonesetting.h>

/*
ESP32 publish telemetry data to Google Cloud (DHT11 sensor)
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"


const char *WIFI_SSID = "IZONE forever"; //your WiFi SSID
const char *WIFI_PASSWORD = "apple124"; // your password
const char *MQTT_SERVER = "34.67.76.60"; // your VM instance public IP address
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "iot"; // MQTT topic


const int DHT_PIN = A4;
const int DHT_TYPE = DHT11;

int IRSensor =21;
int LED_PIN = 47;   

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}


void setup()
{
 Serial.begin(115200);
 Serial.println("Starting");
 dht.begin();
 setup_wifi();
 client.setServer(MQTT_SERVER, MQTT_PORT);

 pinMode(IRSensor, INPUT);    

 pinMode(LED_PIN, OUTPUT); //Output pin for led lights
}


void reconnect()
{
  while (!client.connected())
 {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
    Serial.println("Connected to MQTT server");
    }
   else
   {
    Serial.print("Failed, rc=");
    Serial.print(client.state());
    Serial.println(" Retrying in 5 seconds...");
    delay(5000);
   }
 }
}


void loop()
{
  if (!client.connected())
   {
    reconnect();
   }
  client.loop();

  int sensorStatus = digitalRead(IRSensor); //Set the GPIO as input

  if (sensorStatus == 0 ) //Check whether the pin high or low
  {
    digitalWrite(LED_PIN,LOW); // IF the pin is high, turn off the board led

    Serial.println("Refrigerator item is sufficient"); //display the output on the serial monitor window

  }

  else{

    digitalWrite(LED_PIN, HIGH); //LED HIGH

    Serial.println("Refrigerator item is empty, please replenish as soon as possible"); //display the output on the seial monitor
  }

 delay(1000); // adjust the delay according to your requirements

  
 delay(2000);
 float temperature = dht.readTemperature();

  if (!isnan(temperature)){
    char payload[10];
    sprintf(payload, "%.2f", temperature);
    client.publish(MQTT_TOPIC, payload);
  }
  
  else{
    Serial.print("Temperature: ");
    Serial.println(temperature);

    Serial.println("Failed to read valid temo");
  }
}