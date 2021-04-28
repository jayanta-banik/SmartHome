#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#define DHTTYPE DHT11

const char* ssid = "Ultron's Intranet";
const char* password = "lol.java";
DHT dht(D5, DHTTYPE);
byte periodicity = 0;
String req = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://52.15.240.218:8080/status?flags=[%27GaneshLamp%27,%27RedTableLamp%27]&format=arduino");
    int httpCode = http.GET();
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      //code here
      digitalWrite(D1, int(payload.charAt(0)) - 48 ? LOW : 255);
      digitalWrite(D2, int(payload.charAt(1)) - 48 ? LOW : 255);
    }
    http.end();
  }
  delay(100);
  periodicity++;
  if (periodicity % 20 == 0) {
    //    get data
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      }
    else {
      float hic = dht.computeHeatIndex(t, h, false);
      //    upload data
      
      if (WiFi.status() == WL_CONNECTED) {
        String upload_url = "http://52.15.240.218:8080/setSensoryData";

        WiFiClient client;
        HTTPClient newhttp;
        newhttp.begin(client, upload_url); //HTTP
        newhttp.addHeader("Content-Type", "application/json");

        int httpCode = newhttp.POST("{\"Temp\":" + String(t)+  ",\"Hum\":"+  String(h)+ ",\"TempFL\":" + String(hic) + "}");

        if (httpCode > 0) { //Check the returning code
          String http_uppayload = newhttp.getString();   //Get the request response payload
//          Serial.println(http_uppayload);
        } else {
//          Serial.printf("[HTTP] POST... failed, error: %s\n", newhttp.errorToString(httpCode).c_str());
        }
        newhttp.end();
      }
    }
  }
}
