/*******************************************************************************
 *
 *          Estacion Metereologica v1.0
 *
 *******************************************************************************
 * FileName:        ESP8266_002.ino
 * Processor:       NodeMCU ESP8266 Development Kit V1.0
 * Biblioteca:      esp8266 by ESP8266 Community v2.5.2
 * Complier:        Arduino v1.8.9
 * Author:          Arduining
 * Description:     Pequeña y basica estacion metereologica, mandamos datos de 
 *                  temperatura, humedad relativa y luminosidad a ThinkSpeak.
 *******************************************************************************
 * Rev.         Date            Comment
 *   v1.00      22/09/2019      Creacion y pruebas de envio de datos.
 ******************************************************************************/
#include <ESP8266WiFi.h>
#include <DHT.h>

String apiKey = "APYKEY WRITE DE SU CUENTA";
const char* ssid = "NOMBRE DE LA RED";
const char* password = "CONTRASEÑA DE LA RED";
const char* server = "api.thingspeak.com";

#define DHTPIN D4 //Pin for DTHT22
#define DHTTYPE DHT22

const int pinLedStatus = D0; //LED on board
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

int flagState = LOW;
long previousMillis = 0;  
long interval = 500;           
int sysFlagStatus = LOW;
unsigned int countSec = 0;
unsigned int timeRefreshData = 299; //Tiempo de actulizacion de datos en segundos

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  pinMode(pinLedStatus, OUTPUT);
  // Setup Serial connection
  Serial.begin(9600);
  delay(10);
  
  dht.begin(); //Init DHT
  
  WiFi.begin(ssid, password);
  Serial.println();
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
}
void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 
    if (flagState == LOW){
      flagState = HIGH;
      sysFlagStatus = HIGH;
      }
    else
        flagState = LOW;
    // set the LED with the ledState of the variable:
    digitalWrite(pinLedStatus, flagState); 
    }
    if (sysFlagStatus == HIGH){
      countSec++;
      if (countSec>timeRefreshData){
        countSec = 0; 
        float temperatura = dht.readTemperature(); 
        float humedad = dht.readHumidity();
        float luminosidad = analogRead(A0);
        if (client.connect(server,80)) {
              String postStr = apiKey;
              postStr +="&field1=";
              postStr += String((int)temperatura);
              postStr +="&field2=";
              postStr += String((int)humedad);
              postStr +="&field3=";
              postStr += String(luminosidad);
              postStr += "\r\n\r\n";
              client.print("POST /update HTTP/1.1\n");
              client.print("Host: api.thingspeak.com\n");
              client.print("Connection: close\n");
              client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
              client.print("Content-Type: application/x-www-form-urlencoded\n");
              client.print("Content-Length: ");
              client.print(postStr.length());
              client.print("\n\n");
              client.print(postStr);
              Serial.print("Temperature: ");
              Serial.print(temperatura);
              Serial.print(" °C ");
              Serial.print("  Humedad: ");
              Serial.print(humedad);
              Serial.print(" %");
              Serial.print("  Luminosidad: ");
              Serial.print(luminosidad);
              Serial.println(" Lux");
              Serial.println("% send to Thingspeak");
          }
          client.stop();
          Serial.println("Waiting…");
          }
          Serial.print("timeOut: ");
          Serial.println(countSec);
          sysFlagStatus = LOW;
    }     
}
