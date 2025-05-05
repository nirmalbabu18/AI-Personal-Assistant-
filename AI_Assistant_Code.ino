#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi credentials
const char* ssid = "ASTOR";
const char* password = "astor018";

// Cohere API Key
const char* apiKey = "qYkx5m37S1bTLzQxtW0Xal4UNQU7VokBz5pgsw9L";

// LED pin
const int ledPin = 4;  // GPIO4 
void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  Serial.println("🛠 Booting...");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("🔄 Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  Serial.print("📶 Connecting to WiFi...");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("🌐 IP Address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("✅ WiFi Connected!");
    display.println(WiFi.localIP());
    display.display();
  } else {
    Serial.println("\n❌ Failed to connect to WiFi.");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("❌ WiFi failed.");
    display.display();
  }

  Serial.println("💬 Type a message:");
}

void loop() {
  if (Serial.available()) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim();

    Serial.print("📥 Received: ");
    Serial.println(userInput);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("📤 Sending...");
    display.println(userInput);
    display.display();

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin("https://api.cohere.ai/v1/chat");
      http.addHeader("Authorization", String("Bearer ") + apiKey);
      http.addHeader("Content-Type", "application/json");

      StaticJsonDocument<1024> jsonDoc;
      jsonDoc["message"] = userInput;
      jsonDoc["model"] = "command-r-plus";

      String requestBody;
      serializeJson(jsonDoc, requestBody);

      Serial.println("🚀 Sending to Cohere...");
      Serial.println(requestBody);

      int httpResponseCode = http.POST(requestBody);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("📶 Response code: ");
        Serial.println(httpResponseCode);

        Serial.println("📄 Raw response: " + response);

        StaticJsonDocument<2048> doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error && doc.containsKey("text")) {
          const char* aiResponse = doc["text"];
          Serial.print("🤖 AI: ");
          Serial.println(aiResponse);

          display.clearDisplay();
          display.setCursor(0, 0);
          display.setTextSize(1);
          display.println("🤖 AI says:");
          display.println(aiResponse);
          display.display();

          //  LED CONTROL LOGIC
          String reply = String(aiResponse);
          reply.toLowerCase();

          if (reply.indexOf("turn on") != -1 && reply.indexOf("led") != -1) {
            digitalWrite(ledPin, HIGH);
            Serial.println("💡 LED turned ON");
          } else if (reply.indexOf("turn off") != -1 && reply.indexOf("led") != -1) {
            digitalWrite(ledPin, LOW);
            Serial.println("💤 LED turned OFF");
          }

        } else {
          Serial.println("⚠️ Failed to parse AI response.");
        }

      } else {
        Serial.print("❌ HTTP error: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("⚠️ WiFi disconnected. Cannot send message.");
    }
  }
}
