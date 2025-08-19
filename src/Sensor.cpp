#include "PinsAndConst.h"
#include "ClassesAndStuff.h"

HardwareSerial npk(2);

Sensor::Sensor(){
    NitroVal = 0;
    PhosVal = 0;
    PotaVal = 0;
    MoistVal = 0;
    AirVal = 0;
}

void Sensor::initSensor(){
    npk.begin(9600, SERIAL_8E1, RXD, TXD);
}

void Sensor::resetAll(){
    NitroVal = 0;
    PhosVal = 0;
    PotaVal = 0;
    MoistVal = 0;
    AirVal = 0;

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