#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Camera WiFi credentials (MUST match your camera settings!)
const char *ssid = "AP_M5STACK_CAM_Gruppe_X";
const char *password = "12345678";                       // Change to YOUR camera password
const char *cameraURL = "http://192.168.4.1:80/capture"; // Camera URL

void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(100);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("Connecting to camera...");

  // Connect to camera's WiFi
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    M5.Lcd.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    M5.Lcd.println("\nConnected!");
    M5.Lcd.print("IP: ");
    M5.Lcd.println(WiFi.localIP());
    delay(2000);
  }
  else
  {
    M5.Lcd.println("\nConnection FAILED!");
    while (1)
    {
      delay(1000);
    } // Stop here
  }
}

void loop()
{
  // Get and display camera image
  HTTPClient http;

  M5.Lcd.setCursor(10, 220);
  M5.Lcd.println("Fetching image...");

  http.begin(cameraURL);
  int httpCode = http.GET();

  if (httpCode == 200)
  { // HTTP OK
    // Get the image size
    int len = http.getSize();

    // Allocate buffer on heap
    uint8_t *buffer = (uint8_t *)malloc(len);

    if (buffer)
    {
      // Read image data
      WiFiClient *stream = http.getStreamPtr();
      int bytesRead = stream->readBytes(buffer, len);

      // Display image on M5Stack screen
      M5.Lcd.drawJpg(buffer, bytesRead, 0, 0, 320, 240);

      // Free the buffer
      free(buffer);
    }
    else
    {
      M5.Lcd.println("Memory allocation failed!");
    }
  }
  else
  {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setCursor(10, 100);
    M5.Lcd.printf("HTTP Error: %d\n", httpCode);
  }

  http.end();
  delay(100); // Small delay before next frame
}