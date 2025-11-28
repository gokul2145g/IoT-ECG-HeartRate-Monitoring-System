/*
  IoT-Based ECG Heart Rate Monitoring System
  
  This Arduino sketch reads ECG signals from an AD8232 sensor,
  processes the data, calculates heart rate, and sends it to
  an IoT cloud platform (Ubidots/ThingSpeak) via WiFi (ESP32/ESP8266).
  
  Components:
  - AD8232 ECG Sensor
  - ESP32 or ESP8266
  - Electrodes (RA, LA, RL)
  
  Author: Gokul
  Date: 2025
*/

#include <WiFi.h>           // WiFi library for ESP32
#include <PubSubClient.h>   // MQTT library
#include <ArduinoJson.h>    // JSON library for data formatting

// ============ WiFi Configuration ============
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// ============ MQTT Configuration (Ubidots) ============
const char* mqtt_server = "industrial.api.ubidots.com";
const char* mqtt_user = "YOUR_UBIDOTS_USERNAME";
const char* token = "YOUR_UBIDOTS_TOKEN";
const int mqtt_port = 1883;
const char* device_label = "ecg-monitor";
const char* ecg_variable = "ecg_signal";
const char* heart_rate_variable = "heart_rate";

// ============ Pin Configuration ============
const int ECG_PIN = 35;          // A0 ADC pin for ECG signal (ESP32)
const int LO_PLUS_PIN = 26;      // LO+ indicator
const int LO_MINUS_PIN = 25;     // LO- indicator
const int SENSOR_LED = 2;        // Status LED

// ============ Signal Processing ============
const int BUFFER_SIZE = 200;     // Buffer for storing samples
const int SAMPLING_RATE = 250;   // Hz (milliseconds between samples)
const int THRESHOLD = 50;        // Peak detection threshold
const int MIN_DISTANCE = 50;     // Minimum distance between peaks (ms)

int ecg_buffer[BUFFER_SIZE];
int buffer_index = 0;
int heart_rate = 0;
int last_peak_time = 0;
int peak_count = 0;
unsigned long last_sample_time = 0;

// ============ WiFi and MQTT Clients ============
WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

// ============ Function Declarations ============
void setup_wifi();
void reconnect_mqtt();
void publish_ecg_data(int ecg_value, int bpm);
int detect_heart_rate(int* buffer, int size);
void read_ecg_sensor();
void print_status();

// ============ SETUP ============
void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("=== IoT ECG Heart Rate Monitor ===\n");
  
  // Initialize Pins
  pinMode(ECG_PIN, INPUT);
  pinMode(LO_PLUS_PIN, INPUT);
  pinMode(LO_MINUS_PIN, INPUT);
  pinMode(SENSOR_LED, OUTPUT);
  digitalWrite(SENSOR_LED, LOW);
  
  // Setup WiFi
  setup_wifi();
  
  // Setup MQTT
  mqtt_client.setServer(mqtt_server, mqtt_port);
  mqtt_client.setCallback(mqtt_callback);
  
  Serial.println("Setup Complete. Starting ECG monitoring...\n");
}

// ============ MAIN LOOP ============
void loop() {
  // Maintain WiFi Connection
  if (!WiFi.isConnected()) {
    Serial.println("WiFi disconnected. Reconnecting...");
    setup_wifi();
  }
  
  // Maintain MQTT Connection
  if (!mqtt_client.connected()) {
    reconnect_mqtt();
  }
  mqtt_client.loop();
  
  // Read ECG Sensor with Sampling Rate Control
  unsigned long current_time = millis();
  if (current_time - last_sample_time >= SAMPLING_RATE) {
    read_ecg_sensor();
    last_sample_time = current_time;
    
    // Every 200 samples (process heart rate)
    if (buffer_index >= BUFFER_SIZE) {
      buffer_index = 0;
      heart_rate = detect_heart_rate(ecg_buffer, BUFFER_SIZE);
      
      // Print and send data every second
      print_status();
      publish_ecg_data(ecg_buffer[BUFFER_SIZE-1], heart_rate);
    }
  }
}

