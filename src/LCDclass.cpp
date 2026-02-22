#include "PinsAndConst.h"
#include "ClassesAndStuff.h"

//LCD Setup
LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, 20, 4);
LcdMenu menu(renderer);

//Encoder Setup
SimpleRotary encoder(CLK, DT, SW);
SimpleRotaryAdapter my_encoder(&menu, &encoder);

int locationScroller = 0;

MENU_SCREEN(locationSelector, locationSelectorItems,
    ITEM_BASIC("Pick & Choose Loc"),
    ITEM_BASIC((char*)sdcard._location),
    ITEM_COMMAND("+", []() {sdcard.upLocFromIndex(++locationScroller); menu.refresh();}),
    ITEM_COMMAND("-", []() {sdcard.upLocFromIndex(--locationScroller); menu.refresh();}),

    ITEM_SUBMENU("DONE", dataReading)
);

MENU_SCREEN(dataReading, dataReadingItems,
    ITEM_COMMAND("Capture Data", []() {
        sensor.refreshAll(); 
        sdcard.addEntry(); 
        sdcard.serverOut();
        menu.refresh();
        }),
    ITEM_BASIC("===================="),
    ITEM_VALUE("N", sensor.NitroVal, "%d"),
    ITEM_VALUE("P", sensor.PhosVal, "%d"),
    ITEM_VALUE("K", sensor.PotaVal, "%d"),
    ITEM_VALUE("Air", sensor.AirVal, "%d"),
    ITEM_VALUE("Moist", sensor.MoistVal, "%d"),
    ITEM_BASIC("===================="),
    ITEM_SUBMENU("Main Menu", mainMenu)
);

MENU_SCREEN(mainMenu, mainMenuItems,
    ITEM_BACK(),
    ITEM_SUBMENU("Location Selector", locationSelector),
    ITEM_COMMAND("Upload Data", []() {wifiman.sync();}),
    ITEM_COMMAND("Del Current Data", []() {sdcard.delEntry();}),
    ITEM_COMMAND("Shut Down", [](){lcdclass.sysOff();})
);

void LCDclass::initLCD(){
    encoderEnable = true;
    Wire.begin(SDA, SCL);
    renderer.begin();
    menu.setScreen(locationSelector);
}

void LCDclass::sysOff(){
    Serial.println("\nShutting Down\n");
    encoderEnable = false;
    wifiman.sync();
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("Habis Shut Down");
    lcd.setCursor(0,1);
    lcd.print("Unplug Now");
    lcd.setBacklight(0);
    ledman.ledSet(off);
}