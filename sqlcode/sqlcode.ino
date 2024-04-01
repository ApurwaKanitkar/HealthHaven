#include <DFRobot_MAX30102.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

const char *ssid = "YourWiFiSSID";
const char *password = "YourWiFiPassword";
const char *host = "your_mysql_host";
const int port = 3306;
char user[] = "your_mysql_username"; // Make sure to use a writable char array
char db_name[] = "your_database_name"; // Make sure to use a writable char array
char db_password[] = "your_database_password"; // Make sure to use a writable char array

const int LM35_PIN = 34;  // Define the GPIO pin connected to the LM35 sensor
DFRobot_MAX30102 particleSensor;

WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(9600);
  while (!WiFi.begin(ssid, password)) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  IPAddress serverIP;
  serverIP.fromString(host); // Convert host string to IPAddress object

  if (conn.connect(serverIP, port, user, db_password, db_name)) {
    Serial.println("Connected to database");
  } else {
    Serial.println("Connection failed");
  }

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

  // Read heart rate and SPO2
  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
  
  // Print readings to Serial monitor
  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  Serial.print("SPO2: ");
  Serial.println(SPO2);
  Serial.print("Temperature: ");
  Serial.println(temperatureF);

  // Insert readings into the database
  if (conn.connected()) {
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    char sql_query[255];
    sprintf(sql_query, "INSERT INTO your_table_name (heart_rate, spo2, temperature) VALUES (%d, %d, %f)", heartRate, SPO2, temperatureF);
    cur_mem->execute(sql_query);
    delete cur_mem;
  } else {
    Serial.println("Error: Database not connected");
  }

  delay(1000);  // Delay for stability between readings
}

