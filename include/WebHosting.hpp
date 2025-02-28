//set up to host website and data exchange
//used this tutorial: https://m1cr0lab-esp32.github.io/remote-control-with-websocket/web-server-setup/

//all necessary library includes
#include <WiFi.h>
#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

//set up server hosting on esp32
WiFiManager wifiManager;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//function delcerations
void initSPIFFS();
void initWebServer();
void initWebServer();
void initWebSocket();
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

//function definitions
void initWebSetup() {
  initSPIFFS();
  //wifiManager.startConfigPortal(); //used to force setup page if needing change wifi
  wifiManager.setSTAStaticIPConfig(IPAddress(192,168,1,141), IPAddress(192,168,1,142), IPAddress(255,255,255,0));
  wifiManager.autoConnect("BioCapstoneESP"); //first parameter is name of access point, second is the password (if used) for the autoconnect feild
  initWebSocket();
  initWebServer();
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
  }
}

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html", false);
}

void initWebServer() {
  server.on("/", onRootRequest);
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

  switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
  }
}