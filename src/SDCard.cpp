#include "PinsAndConst.h"
#include "ClassesAndStuff.h"

SPIClass spi = SPIClass(VSPI);

SDCard::SDCard(){
    outputExist = false; //Used in WiFiMan to decide if need to upload output.csv or not, using SD.open("/output.csv", FILE_READ) might cause some I/O traffic errors
    available = false;
    cSFA(sflocation,_location);
    sflocation = "NaN";
    cSFA(sflocFile,_locFile);
    sflocFile = "/data/NaN.csv";
    //initialize to NaN to make sure if no location, at least shows NaN to show it defined the string, but something gone wrong with upLoc()
    count = 0; //this keeps track of the number of locations listed in the txt file
}

bool SDCard::verifyIntegrity(){
    if (SD.cardSize() == 0 || SD.cardType() == CARD_NONE || SD.usedBytes() == SD.totalBytes()) {
        Serial.println("\nSD card not detected or communication error");
        return false;
    } else {
        Serial.println("\nSD card OK");
        return true;
    }
}

void SDCard::initSD(){
    spi.begin(SCK, MISO, MOSI, CS);

    if (SD.begin(CS,spi,7500000) && SD.cardType() != CARD_NONE && SD.usedBytes() != SD.totalBytes()){
        available = true;
        Serial.println(" ");
        Serial.println("SD card attached");
        Serial.println(" ");
    }

    else{
        available = false;
        createSafeString(fatalMsg, 16);
        fatalMsg = "FATAL SD ERROR";
        sdcard.upLoc(fatalMsg);
        Serial.println(" ");
        Serial.println("No SD card attached or Card Mount Failed");
        Serial.println("I dunno which one plz don't hurt me");
        Serial.println(" ");
        return;
    }

    locIndex();
    if(!SD.exists("/loc.txt")){
        File file = SD.open("/loc.txt", FILE_WRITE);
        file.print("");
        file.close();
    }
    if(!SD.exists("/data")){
        Serial.println("Folder '/data' does not exist, creating...");
        SD.mkdir("/data");
    }
    if(SD.exists("/output.csv")){
        outputExist = true;
    }
}

void SDCard::upLoc(SafeString& newLocation){
    cSFA(sflocation,_location);
    sflocation = "\"";
    sflocation += newLocation;
    sflocation += "\""; //adds the ability to have commas
    //Changes reflected in (char*)_location
    cSFA(sflocFile,_locFile);
    sflocFile = "/data/";
    sflocFile += newLocation;
    sflocFile += ".csv";
    //Changes reflected in (char*)_locFile;
    Serial.println("______________");
    Serial.println((char*)_location);
    Serial.println((char*)_locFile);
}

void SDCard::locIndex(){

    available = verifyIntegrity();
    if(!available){
        Serial.println(" ");
        Serial.println("SD Card not available");
        Serial.println(" ");
        return;
    } //Very important safety check, do it every time

    File file = SD.open("/loc.txt", FILE_READ);

    if(file.size() == 0) {
        Serial.println("No Locations Listed");
        file.close();
        return;
    }

    if (file) {
        file.seek(0);  // Start from beginning
        count = 0;

        while (file.available() && count < sizeof(indexBuf) / sizeof(indexBuf[0])) {
            indexBuf[count++] = file.position();
            file.readStringUntil('\n');
        }
    }//Index all the new lines so user can select location

    file.seek(0); //just in case
    file.close(); //Don't forget to close file
}

