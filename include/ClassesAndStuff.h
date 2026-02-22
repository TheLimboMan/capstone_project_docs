#ifndef CLASSESANDSTUFF_H
#define CLASSESANDSTUFF_H

#include "PinsAndConst.h"

/*
***********************
*  Classes and Stuff  *
***********************
*/

class Sensor{
    public:
        int NitroVal;
        int PhosVal;
        int PotaVal;

        int MoistVal;
        
        int AirVal;
        
        //get data from sensor, store in var above
        Sensor();
        void initSensor();
        void resetAll();
        void refreshAir(); 
        void refreshMoist();
        void refreshNPK();
        void refreshAll();
};

class ledManager{
    public:
        void initLED();
        void ledBlink(int color, int interval, int amount); //interval in ms
        void ledSet(int color); //color is a number because binary number can be used to do bitwise operations and see what needs to be light up
};

class SDCard{
    private:
        char loc[150];
    public:
        char _location[103];
        char _locFile[113];
        int count = 0; //this keeps track of the number of locations listed in the txt file

        bool available; //always include check
        int indexBuf[30];

        bool outputExist;

        SDCard();
        bool verifyIntegrity();
        void initSD();
        void upLoc(SafeString& newLocation); //upLoc = Update Location, use (char*)_location to call the updated new location
        void locIndex(); 
        bool upLocFromIndex(int inputlineNum); //Already added safety mechanism to prevent selecting out of range
        void addEntry(); 
        void delEntry();
        void serverOut(); //format specifically for outputting to server
};

class LCDclass{
    public:
        bool encoderEnable;

        void initLCD();
        void sysOff(); //System Off Routine
};

class WiFiMan{
    private:
        bool spiffAvailable; //if this is false, then all is doomed

        void handleRoot();
        void handleGet();
        void handleGetConfig();
        void handleGetloc();
        void handleSaveloc();
        void handleDelloc();
    public:
        bool WiFiStatus;
        bool syncing;
        bool POSTFailed;

        WiFiMan();
        void initWiFi();
        void wifiWatchDog();
        void sync();
};

//global classes which will only ever have one instance //I ackowledge this is not best practice
extern Sensor sensor;
extern ledManager ledman;
extern SDCard sdcard;
extern LCDclass lcdclass;
extern WiFiMan wifiman;

//AP
extern WebServer server;
extern DNSServer dnsServer;

//NPK Serial Setup
extern HardwareSerial npk;

//SD Card SPI Setup
extern SPIClass spi;

//Encoder Setup
extern SimpleRotary encoder;
extern SimpleRotaryAdapter my_encoder;

//LCD Setup
extern LiquidCrystal_I2C lcd;
extern LiquidCrystal_I2CAdapter lcdAdapter;
extern CharacterDisplayRenderer renderer;
extern LcdMenu menu;

//Menu Screens
extern MenuScreen* locationSelector;
extern MenuScreen* dataReading;
extern MenuScreen* mainMenu;

#endif