#include <Arduino.h>
#include <BLEDevice.h>
#include "SparkFunLSM6DSO.h"
#include "Wire.h"

#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define LED_PIN 13
#define Task 1
#define Threshold 30
 
LSM6DSO myIMU; //Default constructor is I2C, addr 0x6B
int stepCount = 0;
bool forward = false;
BLECharacteristic *pCharacteristic = NULL;

class MyCallbacks: public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *pCharacteristic) {
     std::string value = pCharacteristic->getValue();

     if (!value.compare("!off")) {
      digitalWrite(LED_PIN, 0);
     } else if (!value.compare("!on")) {
      digitalWrite(LED_PIN, 1);

     }

     
 
     if (value.length() > 0) {
       Serial.println("*********");
       Serial.print("New value: ");
       for (int i = 0; i < value.length(); i++)
         Serial.print(value[i]);
      //Serial.print(value);
       Serial.println();
       Serial.println("*********");
     }
   }
};
 
void setup() {
 Serial.begin(9600);

 Wire.begin();

 pinMode(LED_PIN, OUTPUT);

 digitalWrite(LED_PIN, 0);

 
 Serial.println("1- Download and install an BLE scanner app in your phone");
 Serial.println("2- Scan for BLE devices in the app");
 Serial.println("3- Connect to MyESP32");
 Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
 Serial.println("5- See the magic =)");
 
 BLEDevice::init("SDSUCS");
 BLEServer *pServer = BLEDevice::createServer();
 
 BLEService *pService = pServer->createService(SERVICE_UUID);
 
 pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
 
 #if Task == 1
 pCharacteristic->setCallbacks(new MyCallbacks());
 #endif
 
 pCharacteristic->setValue("Hello World");
 pService->start();
 
 BLEAdvertising *pAdvertising = pServer->getAdvertising();
 pAdvertising->start();


 if( myIMU.begin() )
    Serial.println("Ready.");
  else { 
    Serial.println("Could not connect to IMU.");
    Serial.println("Freezing");
  }

  if( myIMU.initialize(BASIC_SETTINGS) )
    Serial.println("Loaded Settings.");
}
 
void loop() {
//Get all parameters

  if (forward) {
    if (myIMU.readFloatGyroY() < - Threshold) {
      forward = false;
      stepCount++;
    }
  } else {
    if (myIMU.readFloatGyroY() > Threshold) {
      forward = true;
    }
  }

  Serial.println(stepCount);
  pCharacteristic->setValue(stepCount);


  
  delay(50);
  }