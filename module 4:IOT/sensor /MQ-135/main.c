
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// -------- Wi-Fi Configuration --------
const char* ssid = "IOT";
const char* password = "12345678";

// -------- ThingSpeak Configuration --------
const char* writeAPIKey = "2DNQUIWHSL5XXSLI";
const char* server = "https://api.thingspeak.com/update";

// -------- Hardware Pins (GPIO numbers) --------
const int MQ135_PIN = A0;
const int LED_PIN = 14;       // D5 = GPIO14
const int BUZZER_PIN = 12;    // D6 = GPIO12

// -------- Sensor Threshold --------
const int GAS_THRESHOLD = 500;

// -------- Passive Buzzer Frequency --------
const int BUZZER_FREQUENCY = 2000;  // 2 kHz

// -------- Timing --------
unsigned long lastReadTime = 0;
unsigned long lastUploadTime = 0;

const unsigned long READ_INTERVAL = 2000;
const unsigned long UPLOAD_INTERVAL = 20000;

// -------- Sensor Variables --------
int rawValue = 0;
int gasIndex = 0;
int pollutionStatus = 0;
int buzzerState = 0;

void connectWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void readSensor()
{
  rawValue = analogRead(MQ135_PIN);

  // Demonstration index, not calibrated air quality percentage
  gasIndex = map(rawValue, 0, 1023, 0, 100);
  gasIndex = constrain(gasIndex, 0, 100);

  // Check gas reading against threshold
  pollutionStatus =
      (rawValue >= GAS_THRESHOLD) ? 1 : 0;

  // Buzzer status: 1 = ON, 0 = OFF
  buzzerState = pollutionStatus;

  // LED control
  digitalWrite(LED_PIN,
               pollutionStatus ? HIGH : LOW);

  // Passive buzzer control
  if (buzzerState == 1)
  {
    tone(BUZZER_PIN, BUZZER_FREQUENCY);
  }
  else
  {
    noTone(BUZZER_PIN);
  }
}

void displayOutput()
{
  Serial.println("\n----- AIR QUALITY MONITOR -----");

  Serial.print("MQ135 Raw Value: ");
  Serial.println(rawValue);

  Serial.print("Relative Gas Index: ");
  Serial.print(gasIndex);
  Serial.println("%");

  Serial.print("Pollution Status: ");
  Serial.println(pollutionStatus ? "ALERT" : "NORMAL");

  Serial.print("LED State: ");
  Serial.println(pollutionStatus ? "ON" : "OFF");

  Serial.print("Buzzer State: ");
  Serial.println(buzzerState ? "ON" : "OFF");

  if (buzzerState == 1)
  {
    Serial.println("Buzzer Frequency: 2000 Hz");
  }

  Serial.println("-------------------------------");
}

void uploadToThingSpeak()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi disconnected!");
    connectWiFi();
  }

  WiFiClientSecure client;

  // Initial testing only; certificate verification is disabled
  client.setInsecure();

  HTTPClient http;

  String url = String(server) +
      "?api_key=" + writeAPIKey +
      "&field1=" + String(rawValue) +
      "&field2=" + String(gasIndex) +
      "&field3=" + String(pollutionStatus) +
      "&field4=" + String(buzzerState);

  Serial.println("Uploading to ThingSpeak...");

  if (http.begin(client, url))
  {
    int responseCode = http.GET();

    if (responseCode == HTTP_CODE_OK)
    {
      String response = http.getString();
      long entryID = response.toInt();

      Serial.print("ThingSpeak Entry ID: ");
      Serial.println(entryID);

      if (entryID > 0)
      {
        Serial.println("Upload successful!");
      }
      else
      {
        Serial.println(
          "Upload rejected. Check API key/rate limit."
        );
      }
    }
    else
    {
      Serial.print("HTTP error: ");
      Serial.println(responseCode);
    }

    http.end();
  }
  else
  {
    Serial.println("Unable to start HTTPS request.");
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.println("\nESP8266 MQ135 IoT Project");
  Serial.println("Starting...");

  connectWiFi();

  Serial.println(
    "Sensor warming up. Readings may initially fluctuate."
  );
}

void loop()
{
  unsigned long now = millis();

  readSensor();

  if (now - lastReadTime >= READ_INTERVAL)
  {
    lastReadTime = now;
    displayOutput();
  }

  if (now - lastUploadTime >= UPLOAD_INTERVAL)
  {
    lastUploadTime = now;
    uploadToThingSpeak();
  }

  delay(50);
}
