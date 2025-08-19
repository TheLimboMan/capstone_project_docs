#include "PinsAndConst.h"
#include "ClassesAndStuff.h"

void ledManager::initLED(){
    pinMode(ledR, OUTPUT_OPEN_DRAIN);
    pinMode(ledG, OUTPUT_OPEN_DRAIN);
    pinMode(ledB, OUTPUT_OPEN_DRAIN);
}

void ledManager::ledBlink(int color, int interval, int amount){
    //resets the leds first
    for(int i = 0; i < amount; i++){
        digitalWrite(ledR, (color & 0b100) ? LOW : HIGH);
        digitalWrite(ledG, (color & 0b010) ? LOW : HIGH);
        digitalWrite(ledB, (color & 0b001) ? LOW : HIGH);
        delay(interval);
        digitalWrite(ledR, HIGH);
        digitalWrite(ledG, HIGH);
        digitalWrite(ledB, HIGH);
        delay(interval);
    }

    digitalWrite(ledR, (color & 0b100) ? LOW : HIGH);
    digitalWrite(ledG, (color & 0b010) ? LOW : HIGH);
    digitalWrite(ledB, (color & 0b001) ? LOW : HIGH);
}

void ledManager::ledSet(int color){
    digitalWrite(ledR, (color & 0b100) ? LOW : HIGH);
    digitalWrite(ledG, (color & 0b010) ? LOW : HIGH);
    digitalWrite(ledB, (color & 0b001) ? LOW : HIGH);
}