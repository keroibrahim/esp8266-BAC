#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "kiromar";      // Replace with your SSID
const char* password = "ke31ro10";  // Replace with your password

ESP8266WebServer server(80);

const int analogPin = A0;  // Analog pin for MQ-3 sensor

// Calibration values (these need to be determined experimentally)
const float CALIBRATION_AIR = 0.098;  // Analog reading in clean air
const float CALIBRATION_1000_PPM = 0.35;  // Analog reading in 1000 ppm alcohol

// Function to calculate BAC %
float calculateBAC(int sensorValue) {
  float voltage = sensorValue * (5.0 / 1023.0);
  float ratio = voltage / CALIBRATION_AIR; // This is just an example ratio
  float bac = ratio * 0.08; // Example conversion, not accurate
  return bac;
}

void handleRoot() {
  int sensorValue = analogRead(analogPin);
  float bac = calculateBAC(sensorValue);

  String html = "<html>" \
                "<head>" \
                  "<title>MQ-3 Sensor</title>" \
                  "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">" \
                  "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0,minimum-scale=1.0,maximum-scale=1.0,user-scalable=no\">" \
                  "<style>" \
                    "body {background-color: #333;color: #fff;font-family: Arial, sans-serif;text-align: center;}" \
                    "p {font-size: 20px;}" \
                  "</style>" \
                "</head>" \
                "<body>" \
                  "<h1>MQ-3 Alcohol Sensor Readings</h1>" \
                  "<p>Analog value: " + String(sensorValue) + "</p>" \
                  "<p>Estimated BAC: " + String(bac, 2) + " %</p>" \
                "</body>" \
              "</html>";

  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // Ensure the ESP8266 is only in station mode
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  MDNS.update();
  server.handleClient();
}
