#define BLYNK_TEMPLATE_ID "TMPL3uZmkBT1r"
#define BLYNK_TEMPLATE_NAME "Iot project"
#define BLYNK_AUTH_TOKEN "vWP2TQvFe3exkLZOmbCXooZOE7X0BXj5"

#define BLYNK_PRINT Serial

#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>
#include <DHT.h>
#include <SimpleDHT.h>

// DHT22 sensor setup
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// TDS and gas sensor pins
#define TDS_PIN A0
#define GAS_PIN A1

// Relay pins
#define RELAY1 3 // Relay 1 for TDS
#define RELAY2 5 // Relay 2 for Fan
#define RELAY3 4 // Relay 3 for the secondary pump (Nutrition)

// Thresholds
#define TEMP_THRESHOLD 21.0 // Temperature in Celsius
#define TDS_MIN_THRESHOLD 600.0 // Minimum TDS value in ppm
#define TDS_MAX_THRESHOLD 800.0 // Maximum TDS value in ppm
#define TDS_AVG_THRESHOLD 700.0 // Average of min and max TDS values
#define GAS_THRESHOLD 50    // Gas in percentage (adjust as needed)

// Blynk credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "EcoHome";
char pass[] = "Ap-03@109";

// Timing variables for the main pump
const unsigned long pumpOnDuration = 1 * 60 * 1000; // 1 minute
const unsigned long pumpOffDuration = 5 * 60 * 1000; // 5 minutes
unsigned long previousPumpMillis = 0; // Variable to store last time pump was turned on

// Function to send sensor data to Blynk
void sendSensorData()
{
  // Read sensors
  float temperature = dht.readTemperature(); // Celsius
  float humidity = dht.readHumidity();       // Humidity percentage
  int tdsValue = analogRead(TDS_PIN);        // TDS value
  int gasValue = analogRead(GAS_PIN);        // Gas sensor value

  // Send sensor data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, tdsValue);
  Blynk.virtualWrite(V3, gasValue);

  // Control relays based on sensor thresholds
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(RELAY2, HIGH); // Activate relay 2 if temperature exceeds threshold
  } else {
    digitalWrite(RELAY2, LOW);  // Deactivate relay 2
  }

  // Control secondary pump based on TDS thresholds
  if (tdsValue < TDS_MIN_THRESHOLD) {
    digitalWrite(RELAY3, HIGH); // Turn on secondary pump to add nutrition
  } else if (tdsValue > TDS_AVG_THRESHOLD) {
    digitalWrite(RELAY3, LOW);  // Turn off secondary pump if TDS exceeds average value
  }

  // Print sensor values for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, TDS: ");
  Serial.print(tdsValue);
  Serial.print(" ppm, Gas: ");
  Serial.print(map(gasValue, 0, 1023, 0, 100)); // Map gas sensor value to percentage
  Serial.println(" %");
}

// Function to control the main pump
void controlMainPump()
{
  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Control pump based on timing
  if (currentMillis - previousPumpMillis >= pumpOnDuration + pumpOffDuration) {
    // Time to switch the pump
    previousPumpMillis = currentMillis;
    if (digitalRead(RELAY3) == LOW) {
      // If pump is off, turn it on
      digitalWrite(RELAY3, HIGH);
      Serial.println("Main pump turned on");
    } else {
      // If pump is on, turn it off
      digitalWrite(RELAY3, LOW);
      Serial.println("Main pump turned off");
    }
  }
}

// Blynk switch state change handler
BLYNK_WRITE(V4) {
  int relayState = param.asInt(); // Get the state of the switch
  
  // Depending on switch state, control RELAY3
  if (relayState == HIGH) {
    digitalWrite(RELAY3, HIGH); // Turn on RELAY3
    Serial.println("Main pump turned on via Blynk switch");
  } else {
    digitalWrite(RELAY3, LOW); // Turn off RELAY3
    Serial.println("Main pump turned off via Blynk switch");
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  // Initialize sensors
  dht.begin();

  // Initialize relays
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every second for sending sensor data
  sendSensorData(); // Call initially to send sensor data immediately
}

void loop()
{
  Blynk.run();
  controlMainPump(); // Call controlMainPump in the loop to manage main pump timing
}
