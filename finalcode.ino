#include <DFRobot_MAX30102.h>
const int LM35_PIN = 34;  // Define the GPIO pin connected to the LM35 sensor
DFRobot_MAX30102 particleSensor;

void setup() {
  Serial.begin(9600); 
  while (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
    delay(1000);
  } // Initialize serial communication
  particleSensor.sensorConfiguration(/*ledBrightness=*/50, /*sampleAverage=*/SAMPLEAVG_4, \
                        /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
                        /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);

}

int32_t SPO2; //SPO2
int8_t SPO2Valid; //Flag to display if SPO2 calculation is valid
int32_t heartRate; //Heart-rate
int8_t heartRateValid;

void loop() {
  // Read the analog voltage from LM35 sensor
  int sensorValue = analogRead(LM35_PIN);

  // Convert analog value to temperature in Celsius
  float temperatureC = (sensorValue * 3.3 / 4095) * 100; 
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32;
  Serial.println(F("Wait about four seconds"));
  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
  //Print result 
  Serial.print(F("heartRate="));
  Serial.print(heartRate, DEC);
  Serial.print(F(", heartRateValid="));
  Serial.print(heartRateValid, DEC);
  Serial.print(F("; SPO2="));
  Serial.print(SPO2, DEC);
  Serial.print(F(", SPO2Valid="));
  Serial.println(SPO2Valid, DEC);
  // Print temperature to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureF);
  Serial.println(" °F");

  delay(1000);  // Delay for stability between readings
}
