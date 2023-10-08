
//les biblioteques


#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 14

// Replace the next variables with your SSID/Password combination
const char* ssid = "fh_fcb688";
const char* password = "000000000";

// Add your MQTT Broker IP address:
const char* mqtt_server = "192.168.1.9";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


float temperature = 0;
float humidity = 0;
float capteursol =0;
int capteurlum=0;

// declaration des Pins
const int ledPin = 2;
bool Auto, Ventilo, Chauff, PompeArrosage,Lampe;
int VentiloPin = 5, ChauffPin = 19, PompeArrosagePin = 15,LampePin =18 ;
void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
  pinMode(VentiloPin, OUTPUT);
  pinMode(ChauffPin, OUTPUT);
  pinMode(PompeArrosagePin, OUTPUT);
  pinMode(LampePin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


//Connection Topic (esp32/output) lire message sur le Topic
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  //Convert Tab char to String
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

 
  // Quelle est le message arriver et faire les action selon chaque message
  if (String(topic) == "esp32/output") {

    if(messageTemp == "AutoOFF"){
      Serial.println("Manuel");
      Auto= true;
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "AutoOn"){
      Serial.println("Automatique");
      Auto= false;
      digitalWrite(ledPin, LOW);
    }
    else if(messageTemp == "VentiloOn"){
      if(not(Auto)){
        Serial.println("Ventilo On");
        Ventilo= 0;
        digitalWrite(VentiloPin, Ventilo);
      }
    }
    else if(messageTemp == "VentiloOff"){
      if(not(Auto)){
        Serial.println("Ventilo Off");
        Ventilo= 1;
        digitalWrite(VentiloPin, Ventilo);
      }
    }
    else if(messageTemp == "ChauffOn"){
      if(not(Auto)){
        Serial.println("Chauffage On");
        Chauff= 0;
        digitalWrite(ChauffPin, Chauff);
      }
    }
    else if(messageTemp == "ChauffOff"){
      if(not(Auto)){
        Serial.println("Chauffage Off");
        Chauff= 1;
        digitalWrite(ChauffPin, Chauff);
      }
    }
    
    else if(messageTemp == "PompeArrosageOn"){
      if(not(Auto)){
        Serial.println("Pompe Arrosage On");
        PompeArrosage= 0;
        digitalWrite(PompeArrosagePin, PompeArrosage);
      }
    }
    else if(messageTemp == "PompeArrosageOff"){
      if(not(Auto)){
        Serial.println("Pompe Arrosage Off");
        PompeArrosage= 1;
        digitalWrite(PompeArrosagePin, PompeArrosage);
      }
    }
    else if(messageTemp == "LampeOn"){
      if(not(Auto)){
        Serial.println("Lampe On");
        Lampe= 0;
        digitalWrite(LampePin, Lampe);
      }
    }
    else if(messageTemp == "LampeOff"){
      if(not(Auto)){
        Serial.println("Lampe Off");
        Lampe= 1;
        digitalWrite(LampePin, Lampe);
      }
    }


  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(200);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    DHT.read(DHT11_PIN);
    // Temperature in Celsius
    temperature = DHT.temperature; 
    capteursol = map(analogRead(35),4095,1000,0,100);
    capteurlum = map(analogRead(34),0,1000,0,100);
    //capteurlum = analogRead(34);
  

    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit
     char capString[8];
    dtostrf(capteursol, 1, 2, capString);
      Serial.print("capteursol ");
    Serial.println(capString);
    client.publish("esp32/capteursol", capString);

    char lumString[8];
    dtostrf(capteurlum, 1, 2, capString);
      Serial.print("capteurlum ");
    Serial.println(capString);
    client.publish("esp32/capteurlum", capString);
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);
    // mode Automatique
    if(Auto){
      if(temperature>30){
        Serial.println("Ventilo On");
        Ventilo= 0;
        digitalWrite(VentiloPin, Ventilo);
        client.publish("esp32/ventilo", "VentiloOn");

      }
      else if(temperature< 25 ){
         Serial.println("Chauffage On");
        Chauff= 0;
        digitalWrite(ChauffPin, Chauff);
        client.publish("esp32/chauffage", "ChauffageOn");

      }
      else {
        Serial.println("Ventilo Off");
        Ventilo= 1;
        digitalWrite(VentiloPin, Ventilo);

        Serial.println("Chauffage Off");
        Chauff= 1;
        digitalWrite(ChauffPin, Chauff);
        client.publish("esp32/chauffage", "ChauffageOff");
        client.publish("esp32/ventilo", "VentiloOff");

      }
      if(capteursol<50){
         Serial.println("PompeArrosageOn");
        PompeArrosage= 0;
        digitalWrite(PompeArrosagePin, PompeArrosage);
        client.publish("esp32/PompeArrosage", "PompeArrosageOn");

      }
      else {
        

        Serial.println("PompeArrosageOff");
        PompeArrosage= 1;
        digitalWrite(PompeArrosagePin, PompeArrosage);
        client.publish("esp32/PompeArrosage", "PompeArrosageOff");
        

      }

      if(capteurlum<30){
         Serial.println("LampeOn");
        Lampe= 0;
        digitalWrite(LampePin, Lampe);
        client.publish("esp32/Lampe", "LampeOn");

      }
      else {
        

        Serial.println("LampeOff");
        Lampe= 1;
        digitalWrite(LampePin, Lampe);
        client.publish("esp32/Lampe", "LampeOff");
        

      }
    }

    humidity = DHT.humidity; 
    
    // Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("esp32/humidity", humString);
  }
}