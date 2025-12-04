#ifndef PINSANDCONST_H
#define PINSANDCONST_H

//LIB INCLUDES

#include <Arduino.h>
#include <SafeString.h>
#include <ItemSubMenu.h>
#include <ItemBack.h>
#include <ItemCommand.h>
#include <ItemLabel.h>
#include <ItemValue.h>
#include <LcdMenu.h>
#include <MenuScreen.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include <input/SimpleRotaryAdapter.h>
#include <string>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SPIFFS.h"

//SD CARD PINS

#define SCK 18
#define MISO 21
#define MOSI 19 //using #define here as it might conflict with SPI.h if do const int
const int CS = 5;

//ENCODER PINS

const int CLK = 4;
const int DT = 2;

const int SW = 15;

//LCD PINS

#define SDA 25
#define SCL 26 //using #define here as it might conflict with LiquidCrystal_I2CAdapter.h if do const int

//AIR QUALITY SENSOR PINS

const int AirPin = 34;

//MOISTURE SENSOR PINS

const int MoistPin = 32;

//LED PINS

const int ledR = 27;
const int ledG = 14;
const int ledB = 12;

//NPK STUFF

/*
+-----------+---------------+-------------------+
| Address   | Function Code | Query Type        |
+-----------+---------------+-------------------+
| 0x05      | 0x03          | Nitrogen (0x03)   |
| 0x05      | 0x03          | Phosphorus (0x04) |
| 0x05      | 0x03          | Potassium (0x05)  |
+-----------+---------------+-------------------+
*/

const __UINT8_TYPE__ nitro[] = {0x05, 0x03, 0x00, 0x03, 0x00, 0x06, 0x34, 0x4C};
const __UINT8_TYPE__ phos[] = {0x05, 0x03, 0x00, 0x04, 0x00, 0x06, 0x85, 0x8D};
const __UINT8_TYPE__ pota[] = {0x05, 0x03, 0x00, 0x05, 0x00, 0x06, 0xD4, 0x4D};

const int sampleSize = 5; //For NPK Sensor, this adjusts the amounts of time it samples the NPK readings, so that the results can be more consistent
//Maybe the ability to change sample size in software, but that will be for another day hehe :)

//RS485 to TTL PINS

const int RXD = 16;
const int TXD = 17;

//COLORS
const int red = 0b100;
const int green = 0b010;
const int blue = 0b001;

const int magenta = 0b101;
const int yellow = 0b110;
const int cyan = 0b011;

const int white = 0b111;
const int off = 0;

/*
const int repeatAmount = 450;
const int repeatDelay = 2000; //in ms
*/

const int repeatAmount = 5;
const int repeatDelay = 2000; //in ms

#endif
