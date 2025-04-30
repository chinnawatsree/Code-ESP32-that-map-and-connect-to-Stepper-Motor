#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

const char* ssid = "BC";
const char* password = "092614pleum";
const char* serverIP = "192.168.86.161"; // IP ของ motor

const char* influx_host = "http://192.168.86.50:8086";  // URL ของ InfluxDB
const char* org = "iot";
const char* bucket = "Turbidity";
const char* token = "_bffHuHUSTIljC7IVEXoQqBPK7JqLmnXyt6ELUj2ELSWkc0JTIlUgXFuwtYEtAPWAgXLvygA_YufdupLmC9YGg==";
WiFiClient client;
#define pin 32

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  if (client.connect(serverIP, 80)) {
    int analogValue = analogRead(pin);
    float voltage = analogValue * (3.3 / 4095.0);

    if (voltage < 1.5) voltage = 1.5;
    if (voltage > 3.3) voltage = 3.3;

    float ntu = -222.22 * voltage + 733.33;
    if (ntu < 0) ntu = 0;
    if (ntu > 200) ntu = 200;

    Serial.print("Sending NTU: ");
    Serial.println(ntu);
    client.print(ntu);  // ส่งไปให้ motor
    client.stop();      // ปิดการเชื่อมต่อหลังส่งเสร็จ

    String line = "ntu_data value=" + String(ntu, 2);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(influx_host) + "/api/v2/write?org=" + org + "&bucket=" + bucket + "&precision=s";
      http.begin(url);
      http.addHeader("Authorization", "Token " + String(token));
      http.addHeader("Content-Type", "text/plain");

      int httpResponseCode = http.POST(line);
      Serial.print("Sent to InfluxDB: ");
      Serial.print(line);
      Serial.print(" → HTTP: ");
      Serial.println(httpResponseCode);

      http.end();
    } else {
      Serial.println("Connection to InfluxDB failed");
    }

    delay(5000);  // ส่งทุก 5 วินาที
  } else {
    Serial.println("Failed to connect to server");
  }
}
