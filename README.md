# IoT-Based ECG Heart Rate Monitoring System

## Project Overview

This IoT-based ECG (Electrocardiogram) Heart Rate Monitoring System enables real-time monitoring of cardiac signals and heart rate data. Using an AD8232 ECG sensor module paired with an ESP8266 or ESP32 microcontroller, the system captures electrical heart signals, processes them, and transmits the data to a cloud platform for remote monitoring and analysis.

## Features

- **Real-time ECG Signal Capture**: Continuous acquisition of heart electrical signals via AD8232 sensor
- **Heart Rate Calculation**: Automated BPM (Beats Per Minute) detection using peak detection algorithms
- **IoT Integration**: WiFi connectivity for cloud data transmission via MQTT/HTTP
- **Remote Monitoring**: Access ECG waveforms and alerts through cloud dashboards (Ubidots/ThingSpeak)
- **Data Visualization**: Real-time graphs and historical data analysis
- **Abnormality Alerts**: Automatic detection and notification for irregular heart patterns
- **Offline Storage**: Local data logging capability
- **Wearable Design**: Compact and portable for continuous monitoring

## Hardware Components

### Essential Components
- **AD8232 ECG Sensor Module** - Signal amplification and filtering
- **ESP8266 or ESP32** - WiFi-enabled microcontroller
- **Electrodes** - RA (Right Arm), LA (Left Arm), RL (Right Leg) placement
- **Jumper Wires** - For connections
- **USB Cable** - For power and programming
- **Breadboard** (optional) - For prototype development

### Optional Components
- **LCD/OLED Display** - For local data visualization
- **SD Card Module** - For offline data logging
- **Battery Module** - For portable/wearable implementation
- **Blynk Module** - For mobile app integration

## Circuit Connections

### AD8232 to ESP32/ESP8266
```
AD8232 Pin      ESP32/ESP8266 Pin
3.3V     -----  3.3V
GND      -----  GND
OUTPUT   -----  A0 (ADC Pin)
LO+      -----  GPIO (optional - low battery indicator)
LO-      -----  GPIO (optional - low battery indicator)
```

### Electrode Placement
```
RA (Right Arm)    -> Positive electrode
LA (Left Arm)     -> Negative electrode  
RL (Right Leg)    -> Ground/Reference electrode
```

## Software Setup

### Arduino IDE Configuration

