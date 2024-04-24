#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h"          //disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems
#include "driver/gpio.h"
#include "ArduinoHttpClient.h"
#include "ArduinoJson.h"

const char ssid[] = "<SSID>"; // your network SSID (name)
const char pass[] = "<password>"; // your network password

char serverAddress[] = "cat-fact.herokuapp.com"; // server address
int port = 443;

String url = "/facts/random?animal_type=cat&amount=3";
WiFiClientSecure networkClient;
HttpClient client = HttpClient(networkClient, serverAddress, port);

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println();

  networkClient.setInsecure();

  Serial.println("Connected to WiFi");
}

void loop()
{
  Serial.print("Making GET request: ");
  Serial.print(serverAddress);
  Serial.println(url);
  // send GET request
  client.beginRequest();
  client.sendHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:124.0) Gecko/20100101 Firefox/124.0");
  client.sendHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Host", "cat-fact.herokuapp.com");
  client.sendHeader("Accept-Encoding", "gzip, deflate, br");
  int result = client.get(url);
  client.endRequest();

  if (result == 0)
  {
    Serial.println("Response received");
    int statusCode = client.responseStatusCode();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    String response = client.responseBody();

    JsonDocument responseJson;
    DeserializationError error = deserializeJson(responseJson, response);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println(response);
    }

    JsonArray catsFacts = responseJson.as<JsonArray>();
    for (JsonVariant catsFact : catsFacts)
    {
      Serial.println(catsFact["text"].as<String>());
    }
  }
  else
  {
    Serial.print("Error on HTTP request: ");
    Serial.println(result);
  }
  client.stop();

  // read the status code and body of the response
  Serial.println("Waiting 10 seconds before making a new request...");
  delay(10000);
}
