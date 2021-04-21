#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Ultron's Intranet";
const char* password = "lol.java";
void setup() {
  // put your setup code here, to run once:
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
  Serial.println("connected");

}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://52.15.240.218:8080/status?flags=[%27GaneshLamp%27,%27RedTableLamp%27]&format=arduino");
    int httpCode = http.GET();
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);
      //code here
      digitalWrite(D1, int(payload.charAt(0))-48?LOW:255);
      digitalWrite(D2, int(payload.charAt(1))-48?LOW:255);
    }
    http.end();
  }
  delay(100);
}
