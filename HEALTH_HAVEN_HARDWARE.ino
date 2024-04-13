#include <DFRobot_MAX30102.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "API_KEY"
#define FIREBASE_PROJECT_ID "PROJECT_ID"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "PASSWORD"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int count = 0;
const int LM35_PIN = 34;  //lm35 pin 
DFRobot_MAX30102 particleSensor;
char arrayStr[50];

void setup()
{

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");

    while (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
    delay(1000);
    }
   
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);

    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    particleSensor.sensorConfiguration(/*ledBrightness=*/50, /*sampleAverage=*/SAMPLEAVG_4, \
                        /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
                        /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);


    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
    Firebase.reconnectNetwork(true);
    fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

    fbdo.setResponseSize(2048);

    Firebase.begin(&config, &auth);
}

int32_t SPO2; 
int8_t SPO2Valid; 
int32_t heartRate; 
int8_t heartRateValid;

void loop()
{
  int sensorValue = analogRead(LM35_PIN);

  float temperatureC = (sensorValue * 3.3 / 4095) * 100; 
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32;
  Serial.println(F("Wait about four seconds"));
  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
  

  Serial.print(F("heartRate="));
  Serial.print(heartRate, DEC);
  Serial.print(F(", heartRateValid="));
  Serial.print(heartRateValid, DEC);
  Serial.print(F("; SPO2="));
  Serial.print(SPO2, DEC);
  Serial.print(F(", SPO2Valid="));
  Serial.println(SPO2Valid, DEC);

  Serial.print("Temperature: ");
  Serial.print(temperatureF);
  Serial.println(" °F");

  //firebase code

    if (Firebase.ready())
    {
      if(SPO2Valid==1 && heartRateValid==1){
        Serial.print("Commit a document (append array)... ");
        std::vector<struct firebase_firestore_document_write_t> writes;
        struct firebase_firestore_document_write_t transform_write;
        transform_write.type = firebase_firestore_document_write_type_transform;
        transform_write.document_transform.transform_document_path = "sensordata/patient";

        struct firebase_firestore_document_write_field_transforms_t field_transforms;

        field_transforms.fieldPath = "appended_data";
        field_transforms.transform_type = firebase_firestore_transform_type_append_missing_elements;

        FirebaseJson content;
        String arrayStr = "[" + String(heartRate) + "," + String(SPO2) + "," + String((int)temperatureF) + "," + "]";
        
        content.set("values/[0]/stringValue", arrayStr);
        //content.set("values/[1]/integerValue", heartRate);
        //content.set("values/[2]/integerValue", temperatureF);

        field_transforms.transform_content = content.raw();

        transform_write.document_transform.field_transforms.push_back(field_transforms);

        writes.push_back(transform_write);

        if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of firebase_firestore_document_write_t */, "" /* transaction */))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
       }
    }

    delay(2000);
}



        // Set the transformation type.
        // firebase_firestore_transform_type_set_to_server_value,
        // firebase_firestore_transform_type_increment,
        // firebase_firestore_transform_type_maaximum,
        // firebase_firestore_transform_type_minimum,
        // firebase_firestore_transform_type_append_missing_elements,
        // firebase_firestore_transform_type_remove_all_from_array

