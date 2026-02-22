//This code is the firmware for my capstone project

#include "PinsAndConst.h"
#include "ClassesAndStuff.h"

/* Implement this if encounter Panics caused by Mutex
SemaphoreHandle_t statusMutex;
SemaphoreHandle_t sdcardMutex;
*/ 

//Multithreading Stuff
TaskHandle_t SecondThread;
TaskHandle_t ledThread;
TaskHandle_t APThread;

//Core Stuff
Sensor sensor;
ledManager ledman;
SDCard sdcard;
LCDclass lcdclass;
WiFiMan wifiman;

void SecondThreadFunc(void * pvParameters);
void ledThreadFunc(void * pvParameters);
void APMan(void * pvParameters);

void setup() {
  Serial.begin(115200);
  ledman.initLED();

  /* Implement this if encounter Panics caused by Mutex
  statusMutex = xSemaphoreCreateMutex();
  sdcardMutex = xSemaphoreCreateMutex();

  if (statusMutex == NULL || sdcardMutex == NULL) {
    Serial.println("Failed to create mutex, halting...");
    ledman.ledBlink(red,50,10);
    ledman.ledSet(off);
    while(1); // Halt
  } //Semaphore time
  */ 
  
  sensor.initSensor();
  sdcard.initSD();

  xTaskCreatePinnedToCore(
    SecondThreadFunc,
    "Secondary",
    10000,
    NULL,
    1,
    &SecondThread,
    1);
  
  xTaskCreatePinnedToCore(
    ledThreadFunc,
    "LED Manager",
    1024,
    NULL,
    1,
    &ledThread,
    1);

  xTaskCreatePinnedToCore(
    APMan,
    "AP Manager",
    8192,
    NULL,
    1,
    &APThread,
    1);

  lcdclass.initLCD(); //call this last
}

void loop() {
  if(lcdclass.encoderEnable){
    my_encoder.observe();
  }//This is so i can disable encoder when shut down
}

void SecondThreadFunc(void * pvParameters){
  wifiman.initWiFi();
  wifiman.wifiWatchDog();
}

void ledThreadFunc(void * pvParameters){
  for(;;){
    if(wifiman.syncing){
      ledman.ledBlink(blue,100,2);
    }
    else if(wifiman.POSTFailed){
      ledman.ledBlink(red,100,5);
      wifiman.POSTFailed = false;
    }
    else if(!wifiman.WiFiStatus || !sdcard.available){
      ledman.ledSet(red);
    }
    else{
      ledman.ledSet(green);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void APMan(void * pvParameters){
  for(;;){
    dnsServer.processNextRequest();
    server.handleClient(); 
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
