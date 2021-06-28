#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

#define DHTTYPE DHT11

const char* ssid = "";
const char* password = "";

byte periodicity = 39;
String req = "";

DHT dht(D5, DHTTYPE);
WiFiClient client;
IPAddress server(52, 15, 240, 218);

void setup() {
  // put your setup code here, to run once:
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);

//  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(server, 8080)) {
      HTTPClient http;
      http.begin(client, "http://52.15.240.218:8080/status?flags=[%27GaneshLamp%27,%27RedTableLamp%27,%27Fan%27,%27TubeLight%27]&format=arduino");
      int httpCode = http.GET();
      if (httpCode > 0) { //Check the returning code
        String payload = http.getString();   //Get the request response payload
        //code here
//        Serial.println(payload);
        digitalWrite(D1, int(payload.charAt(0)) - 48 ? LOW : 255);
        digitalWrite(D2, int(payload.charAt(1)) - 48 ? LOW : 255);
        digitalWrite(D3, int(payload.charAt(2)) - 48 ? LOW : 255);
        digitalWrite(D4, int(payload.charAt(3)) - 48 ? LOW : 255);
      }
      http.end();
      periodicity++;
//      Serial.print("Period=");
//      Serial.println(periodicity);
      if (periodicity % 40 == 0) {
        //    get data
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        if (isnan(h) || isnan(t)) {
        }
        else {
          float hic = dht.computeHeatIndex(t, h, false);
          //    upload data
          String upload_url = "http://52.15.240.218:8080/setSensoryData";
          HTTPClient newhttp;
          newhttp.begin(client, upload_url); //HTTP
          newhttp.addHeader("Content-Type", "application/json");

          int httpCode = newhttp.POST("{\"Temp\":" + String(t) +  ",\"Hum\":" +  String(h) + ",\"TempFL\":" + String(hic) + "}");

          if (httpCode > 0) { //Check the returning code
            String http_uppayload = newhttp.getString();   //Get the request response payload
//            Serial.println(http_uppayload);
          } else {
//            Serial.printf("[HTTP] POST... failed, error: %s\n", newhttp.errorToString(httpCode).c_str());
          }
          newhttp.end();
        }
      }
    }
  }
  else {
    //    turn to default mode
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, LOW);
    digitalWrite(D4, LOW);
  }
}
