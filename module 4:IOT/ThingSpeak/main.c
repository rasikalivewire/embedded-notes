/* install dht sensor library,thingspeak library*/


#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>

// Wi-Fi credentials
const char* ssid = "JioFiber-Livewire_5G";
const char* password = "987654321";

// ThingSpeak
unsigned long channelID = 3438275;
const char* writeAPIKey = "09G47P608V133PJG";

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup()
{
  Serial.begin(115200);

  dht.begin();

  Serial.println();
  Serial.println("DHT11 + ESP8266 + ThingSpeak");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start ThingSpeak
  ThingSpeak.begin(client);
}

void loop()
{
  // Read DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check sensor reading
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read DHT11!");
    delay(2000);
    return;
  }

  // Display values
  Serial.println("-------------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (response == 200)
  {
    Serial.println("ThingSpeak update successful!");
  }
  else
  {
    Serial.print("ThingSpeak update failed. HTTP error: ");
    Serial.println(response);
  }

  // Wait before next update
  delay(20000);
}
