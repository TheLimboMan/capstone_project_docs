#include "PinsAndConst.h"
#include "ClassesAndStuff.h"
#include "Secrets.h"

WiFiClient client;
HTTPClient http;
WebServer server(80);
Preferences prefs;
DNSServer dnsServer;

const char* ap_ssid = "CAPSTONE_AP";
const char* ap_pass = "123";

const byte DNS_PORT = 53;

WiFiMan::WiFiMan(){
    spiffAvailable = true;
    POSTFailed = false;
    WiFiStatus = false;
    syncing = false;
}

void WiFiMan::handleGetConfig() {
    prefs.begin("wifi", true);
    String ssid = prefs.getString("ssid", WIFI_SSID);
    String pass = prefs.getString("pass", WIFI_PASSWORD);
    String database = prefs.getString("database", SERVER_NAME);
    prefs.end();

    String json = "{";
    json += "\"ssid\":\"" + ssid + "\",";
    json += "\"pass\":\"" + pass + "\",";
    json += "\"database\":\"" + database + "\"";
    json += "}";
    server.send(200, "application/json", json);
}

void WiFiMan::handleRoot(){
    if(!spiffAvailable){
        return;
    }
    
    File f = SPIFFS.open("/portal.html", "r");
    if (!f) {
        server.send(404, "text/plain", "portal.html not found");
        return;
    }
    server.streamFile(f, "text/html");
    f.close();
}

void WiFiMan::handleGet(){
    if(!spiffAvailable){
        return;
    }

    // Read values
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    String database = server.arg("database");

    // Store to Preferences (NVS)
    prefs.begin("wifi", false);
    if (ssid.length() > 0) {
        prefs.putString("ssid", ssid);
        Serial.printf("Updated SSID: %s\n", ssid.c_str());
    }
    if (pass.length() > 0) {
        prefs.putString("pass", pass);
        Serial.printf("Updated PASS: %s\n", pass.c_str());
    }
    if (database.length() > 0) {
        prefs.putString("database", database);
        Serial.printf("Updated DB: %s\n", database.c_str());
    }
    prefs.end();

    server.send(200, "text/plain", "OK");

    delay(500);
    WiFi.disconnect();
    WiFi.begin(ssid,pass);
}

void WiFiMan::handleGetloc(){
    sdcard.available = sdcard.verifyIntegrity();
    if (!sdcard.available) {
        Serial.println("\nSD Card not available\n");
        server.send(500, "text/plain", "Failed to open loc.txt");
        return;
    }

    File f = SD.open("/loc.txt");
    if(f){
      String content;
      while(f.available()){
        content += (char)f.read();
      }
      f.close();
      server.send(200, "text/plain", content);
    } else {
      server.send(200, "text/plain", ""); // empty if missing
    }
}

void WiFiMan::handleSaveloc(){
    sdcard.available = sdcard.verifyIntegrity();
    if (!sdcard.available) {
        Serial.println("\nSD Card not available\n");
        server.send(500, "text/plain", "Failed to open loc.txt");
        return;
    }

    if(!server.hasArg("plain")){
      server.send(400, "text/plain", "No data");
      return;
    }

    String body = server.arg("plain");

    SD.remove("/loc.txt"); 
    File f = SD.open("/loc.txt", FILE_WRITE);
    if(!f){
      server.send(500, "text/plain", "Failed to open loc.txt");
      return;
    }

    f.print(body);  // Write new locations
    f.close();
    sdcard.locIndex();

    server.send(200, "text/plain", "OK");
}

void WiFiMan::handleDelloc() {
    sdcard.available = sdcard.verifyIntegrity();
    if (!sdcard.available) {
        Serial.println("\nSD Card not available\n");
        server.send(500, "text/plain", "Failed to open loc.txt");
        return;
    }

    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "No data received");
        return;
    }

    String deleted = server.arg("plain");
    int start = 0;
    int deletedCount = 0;

    while (true) {
        int end = deleted.indexOf('\n', start);
        String line;
        if (end == -1) {
            line = deleted.substring(start);
        } 
        else {
            line = deleted.substring(start, end);
        }
        line.trim();

        if (line.length() > 0) {
            String path = "/data/" + line + ".csv";

            if (SD.exists(path)) {
                if (SD.remove(path)) {
                    Serial.printf("Deleted file: %s\n", path.c_str());
                    deletedCount++;
                } 
                else {
                    Serial.printf("Failed to delete: %s\n", path.c_str());
                }
            } 
            else {
                Serial.printf("File not found: %s\n", path.c_str());
            }
        }

        if (end == -1) break;
        start = end + 1;
    }

    String response = "Deleted " + String(deletedCount) + " files";
    server.send(200, "text/plain", response);
}


void WiFiMan::wifiWatchDog(){
    bool resync = false;
    for(;;){
        while (WiFi.status() != WL_CONNECTED) {
            WiFiStatus = false;
            resync = true;
            Serial.println(" ");
            Serial.println("Connection Lost, Getting Back Online...");
            delay(1000);
        }
        WiFiStatus = true;
        if(resync == true){
            Serial.println("\nAttempting to Sync After Reconnection");
            sync();
            resync = false;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void WiFiMan::initWiFi(){
    //Setup WiFI
    WiFi.mode(WIFI_AP_STA);

    //AP
    WiFi.softAP(ap_ssid, ap_pass);
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP IP: ");
    Serial.println(apIP);
    dnsServer.start(DNS_PORT, "*", apIP);

    //Setup SPIFF
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS mount failed, can't load web server");
        spiffAvailable = false;
    }

    else{
        server.on("/del_loc", HTTP_POST, [this]() { handleDelloc(); });
        server.on("/save_loc",HTTP_POST,[this]() { handleSaveloc(); });
        server.on("/loc.txt",HTTP_GET,[this]() { handleGetloc(); });
        server.on("/get_config", HTTP_GET, [this]() { handleGetConfig(); });
        server.on("/", HTTP_GET, [this]() { handleRoot(); });
        server.on("/get", HTTP_GET, [this]() { handleGet(); });
        server.onNotFound([this]() { handleRoot(); });

        server.begin();
        Serial.println("Web server started");
    }

    //WiFi

    prefs.begin("wifi", true);

    String ssid = prefs.getString("ssid", WIFI_SSID);
    String pass = prefs.getString("pass", WIFI_PASSWORD);

    WiFi.begin(ssid.c_str(),pass.c_str());

    prefs.end();

    Serial.print("\nConnecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
    WiFiStatus = true;
    sync();

    return;
}

void WiFiMan::sync(){
    prefs.begin("wifi", true);
    String database = prefs.getString("database", SERVER_NAME);
    prefs.end();

    if(sdcard.outputExist && WiFiStatus){
        Serial.println("Syncing...");
        lcdclass.encoderEnable = false;
        syncing = true;
        File file = SD.open("/output.csv",FILE_READ);
        if (!file) {
            Serial.println("output.csv is broken, Stopping Sync...");
            syncing = false;
            return;
        }
        http.begin(client, database.c_str());
        http.addHeader("Content-Type", "text/csv");
        int httpResponseCode = http.sendRequest("POST", &file, file.size());

        if (httpResponseCode > 0) {
            Serial.printf("Server responded: %d\n", httpResponseCode);
            Serial.println(http.getString());
        } else {
            Serial.printf("POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
            POSTFailed = true;
        }

        http.end();
        file.close();
        syncing = false;
        lcdclass.encoderEnable = true;
    }
    else{
        Serial.println("Something Messed Up, Syncing Not Possible");
        syncing = false; //redundant, but keeping it here just in case I failed to notice a discrepancy
    }
}
