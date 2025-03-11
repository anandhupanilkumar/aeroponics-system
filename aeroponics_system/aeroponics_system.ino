#include <WiFi.h>
#include <PubSubClient.h>

// Pin definitions
#define ANALOG_IN_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to voltage sensor
#define REF_VOLTAGE    3.3 // Reference voltage for ADC
#define ADC_RESOLUTION 4096.0 // 12-bit ADC resolution
#define R1             30000.0 // Resistor R1 in voltage divider (in ohms)
#define R2             7500.0  // Resistor R2 in voltage divider (in ohms)
#define BAT_FULL_VOLT  12.6 // Full charge voltage of the battery
#define CHARGING_CURRENT_PIN 34 // Pin to measure charging current
#define BAT_CURRENT_PIN 32 // Pin to measure battery current
#define BAT_VOLTAGE_PIN 35 // Pin to measure battery voltage
#define MOTOR_PIN 15 // Pin to control the motor
#define CHARGING_VOLTAGE_PIN 33 // Pin to measure charging voltage
#define CHARGING_EFFICIENCY 0.8 // Charging efficiency factor

// WiFi credentials
const char* ssid = "aeroponics_system";
const char* password = "aeroponics_system";

// MQTT Broker details
const char* mqtt_server = "broker.mqtt.cool"; // mqtt server
const int mqtt_port = 1883; // mqtt port number
const char* mqtt_user = ""; // Leave empty if no authentication
const char* mqtt_password = ""; // Leave empty if no authentication

// MQTT topics
const char* mqtt_topic_sub = "Battery143pub"; // Topic to subscribe to
const char* mqtt_topic_pub = "Battery143sub"; // Topic to publish to

WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to a null-terminated string
  char message[length + 1]; // +1 for null terminator
  memcpy(message, payload, length); // Copy payload to message buffer
  message[length] = '\0'; // Null-terminate the string

  // Check if the message is "system_on"
  if (strstr(message, "system_on")) {
    digitalWrite(MOTOR_PIN, LOW); // Turn on the motor
  } else if (strstr(message, "system_off")) {
    digitalWrite(MOTOR_PIN, HIGH); // Turn off the motor
  }
}

// Function to calculate battery percentage
uint8_t battery_percentage() {
  float bat_voltage = voltage(BAT_VOLTAGE_PIN); // Get battery voltage
  uint8_t battery_percentage = 0;

  // Calculate battery percentage based on voltage
  if (bat_voltage < 7.5) {
    battery_percentage = 0; // 0% if voltage is below 7.5V
  } else if (bat_voltage >= 7.5) {
    battery_percentage = ((bat_voltage - 7.5) / (BAT_FULL_VOLT - 7.5)) * 100; // Linear mapping
  }
  return battery_percentage;
}

// Function to measure voltage
float voltage(uint8_t voltagepin) {
  // Read the analog input
  int adc_value = analogRead(voltagepin);
  // Determine voltage at ADC input
  float voltage_adc = ((float)adc_value * 3.4) / ADC_RESOLUTION; // Convert ADC value to voltage
  // Calculate voltage at the sensor input
  float voltage_in = voltage_adc * (R1 + R2) / R2; // Apply voltage divider formula
  // Print results to serial monitor
  Serial.print("Measured Voltage = ");
  Serial.println(voltage_in, 3);
  return voltage_in;
}

// Function to measure current
float current(uint8_t currentpin) {
  float current = 0, adc_voltage = 0, adc = 0;
  // Take 500 samples for averaging
  for (int i = 0; i < 500; i++) {
    adc = analogRead(currentpin); // Read ADC value
    adc_voltage = adc * (3.4 / 4096.0); // Convert ADC value to voltage
    current += (adc_voltage - 1.53) / 0.100; // Calculate current using sensor sensitivity
  }
  current = current / 500; // Average the readings
  // Print results to serial monitor
  Serial.print("adc_voltage: ");
  Serial.println(adc_voltage);
  Serial.print("Current Value: ");
  Serial.println(current, 3);
  return current;
}

// Function to reconnect to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Resubscribe to the topic
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Function to calculate charging time
float charging_time(uint8_t soc, float current) {
  float time = (3.8 * (1 - soc / 100) / (current * CHARGING_EFFICIENCY)); // Charging time formula
  return time;
}

// Setup function
void setup() {
  Serial.begin(115200); // Setting baud rate for serial monitor
  pinMode(MOTOR_PIN, OUTPUT); // Set motor pin as output
  digitalWrite(MOTOR_PIN, HIGH); // Turn off motor initially
  setup_wifi(); // Connect to WiFi
  client.setServer(mqtt_server, mqtt_port); // Set MQTT server
  client.setCallback(callback); // Set MQTT callback
  pinMode(CHARGING_CURRENT_PIN, INPUT); // Set charging current pin as input
  pinMode(BAT_CURRENT_PIN, INPUT); // Set battery current pin as input
  pinMode(BAT_VOLTAGE_PIN, INPUT); // Set battery voltage pin as input
  pinMode(CHARGING_VOLTAGE_PIN, INPUT); // Set charging voltage pin as input
}

// Main loop
void loop() {
  if (!client.connected()) {
    reconnect(); // Reconnect to MQTT if disconnected
  }
  client.loop(); // Handle MQTT communication

  // Publish a message every 5 seconds
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    float time = 0;
    lastMsg = now;

    // Read sensor values
    uint8_t bat_percent = battery_percentage(); // Battery percentage
    float bat_current = current(BAT_CURRENT_PIN) - 0.26; // Battery current (with offset correction)
    float bat_voltage = voltage(BAT_VOLTAGE_PIN); // Battery voltage
    float charging_voltage = voltage(CHARGING_VOLTAGE_PIN); // Charging voltage
    float charging_current = current(CHARGING_CURRENT_PIN) - 0.22; // Charging current
    bool charging_state = (charging_voltage != 0) && (charging_current > 0.01) ? 1 : 0; // Charging state

    // Calculate charging time if charging
    if (charging_state == 1) {
      time = charging_time(bat_percent, charging_current);
    } else {
      time = 0;
    }

    // Create MQTT message
    char msg[100];
    snprintf(msg, sizeof(msg), "%d, %.2f, %.2f, %d, %.2f, %.2f", bat_percent, bat_voltage, bat_current, charging_state, charging_current, charging_voltage);
    client.publish(mqtt_topic_pub, msg); // Publish message
  }
}