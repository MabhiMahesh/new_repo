#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

#define LED_BUILTIN 2

const char* ssid = "Technotouch";
const char* password = "5135Innova";

const char* version_url = "https://raw.githubusercontent.com/Mahesh-rss/new_repo/master/version.txt";
const char* firmware_url = "https://raw.githubusercontent.com/Mahesh-rss/new_repo/master/build/esp32.esp32.esp32/test.ino.bin";
const int CURRENT_VERSION = 7;

void checkOTA() {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  Serial.println("Checking OTA...");

  http.begin(client, version_url);

  int httpCode = http.GET();

  Serial.print("Version HTTP code: ");
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    String serverVersion = http.getString();
    serverVersion.trim();

    int newVersion = serverVersion.toInt();

    Serial.print("Current version: ");
    Serial.println(CURRENT_VERSION);

    Serial.print("Server version: ");
    Serial.println(newVersion);

    if (newVersion > CURRENT_VERSION) {
      Serial.println("New firmware available");

      http.end();

      http.begin(client, firmware_url);

      httpCode = http.GET();

      Serial.print("Firmware HTTP code: ");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK) {
        int contentLength = http.getSize();

        Serial.print("Firmware size: ");
        Serial.println(contentLength);

        if (contentLength > 0) {
          Serial.println("Starting OTA...");

          if (Update.begin(contentLength)) {
            Serial.println("OTA memory ready");

            WiFiClient* stream = http.getStreamPtr();

            size_t written = Update.writeStream(*stream);

            Serial.print("Written: ");
            Serial.println(written);

            if (written == contentLength) {
              Serial.println("Firmware written successfully");

              if (Update.end() && Update.isFinished()) {
                Serial.println("OTA successful");
                Serial.println("Restarting...");
                ESP.restart();
              }
            } else {
              Serial.println("Firmware write incomplete");
            }
          } else {
            Serial.print("Update.begin failed: ");
            Serial.println(Update.errorString());
          }
        }

        http.end();
      } else {
        Serial.println("No update required");
      }
    } else {
      Serial.println("Failed to check version");
    }

    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Starting...");

  //  Serial.println("Checking OTA...");
  checkOTA();
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}