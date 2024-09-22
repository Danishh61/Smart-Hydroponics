#include <DHT.h>

// DHT sensor settings
#define DHTPIN 2        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// Sensor pins
#define TDS_PIN A0      // Analog pin for TDS sensor
#define GAS_PIN A2      // Analog pin for gas sensor

// Relay module pins
#define FAN1_RELAY 3    // Digital pin for fan 1 relay
#define FAN2_RELAY 4    // Digital pin for fan 2 relay
#define PUMP_RELAY 6    // Digital pin for pump relay

// Thresholds
#define TEMP_THRESHOLD 20.0     // Temperature threshold for fan control
#define GAS_THRESHOLD 200      // Gas sensor threshold for exhaust fans
#define TDS_THRESHOLD 600       // TDS sensor threshold for nutrient pump

void setup() {
  Serial.begin(9600);
  
  // Initialize the DHT sensor
  dht.begin();

  // Set relay module pins as outputs
  pinMode(FAN1_RELAY, OUTPUT);
  pinMode(FAN2_RELAY, OUTPUT);
  pinMode(PUMP_RELAY, OUTPUT);
  
  // Ensure all relays are initially off
  digitalWrite(FAN1_RELAY, LOW);
  digitalWrite(FAN2_RELAY, LOW);
  digitalWrite(PUMP_RELAY, LOW);
}

void loop() {
  // Read sensors
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int tdsValue = analogRead(TDS_PIN);
  int gasValue = analogRead(GAS_PIN);

  // Control fan 1 based on temperature
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(FAN1_RELAY, HIGH);
  } else {
    digitalWrite(FAN1_RELAY, LOW);
  }

  // Control fan 2 based on gas sensor
  if (gasValue > GAS_THRESHOLD) {
    digitalWrite(FAN2_RELAY, HIGH);
  } else {
    digitalWrite(FAN2_RELAY, LOW);
  }

  // Control pump based on TDS sensor
  if (tdsValue < TDS_THRESHOLD) {
    digitalWrite(PUMP_RELAY, HIGH);
  } else {
    digitalWrite(PUMP_RELAY, LOW);
  }

  // Display data on Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("TDS Value: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  delay(2000); // Update every 2 seconds
}