1. **Install Arduino IDE** from [arduino.cc](https://www.arduino.cc/en/software)

2. **Add ESP32/ESP8266 Board**:
   - Go to File → Preferences
   - Add board URL: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board Manager → Search "esp32" → Install

3. **Install Required Libraries**:
   ```
   - PubSubClient (for MQTT)
   - ArduinoJson (for JSON handling)
   - WiFi (built-in)
   - EEPROM (built-in)
   ```

### IoT Cloud Platform Setup

#### Using Ubidots (Recommended)
1. Create account at [ubidots.com](https://ubidots.com)
2. Create a new device "ECG_Monitor"
3. Add variable "ECG_Signal" and "Heart_Rate"
4. Copy API token from account settings
5. Update API credentials in Arduino code

#### Using ThingSpeak
1. Create account at [thingspeak.com](https://thingspeak.com)
2. Create new channel with fields: "ECG_Signal", "Heart_Rate"
3. Copy Channel ID and API Key
4. Update in Arduino sketch

## Arduino Code Outline

Key sections of the firmware:

```cpp
// WiFi configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Cloud credentials
const char* mqtt_server = "industrial.api.ubidots.com";
const char* token = "YOUR_UBIDOTS_TOKEN";

// Read ECG signal from A0
int ecg_signal = analogRead(A0);

// Peak detection for BPM calculation
int heart_rate = detectPeaks(ecg_signal);

// Send to cloud via MQTT
publishData(ecg_signal, heart_rate);
```

## Installation & Usage

1. **Clone Repository**:
   ```bash
   git clone https://github.com/gokul2145g/IoT-ECG-HeartRate-Monitoring-System.git
   ```

2. **Open Arduino Sketch**:
   - Open `ECG_Monitor.ino` in Arduino IDE

3. **Configure WiFi & Cloud Credentials**:
   - Edit SSID, password, API tokens
   - Select appropriate board (ESP32 or ESP8266)

4. **Upload to Microcontroller**:
   - Select correct COM port
   - Click Upload

5. **Verify in Serial Monitor**:
   - Open Serial Monitor (9600 baud)
   - Check WiFi connection and cloud data transmission

6. **Access Cloud Dashboard**:
   - View real-time ECG waveforms
   - Monitor heart rate trends
   - Set up alerts for abnormal values

## Data Flow

```
ECG Sensor (AD8232)
       ↓
   ESP32/ESP8266 (Processing)
       ↓
   WiFi Connection
       ↓
   MQTT/HTTP Protocol
       ↓
   Cloud Platform (Ubidots/ThingSpeak)
       ↓
   Dashboard & Mobile App
       ↓
   Alerts & Notifications
```

## Key Algorithms

### ECG Signal Filtering
- Low-pass filter: Remove high-frequency noise
- High-pass filter: Remove baseline drift
- Notch filter: Remove 50/60 Hz noise

### Heart Rate Calculation
- **R-Peak Detection**: Identify QRS complex peaks
- **RR Interval**: Time between consecutive R-peaks
- **BPM Formula**: `Heart_Rate = 60 / (RR_Interval in seconds)`

## Project Applications

- Personal health monitoring
- Telemedicine and remote patient monitoring
- Fitness tracking and sports performance analysis
- Clinical research and data collection
- IoT-based health management systems
- Smart wearable devices

## Advanced Features (Future Enhancements)

- [ ] Machine learning for arrhythmia detection
- [ ] Integration with medical databases
- [ ] Multi-patient monitoring dashboard
- [ ] Standalone wearable band design
- [ ] Battery optimization for long-term operation
- [ ] Android/iOS mobile app
- [ ] Cloud-based data analytics

## Troubleshooting

### No ECG Signal
- Check electrode placement and contact
- Verify AD8232 connections
- Check ADC pin configuration
- Ensure proper electrode adhesion

### WiFi Connection Issues
- Verify SSID and password
- Check WiFi signal strength
- Restart ESP module
- Check firewall settings

### Noisy Signal
- Add proper shielding
- Check power supply quality
- Use shorter jumper wires
- Add ferrite beads on analog lines

## Safety Precautions

⚠️ **Important**: This is a DIY/educational project and should NOT be used for medical diagnosis.
- Results are for monitoring purposes only
- Consult medical professionals for health decisions
- Ensure proper electrode hygiene
- Do not use on patients with implanted devices without medical guidance

## References

- AD8232 Datasheet: [Analog Devices](https://www.analog.com/media/en/technical-documentation/data-sheets/ad8232.pdf)
- ESP32 Documentation: [Espressif](https://docs.espressif.com/)
- MQTT Protocol: [mqtt.org](https://mqtt.org/)
- Ubidots IoT Platform: [ubidots.com](https://ubidots.com)

## File Structure

```
IoT-ECG-HeartRate-Monitoring-System/
├── ECG_Monitor.ino              # Main Arduino sketch
├── config.h                     # Configuration file
├── signal_processing.cpp        # Signal filtering & peak detection
├── mqtt_handler.cpp             # MQTT communication functions
├── CircuitDiagram.png           # Fritzing circuit diagram
├── Connections.md               # Detailed connection guide
├── README.md                    # This file
└── LICENSE
```

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Commit changes
4. Push to the branch
5. Create a Pull Request

## License

MIT License - See LICENSE file for details

## Author

**Gokul** - ECE/EEE Engineering Student
- GitHub: [@gokul2145g](https://github.com/gokul2145g)
- Portfolio Project for IoT & Embedded Systems

## Contact & Support

For questions or issues:
- Open an issue on GitHub
- Email: gokul@example.com

---

### Disclaimer
This project is for educational and monitoring purposes only. Always consult qualified healthcare professionals for medical advice and diagnosis.