// ============ WiFi Setup ============
void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  if (WiFi.isConnected()) {
    Serial.print("WiFi Connected! IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(SENSOR_LED, HIGH);
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

// ============ MQTT Callback ============
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Message Received on topic: ");
  Serial.print(topic);
  Serial.print(" - ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// ============ MQTT Reconnection ============
void reconnect_mqtt() {
  int attempts = 0;
  while (!mqtt_client.connected() && attempts < 5) {
    Serial.print("Connecting to MQTT Server...");
    
    if (mqtt_client.connect(device_label, mqtt_user, token)) {
      Serial.println(" Connected!");
      Serial.println("MQTT broker connected successfully");
    } else {
      Serial.print(" Failed (rc=");
      Serial.print(mqtt_client.state());
      Serial.println(")");
      delay(2000);
      attempts++;
    }
  }
}

// ============ Read ECG Sensor ============
void read_ecg_sensor() {
  // Check Sensor Connection Status
  int lo_plus = digitalRead(LO_PLUS_PIN);
  int lo_minus = digitalRead(LO_MINUS_PIN);
  
  if (lo_plus == 1 || lo_minus == 1) {
    Serial.println("WARNING: Sensor disconnected or electrodes not in contact!");
    return;
  }
  
  // Read Analog Signal
  int ecg_value = analogRead(ECG_PIN);
  
  // Store in Buffer
  ecg_buffer[buffer_index] = ecg_value;
  buffer_index++;
  
  // Optional: Print raw data for debugging
  // Serial.println("ECG: " + String(ecg_value));
}

// ============ Peak Detection Algorithm ============
int detect_heart_rate(int* buffer, int size) {
  int peak_threshold = 2048 + THRESHOLD;  // Set threshold above baseline
  int peaks = 0;
  int last_peak_index = 0;
  
  // Find peaks in the buffer
  for (int i = 1; i < size - 1; i++) {
    // Check if current sample is a local maximum
    if (buffer[i] > buffer[i-1] && buffer[i] > buffer[i+1] && buffer[i] > peak_threshold) {
      // Check minimum distance between peaks
      if (i - last_peak_index > MIN_DISTANCE) {
        peaks++;
        last_peak_index = i;
      }
    }
  }
  
  // Calculate BPM
  // Buffer size 200 at 250 Hz = 0.8 seconds of data
  // Scale to 60 seconds: (peaks / 0.8) * 60 = peaks * 75
  int bpm = peaks * 75;  // Adjust multiplier based on your actual sampling
  
  // Constrain BPM to reasonable range (40-200)
  if (bpm < 40) bpm = 0;  // No valid reading
  if (bpm > 200) bpm = 200;  // Invalid reading
  
  return bpm;
}

// ============ Publish Data to MQTT ============
void publish_ecg_data(int ecg_value, int bpm) {
  if (!mqtt_client.connected()) {
    return;  // Not connected, skip publishing
  }
  
  // Create JSON payload
  StaticJsonDocument<100> doc;
  doc["value"] = ecg_value;
  
  String json_string;
  serializeJson(doc, json_string);
  
  // Publish ECG Signal
  String ecg_topic = String("/v2.0/devices/") + device_label + "/" + ecg_variable + "/lv";
  mqtt_client.publish(ecg_topic.c_str(), json_string.c_str());
  
  // Publish Heart Rate
  doc["value"] = bpm;
  json_string = "";
  serializeJson(doc, json_string);
  String heart_rate_topic = String("/v2.0/devices/") + device_label + "/" + heart_rate_variable + "/lv";
  mqtt_client.publish(heart_rate_topic.c_str(), json_string.c_str());
  
  Serial.print("[Published] ECG: ");
  Serial.print(ecg_value);
  Serial.print(" | Heart Rate: ");
  Serial.print(bpm);
  Serial.println(" BPM");
}

// ============ Print Status to Serial ============
void print_status() {
  Serial.print("[Timestamp: ");
  Serial.print(millis() / 1000);
  Serial.print("s] ");
  Serial.print("ECG Signal: ");
  Serial.print(ecg_buffer[BUFFER_SIZE-1]);
  Serial.print(" | Heart Rate: ");
  Serial.print(heart_rate);
  Serial.print(" BPM | WiFi: ");
  Serial.print(WiFi.isConnected() ? "Connected" : "Disconnected");
  Serial.print(" | MQTT: ");
  Serial.println(mqtt_client.connected() ? "Connected" : "Disconnected");
}

/*
  TODO / Future Enhancements:
  - [ ] Add SD card logging for offline data storage
  - [ ] Implement more advanced signal filtering (butterworth, IIR)
  - [ ] Add OLED display for local monitoring
  - [ ] Implement abnormality detection (arrhythmia alerts)
  - [ ] Add Bluetooth connectivity for mobile app
  - [ ] Battery power optimization for wearable
  - [ ] Data encryption for cloud transmission
  - [ ] Multi-patient monitoring capability
*/