bool SDCard::upLocFromIndex(int inputlineNum){

    int lineNum = ((inputlineNum % count) + count) % count;

    /*
    The above operations allows for any input number of any size
    so no matter what the input number is, 
    it will always wrap to a range of 0 to the max number of locations listed.

    This allows the input line number to increase or decrease 
    without worrying about going out of bounds.

    putting it here prevents me from forgetting to add later on
    and it's just safer in general
    */
    available = verifyIntegrity();
    if(!available){
        Serial.println(" ");
        Serial.println("SD Card not available");
        Serial.println(" ");
        return false;
    } //Very important safety check, do it every time

    if (lineNum < 0 || lineNum >= sizeof(indexBuf)/sizeof(indexBuf[0])) {
        Serial.println("Line number out of bounds");
        return false;
    } //just in case

    File file = SD.open("/loc.txt", FILE_READ);

    if(file.size() == 0) {
        Serial.println("No Locations Listed");
        file.close();
        return false;
    }

    if(file){
        file.seek(0);
        file.seek(indexBuf[lineNum]);
        if(file.available()){
            memset(loc, 0, sizeof(loc)); //clears loc buffer
            int loclen = file.readBytesUntil('\n', loc, sizeof(loc)-1);
            loc[loclen] = '\0';
            createSafeStringFromCharArray(sfloc,loc);
            sfloc.trim();
            upLoc(sfloc);
            file.close();
            Serial.println("\nLocation Updated\n");
            Serial.println((char*)loc);
            return true;
        }
        else{
            file.close();
            Serial.println("\nLocation Not Updated\n");
            return false;
        }
    }

    return false; //just in case, doesn't hurt

}

void SDCard::addEntry(){

    available = verifyIntegrity();
    if(!available){
        Serial.println(" ");
        Serial.println("SD Card not available");
        Serial.println(" ");
        return;
    } //Very important safety check, do it every time

    if(!SD.exists("/data")){
        SD.mkdir("/data");
    }
    
    SD.remove((char*)_locFile);
    File file = SD.open((char*)_locFile, FILE_WRITE);

    if (file) {
        //Location,Nitrogen,Phosporus,Potassium,Soil Moisture,Air Quality
        file.print((char*)_location); file.print(',');
        file.print(sensor.NitroVal); file.print(',');
        file.print(sensor.PhosVal); file.print(',');
        file.print(sensor.PotaVal); file.print(',');
        file.print(sensor.MoistVal); file.print(',');
        file.print(sensor.AirVal); file.print('\n');
        file.close();
        Serial.println("Write done.");
        return;
    } 
    else {
        Serial.println("Failed to open file for writing");
        return;
    }

}

void SDCard::delEntry(){

    available = verifyIntegrity();
    if(!available){
        Serial.println(" ");
        Serial.println("SD Card not available");
        Serial.println(" ");
        return;
    } //Very important safety check, do it every time

    sensor.resetAll();
    SD.remove((char*)_locFile);
    serverOut();

}

void SDCard::serverOut() {

    available = verifyIntegrity();
    if (!available) {
        Serial.println("\nSD Card not available\n");
        return;
    }

    SD.remove("/output.csv");
    outputExist = false;

    // Ensure folder exists
    if (!SD.exists("/data")) {
        Serial.println("Folder '/data' not found, creating...");
        if (!SD.mkdir("/data")) {
            Serial.println("Failed to create '/data' folder");
            return;
        }
    }

    File dir = SD.open("/data");
    if (!dir || !dir.isDirectory()) {
        Serial.println("'/data' not a directory or failed to open");
        return;
    }

    // Open output file
    File targetFile = SD.open("/output.csv", FILE_WRITE);
    if (!targetFile) {
        Serial.println("Failed to open output.csv for writing");
        dir.close();
        return;
    }

    // Write CSV header
    targetFile.println("Location,Nitrogen,Phosporus,Potassium,Soil Moisture,Air Quality");

    // Iterate over all files in /data
    File entry = dir.openNextFile();
    while (entry) {
        if (!entry.isDirectory()) {
            Serial.print("Appending file: ");
            Serial.println(entry.name());

            // Copy file contents to output
            const size_t bufferSize = 128;
            uint8_t buffer[bufferSize];
            int bytesRead;

            while ((bytesRead = entry.read(buffer, bufferSize)) > 0) {
                targetFile.write(buffer, bytesRead);
            }
        }
        entry.close();
        entry = dir.openNextFile(); // move to next file
    }

    targetFile.close();
    dir.close();

    Serial.println("All files in '/data' appended successfully.");
    outputExist = true;
    wifiman.sync();
}