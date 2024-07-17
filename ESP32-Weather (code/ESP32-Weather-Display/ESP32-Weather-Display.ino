#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <U8g2lib.h> // Include the U8g2 library

const char* ssid = "yo";
const char* password = "H@123456789";

// Your OpenWeatherMap API Key
String openWeatherMapApiKey = "114991a0a900f2dfb05237aaee7af144"; 

String city = "Hubli";
String countryCode = "1269920"; // Country code for India

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

String jsonBuffer;

// Initialize OLED display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


// Function declaration
String httpGETRequest(const char* serverName);
void displayWeatherData(JSONVar myObject);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize OLED display
  u8g2.begin();
  u8g2.clearBuffer();
u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(0, 20, "Weather Display");
  u8g2.sendBuffer();
  
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey +"&units=metric";
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      
      displayWeatherData(myObject); // Call function to display weather data on OLED
      
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void displayWeatherData(JSONVar myObject) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tr);



  u8g2.drawStr(0, 20, "Temperature: ");
  u8g2.setCursor(90, 20);
  u8g2.print(myObject["main"]["temp"]);
  
  u8g2.drawStr(0, 80, "Pressure: ");
  u8g2.setCursor(90, 80);
  u8g2.print(myObject["main"]["pressure"]);
  
  u8g2.drawStr(0, 60, "Humidity: ");
  u8g2.setCursor(90, 60);
  u8g2.print(myObject["main"]["humidity"]);
  
  u8g2.drawStr(0, 40, "Wind Speed: ");
  u8g2.setCursor(90, 40);
  u8g2.print(myObject["wind"]["speed"]);
  
  u8g2.sendBuffer();
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode > 0) {
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();

  return payload;
}
