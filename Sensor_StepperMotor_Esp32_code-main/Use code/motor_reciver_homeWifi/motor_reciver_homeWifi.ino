#include <WiFi.h>
#include <WiFiServer.h>
#include <Stepper.h>

// WiFi บ้าน
const char* ssid = "BC";
const char* password = "092614pleum";

WiFiServer server(80);

// มอเตอร์
const int stepsPerRevolution = 2048;
const int step180 = stepsPerRevolution / 2;

#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

int A = 0;
float B = 1024;
int newValue;
int currentValue = 0;

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP());

  myStepper.setSpeed(5);
  Serial.println("move back 180 ");
  myStepper.step(-step180);
  delay(2000);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    String receivedData = client.readString();
    A = receivedData.toInt();
    if (A > 0) {
      if (A < 0) A = 0;
      if (A > 200) A = 200;

      B = 1024 * ((float)A / 200);
      newValue = int(B);

      int addValue = newValue - currentValue;
      currentValue = newValue;

      myStepper.step(addValue);
      Serial.print("Received A = ");
      Serial.print(A);
      Serial.print(" → Stepper moved: ");
      Serial.println(addValue);
    }else{
      int minusValue = currentValue ;
      currentValue -= minusValue;
      Serial.print("Received A = ");
      Serial.print(A);
      Serial.print(" → Stepper moved: ");
      Serial.println(-minusValue);
      myStepper.step(-minusValue);
    }
  }
}
