#include "PinsAndConst.h"
#include "ClassesAndStuff.h"

HardwareSerial npk(2);

Sensor::Sensor(){
    NitroVal = 0;
    PhosVal = 0;
    PotaVal = 0;
    MoistVal = 0;
    MoistValTempSum = 0;
    AirVal = 0;
    AirValTempSum = 0;
    Repeat = false;
    previousMillis = millis();;
    sampleCount = 0;
}

void Sensor::initSensor(){
    npk.begin(9600, SERIAL_8E1, RXD, TXD);
}

void Sensor::resetAll(){
    NitroVal = 0;
    PhosVal = 0;
    PotaVal = 0;
    MoistVal = 0;
    MoistValTempSum = 0;
    AirVal = 0;
    AirValTempSum = 0;
    Repeat = false;
    previousMillis = millis();;
    sampleCount = 0;

    Serial.println(" ");

    Serial.println("===Reset Triggered, check if all is 0===");
    Serial.print("Nitrogen: "); Serial.println(NitroVal);
    Serial.print("Phosphorus: "); Serial.println(PhosVal);
    Serial.print("Potassium: "); Serial.println(PotaVal);
    Serial.print("Soil Moisture: "); Serial.println(MoistVal);
    Serial.print("Air Quality: "); Serial.println(AirVal);

    Serial.println(" ");
}

void Sensor::refreshAir(){
    AirVal = analogRead(AirPin);

    Serial.println(" ");
    Serial.print("Air Quality: "); Serial.print(AirVal);
    Serial.println(" ");
}

void Sensor::refreshMoist(){
    MoistVal = analogRead(MoistPin);

    Serial.println(" ");
    Serial.print("Soil Moisture: "); Serial.print(MoistVal);
    Serial.println(" ");
}

void Sensor::refreshNPK(){
    byte values[11];
    uint16_t finalVal = 0;
    byte index = 0;
    const uint8_t* cycleCommands[] = { nitro, phos, pota };
    int* cycleVar[] = {&NitroVal, &PhosVal, &PotaVal};

    Serial.println(" ");

    Serial.println("===NPK Time===");

    for (int i = 0; i<3; i++){
        for(int j = 0; j<sampleSize; j++){
            //Flushes the Serial Buffer
            while (npk.available()){
                npk.read();
            }
            npk.write(cycleCommands[i], 8); // Send Modbus command
            npk.flush();       // Ensure transmission complete

            delay(100);

            index = 0;
            while (npk.available() && index < 7) {
                values[index] = npk.read();
                index++;
            }

            if (index >= 5) {
                finalVal += (values[3] << 8) | values[4];
                Serial.println(finalVal);
            } 

            else {
                Serial.println("!!!Error: Incomplete response!!!");
                Serial.println("==============");
                Serial.println(" ");
                finalVal = 404;
                NitroVal = 0;
                PhosVal = 0;
                PotaVal = 0;
                MoistVal = 0;
                AirVal = 0;
                break;
            }
        }
        if (finalVal == 404){
            break;
        }
        *cycleVar[i] = finalVal/sampleSize; //assign average of sample size
        finalVal = 0;
        delay(10); //Just in case
        Serial.println("==============");
        Serial.println(" ");
    }
}

void Sensor::refreshAll(){
    refreshNPK();
    refreshAir(); 
    refreshMoist();
}

void Sensor::refreshRepeat(){
    if(!Repeat) return;
    
    if(sampleCount >= repeatAmount){
        AirVal = AirValTempSum/repeatAmount;
        MoistVal = MoistValTempSum/repeatAmount;
        Serial.print("Final Avg Air: "); Serial.println(AirVal);
        Serial.print("Final Avg Moist: "); Serial.println(MoistVal);

        Repeat = false;
        sampleCount = 0;
        AirValTempSum = 0;
        MoistValTempSum = 0;
        previousMillis = millis();
        return;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= repeatDelay){
        previousMillis = currentMillis;

        AirValTemp = analogRead(AirPin);
        MoistValTemp = analogRead(MoistPin);

        AirValTempSum += AirValTemp;
        MoistValTempSum += MoistValTemp;

        Serial.print("Sample "); Serial.print(sampleCount + 1); 
        Serial.print(": \nAir="); Serial.print(AirValTemp);
        Serial.print(" Moist="); Serial.println(MoistValTemp);
        Serial.print(" Current Air AVG="); Serial.println(AirValTempSum/(sampleCount+1));
        Serial.print(" Current Moist AVG="); Serial.println(MoistValTempSum/(sampleCount+1));

        sampleCount++;
    }
    
}

//make it so that it samples data for 15mins
//make it so that the if the npk sensor fails the other data can still be used, the point of failure is there.
//make this repeat special so it can be multithreading
