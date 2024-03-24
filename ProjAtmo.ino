// Air Quaity Sensor
// Michael Hansen
// 23/3/2024

/*
  Parts used: 
  BME688 Sensor Module
  Arduino Nano 33 IOT w/ Bluetooth

*/

/*
  This Sketch is intended to control an air quality and atmospheric sensor 
  unit comprised of a Bosche sensor and an Arduino Nano. This is a  part of
  a larger project called project Atmo
  For the full project story and scope, visit:

*/ 

// Initialize the libraries needed and then define BME680 sensor points
#include "Adafruit_BME680.h"
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Wire.h>
#define BME_CS 10
#define BME_MISO 12
#define BME_MOSI 11
#define BME_SCK 13
#define pAtSeaLevel (1013.25)

// Create our sensor Object
Adafruit_BME680 BSensor(BME_CS, BME_MISO, BME_MOSI, BME_SCK);

// Create a function to perform the 'burn-in'
void burnInSensor() {
  const unsigned long burnInDuration = 48 * 60 * 60 * 1000; // 48 hours in milliseconds
  unsigned long startTime = millis();
  unsigned long currentTime;
  time_t currentTime = now(); // Get current time
  char timeBuffer[20]; 
  strftime(timeBuffer, 20, "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

  Serial.println("Starting 48-hour sensor burn-in...");

  while (currentTime - startTime < burnInDuration) {
    if (!BSensor.performReading()) { 
      Serial.println("Failed reading :(");
    // CSV Output for Logging
    Serial.print(timeBuffer); 
    Serial.print(","); // Add comma as a separator
    Serial.print(BSensor.temperature);
    Serial.print(","); 
    Serial.print((BSensor.pressure) / 100);
    Serial.print(","); 
    Serial.print(BSensor.humidity);
    Serial.print(","); 
    Serial.print(BSensor.gas_resistance / 1000.0);
    Serial.print(","); 
    Serial.println();
    delay(5000); // Take readings every 5 seconds (adjust as needed)
    currentTime = millis();
    }
  }

  Serial.println("Burn-in complete!");
}


void setup() {

  /*
    Here we wil begin our serial monitor and test for module connection
    the while(1) statement will hold the loop until condition is met
  */
  Serial.begin(9600);
  Serial.println(F("Sensor Module Test: "));
  if (!BSensor.begin()){
    Serial.println("Searching for Sensor Module failed. Check connections");
    Serial.println("The system will pause until Sensor Module is detected");
    while(1);
  } 
  Serial.println("Sensor Module found. Proceeding with first reading:");
  // Lastly sensor config is initialized.
  BSensor.setTemperatureOversampling(BME680_OS_8X);
  BSensor.setHumidityOversampling(BME680_OS_2X);
  BSensor.setPressureOversampling(BME680_OS_4X);
  BSensor.setIIRFilterSize(BME680_FILTER_SIZE_3);
  BSensor.setGasHeater(300, 160);
  // Initiate the Burn in **** REMOVE AFTER FIRST RUN ***
  burnInSensor();
}

void loop() {
  // verify reading
  if (!BSensor.performReading()) {
    Serial.println("Failed reading :(");
    return;
  }

  // Display temperature in degrees Celsius
  Serial.print("Temperature = ");
  Serial.print(BSensor.temperature);
  Serial.println(" °C");
 
  // Display pressure in hPa (hectopascals)
  Serial.print("Pressure = ");
  Serial.print(BSensor.pressure / 100.0);
  Serial.println(" hPa");
 
  // Display humidity in percentage
  Serial.print("Humidity = ");
  Serial.print(BSensor.humidity);
  Serial.println(" %");
 
  // Calculate Dew Point
  float dewPoint = BSensor.temperature - ((100 - BSensor.humidity) / 5);
  Serial.print("Dew Point = ");
  Serial.print(dewPoint);
  Serial.println(" °C");
 
  Serial.print("Gas = ");
  Serial.print(BSensor.gas_resistance / 1000.0);
  Serial.println(" KOhms");
 
  Serial.print("Approx. Altitude = ");
  Serial.print(BSensor.readAltitude(pAtSeaLevel));
  Serial.println(" m");
 
  Serial.println();
 
  // Delay for 2 seconds before the next reading
  delay(2000);
}
