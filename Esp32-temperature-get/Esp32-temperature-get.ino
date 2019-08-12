#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// ======================== SENSOR NUMBER ==========================
                    #define SENSOR_NUMBER 1
// ======================== ============= ==========================


#define LED_BUILTIN 2 
#define DHTPIN 18 
#define LM35 32


#define DHTTYPE DHT22 

#define MINUTES 3


#define MAXIMUM_DIFFERENCE 3

 
const char* ssid = "GVT-DE94";
const char* password =  "0011710928";


float humidityDHT = 0;
float temperatureDHT = 0;

int rawValueLM35 = 0;
double voltageLM35 = 0;
double temperatureLM35 = 0;


double finalTemperature = 0;



double difference = 0;


String url = "";


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);  
  delay(4000);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("Connected to the WiFi network");
  Serial.print(F("DHT22 Begin!"));

  dht.begin();
 
}
 
void loop() {
  humidityDHT = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperatureDHT = dht.readTemperature();

  if(isnan(humidityDHT)) {
    humidityDHT = 80;
  }
   
 
  rawValueLM35 = analogRead(LM35);
  voltageLM35 = (rawValueLM35 / 2048.0) * 3300; // 5000 to get millivots.
  temperatureLM35 = voltageLM35 * 0.1;

  if(isDifferent(temperatureLM35, temperatureDHT)) {
    rawValueLM35 = analogRead(LM35);
    voltageLM35 = (rawValueLM35 / 2048.0) * 3300; // 5000 to get millivots.
    temperatureLM35 = voltageLM35 * 0.1;

    finalTemperature = temperatureLM35;
    
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    finalTemperature = temperatureDHT;
  }

  Serial.println(temperatureLM35);
  Serial.println(temperatureDHT);

  url = "http://backend-farm-automation.herokuapp.com/add-data?";
  url += "temperature=" + String(finalTemperature) + "&";
  url += "humidity=" + String(humidityDHT) + "&";
  url += "title=" + String(SENSOR_NUMBER);

  Serial.println(url);
  
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    Serial.println("Gerando Requisição");
 
    HTTPClient http;

    http.begin(url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
        String payload = http.getString();
        Serial.println(url);
        Serial.println(httpCode);
        Serial.println(payload);
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  delay(1000 * 60 * MINUTES); 
}


bool isDifferent(double temperatureLM35, double temperatureDHT) {
  difference = temperatureLM35 - temperatureDHT;
 
  if(difference > MAXIMUM_DIFFERENCE || difference < -MAXIMUM_DIFFERENCE){
    return true;
  } else {
    return false;
  }
}
