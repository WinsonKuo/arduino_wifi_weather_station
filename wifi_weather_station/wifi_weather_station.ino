/* 
*  Simple WiFi weather station with Arduino, the DHT11 sensor & the CC3000 chip
*  Part of the code is based on the work done by Adafruit on the CC3000 chip & the DHT11 sensor
*  Writtent by Marco Schwartz for Open Home Automation
*/

// Include required libraries
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <dht.h>

// Define CC3000 chip pins
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

// WiFi network (change with your settings !)
#define WLAN_SSID       "HTC Portable Hotspot 2B41"        // cannot be longer than 32 characters!
#define WLAN_PASS       "0936967237"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

// DHT22 sensor pins
#define DHT22_PIN 7

//LED 
#define  LED_R       22
#define  LED_G       24
#define  LED_B       26

// Create CC3000 & DHT instances
dht DHT;
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2);
                                         
// Local server IP, port, and repository (change with your settings !)
//uint32_t ip = cc3000.IP2U32(61,228,246,253);
uint32_t ip = 0;
int port = 80;
String repository = "http://winsonkuo.myds.me/";
char website[] = "winsonkuo.myds.me";
                          
void setup(void)
{
  Serial.begin(115200);
  
  //Setup LED
  pinMode(LED_R, OUTPUT);   // sets the pin as output
  pinMode(LED_G, OUTPUT);   // sets the pin as output
  pinMode(LED_B, OUTPUT);   // sets the pin as output
   
  // Get DHT sensor Lib version
  Serial.print("DHT LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
    
  // Initialise the CC3000 module
  if (!cc3000.begin())
  {
    while(1);
  }

  // Connect to  WiFi network
  cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY);
  Serial.println("Connected to WiFi network!");
    
  // Check DHCP
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100);
  } 
  
  Serial.println("Resolve WebSite...");
  while  (ip  ==  0)  {
    if  (! cc3000.getHostByName(website, &ip))  {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }
  Serial.print(F("WebSite IP is "));  
  cc3000.printIPdotsRev(ip);
  Serial.println(""); 
}

void loop(void)
{
    // Check DHT ability
    int chk = DHT.read22(DHT22_PIN);
    Serial.print("DHT status is \t");
    switch (chk)
    {
    case DHTLIB_OK:
        Serial.print("OK.\n");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error.\n");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error.\n");
        break;
    default:
        Serial.print("Unknown error.\n");
        break;
    }
    // Measure the humidity & temperature

    float h = DHT.humidity;
    float t = DHT.temperature;
    char char_h[10],char_t[10];
    dtostrf(h,3,1,char_h);
    dtostrf(t,3,1,char_t);
    // Transform to String
    String temperature = String(char_t);
    String humidity = String(char_h);

    // Print data
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.println("");
    
    // Send request
    String request = "GET "+ repository + "sensor.php?temp=" + temperature + "&hum=" + humidity + " HTTP/1.0";
    send_request(request);
    
    // Update every second
    delay(1000);
}

void control_light(int R, int G, int B) {
  analogWrite(LED_R, 255 - R);
  analogWrite(LED_G, 255 - G);
  analogWrite(LED_B, 255 - B);
}

// Function to send a TCP request and get the result as a string
void send_request (String request) {
     
    // Connect    
    Serial.println("Starting connection to server...");
    Adafruit_CC3000_Client client = cc3000.connectTCP(ip, port);
    
    // Send request
    if (client.connected()) {
      client.println(request);      
      client.println(F(""));
      Serial.println("Connected & Data sent");
    } 
    else {
      Serial.println(F("Connection failed"));    
    }

    while (client.connected()) {
      while (client.available()) {

      // Read answer
      char c = client.read();
      }
    }
    Serial.println("Closing connection");
    Serial.println("");
    client.close();
    
}
