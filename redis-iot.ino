//
// IoT Redis Workshop
// F.Cerbelle
//
// Goal : Understand and use the RESP Protocol
//
// Base file :
// - Serial console initialized with DEBUG, STATS and PROFILING
// - Disabled WiFi debug on Serial
// - Wifi network configuration and connection
// - LED configuration and blink on stats
// - Wifi connection sample with send and receive
// - Sensor read every 5 seconds
//
// Notice that everything is done to be non-blocking, 
// the main loop has to run as fast as possible
//
// Lab1 : Connect to the redis instance and send/receive query
// Task 1
// - Open the API docs
//      https://www.arduino.cc/en/Reference/WiFiClient
//      http://esp8266.github.io/Arduino/versions/2.3.0/doc/reference.html
// - Declare a WiFiClient object 
// - Declare an IPAddress variable
// - Check connection with WiFiClient.connected()
//      uint8_t connected();
// - If not connected force a new resolution of the redis server IP address with WiFi.hostByName (1=OK, other=error)
//      int WiFi::hostByName(const char* aHostname, IPAddress& aResult);
// - If not connected open the TCP connection with WiFiClient.connect
//      int connect(IPAddress ip, uint16_t port);
// Task 2
// - Send a simple command (PING) to Redis using the RESP protocol (https://redis.io/topics/protocol) with WiFiClient.write "*1\r\n$4\r\nPING\r\n"
//      size_t write(uint8_t);
//      size_t write(const uint8_t *buf, size_t size);
// - Eventually force the sending with WiFiClient.flush()
//      void flush();
// Task 3
// Wait for reply with WiFiClient.available()
//      int available();
// Read and Print the Redis answer with WiFiClient.read and Serial.print/Serial.println (only one single arg)
//      int read();
//      int read(uint8_t *buf, size_t size);
// Task 4
// - Save all the files
// - Open a terminal in the source folder
// - Add your changes to the changeset (for the next commit) : git add .
// - Commit your changeset in the local repository : git commit
// - Push your commit : git push
// - Twit what you made with hastags and eventually with a picture or screenshot
// Task 5 (Extra if finished early)
// - "DEL test:<WiFi..macAddress()>" 
// - "SET test:<WiFi..macAddress()> 1" 
// - "TYPE test:<WiFi..macAddress()>" 
// - "INCR test:<WiFi..macAddress()>" 
// - "GET test:<WiFi..macAddress()>" 

// Lab2: Use the library to generate RESP, RealTime with PubSub
// Task 1
// - Include the "RedisCommand.h" header
// - Create a RedisCommand_t variable
// - In the setup() : Add a device entry with your MAC address in the "devices" set (SADD devices <MACAddress>) using the rediscommand_* helpers
// - In the loop() : Push your sensor value on the left side of the list "v:<MacAddress>"
// Task 2
// - In the setup() : Publish your MAC address on the refreshdevices PubSub channel to notify the dashboard application and to update the Web UI
// - In the loop() : Publish your MAC address on the refreshvalues PubSub channel to notify the dashboard application and to update the Web UI
// - Watch the number of loop / second
// Task 3
// - Save all the files
// - Open a terminal in the source folder
// - Add your changes to the changeset (for the next commit) : git add .
// - Commit your changeset in the local repository : git commit
// - Push your commit : git push
// - Twit what you made with hastags and eventually with a picture or screenshot
// Task 4 (Extra if finished early)
// - Publish on the refreshvalues channel only every 100 reads

// Lab3: Use pipelining to boost the ops/sec and save bandwidth
// - include the "RedisClient.h" header
// - Declare a RedisClient_t variable
// - in setup(): Initialize the variable with redisclient_init
// - Replace your WiFiClient.print(RedisCommand) by redisclient_send()
//   It will take care of reopening the connection if disconnected
// - Add the value of redisclient_getpipelinemaxsize to the STATS_HOOK
// - in setup() : add a redisclient_setpipelinemaxsize(int) with values between 1 and 100
// - in loop() : increase the pipelinemaxsize every sec



// Configuration
#define DEBUG
#define PROF
#define STATS
//#define STATS_MEM
#define STATS_HOOK digitalWrite(LED_BUILTIN,((digitalRead(LED_BUILTIN)==HIGH)?LOW:HIGH))
#define STATS_PERIOD 1000

//  your network SSID (name)
#define WIFI_SSID "Freebox-AEA6A1"
#define WIFI_PASS "adherebit-commend96-sonatio#!-calidior26"

#include <ESP8266WiFi.h>
#include "tools.h"

/********/
/* Main */
/********/
void setup() {
  Serial.begin(115200);
//  Serial.setDebugOutput(true);
  while (!Serial);
  DEBUG_PRINTLN("Serial initialized.");

  // WIFI stuff
  DEBUG_PRINT("Connecting to ");
  DEBUG_PRINT(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("");
  DEBUG_PRINT("WiFi (");
  DEBUG_PRINT(WiFi.macAddress());
  DEBUG_PRINT(") connected with IP ");
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT("DNS0 : ");
  DEBUG_PRINTLN(WiFi.dnsIP(0));
  DEBUG_PRINT("DNS1 : ");
  DEBUG_PRINTLN(WiFi.dnsIP(1));

  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i<5; i++) {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN,LOW);
    delay(200);
  }
}

unsigned long lastSensorRead=0;

void loop() {
  STATS_LOOP

  if ((millis() - lastSensorRead)>5000) {
    PROF_START(SensorRead);
    Serial.print("Sensor value (0-1024) : ");
    Serial.println(analogRead(0));
    PROF_STOP(SensorRead);
    lastSensorRead = millis();
  }
}
