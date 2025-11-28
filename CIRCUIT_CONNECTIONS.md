# ECG Heart Rate Monitoring System - Circuit Connections Guide

## Overview
This document provides detailed hardware connections and circuit diagrams for the IoT-based ECG Heart Rate Monitoring System.

## Component Pinout

### AD8232 ECG Sensor Module

| Pin | Function | Description |
|-----|----------|-------------|
| GND | Ground | Connect to ESP32 GND |
| 3.3V | Power Supply | Connect to ESP32 3.3V |
| OUTPUT | Analog Output | ECG signal (ADC input) |
| LO+ | Lead-off Detection + | Detects electrode disconnection |
| LO- | Lead-off Detection - | Detects electrode disconnection |

### ESP32 Pinout (Used in This Project)

| ESP32 Pin | Function | Connected To |
|-----------|----------|---------------|
| GND | Ground | AD8232 GND, Electrodes RL |
| 3.3V | Power Supply | AD8232 3.3V |
| GPIO 35 (ADC Pin) | Analog Input | AD8232 OUTPUT |
| GPIO 26 | Digital Input | AD8232 LO+ |
| GPIO 25 | Digital Input | AD8232 LO- |
| GPIO 2 | Digital Output | Status LED (Optional) |

## Wiring Diagram

```
┌─────────────────┐
│    AD8232       │
│  ECG Sensor     │
└─────────────────┘
     │         │
   3.3V       GND
     │         │
     ▼         ▼
    ─────────────── 3.3V (ESP32)
     │         │
   OUTPUT    LO+    LO-
     │         │     │
     ▼         ▼     ▼
   GPIO35   GPIO26  GPIO25 (ESP32)

   USB Power Supply
     │
     ▼
   ─────── ESP32 (USB)
     │
     ▼
  WiFi Antenna (built-in)
```

## Electrode Placement (10-20 System)

### Standard Electrode Configuration

```
    HEAD
     ▲
    ╱ ╲
  LA   RA  (Left Arm, Right Arm)
  │       │
  │   ♥   │  (Heart)
  │       │
  └─ ─ ─ ─┘
     ║
    RL  (Right Leg - Reference)

LA (Negative) - Left Arm (Inside wrist)
RA (Positive) - Right Arm (Inside wrist)
RL (Ground)   - Right Leg (Inside ankle or shin)
```

### Pin Assignment to Electrodes

| Electrode | Placement | Connection | ECG Signal |
|-----------|-----------|-----------|-----------|
| RA (Positive/Red) | Right Arm | AD8232 INPUT+ | Measures positive potential |
| LA (Negative/Black) | Left Arm | AD8232 INPUT- | Measures negative potential |
| RL (Ground/Green) | Right Leg | AD8232 COM (Ground) | Reference potential |

### Electrode Placement Instructions

1. **RA (Right Arm)**
   - Location: Inside wrist of right arm, near the radial artery
   - Preparation: Clean skin with alcohol pad, wait 30 seconds
   - Application: Apply electrode firmly, ensure good contact

2. **LA (Left Arm)**
   - Location: Inside wrist of left arm, symmetrical to RA
   - Preparation: Same as RA
   - Application: Same as RA

3. **RL (Right Leg) - Reference**
   - Location: Inside ankle or lateral shin
   - Preparation: Same as RA
   - Application: Same as RA

## Connection Steps

### Step 1: Prepare Components
- Gather ESP32, AD8232 sensor, electrodes, jumper wires
- Ensure all components are in working condition

### Step 2: Power Connections
```
AD8232 GND    ────────────► ESP32 GND
AD8232 3.3V   ────────────► ESP32 3.3V
```

### Step 3: Signal Connections
```
AD8232 OUTPUT ────────────► ESP32 GPIO 35 (ADC)
AD8232 LO+    ────────────► ESP32 GPIO 26
AD8232 LO-    ────────────► ESP32 GPIO 25
```

### Step 4: Electrode Connections
```
Electrode RA  ────────────► AD8232 INPUT+
Electrode LA  ────────────► AD8232 INPUT-
Electrode RL  ────────────► AD8232 COM (Ground)
```

## Optional Connections

### Status LED (GPIO 2)
```
ESP32 GPIO 2  ────────────► Resistor (220Ω)
                                  │
                                  ▼
                              LED Anode
                                  │
                              LED Cathode
                                  │
                              ESP32 GND
```

### OLED Display (I2C) - For Future Enhancement
```
OLED SDA ──────────────► ESP32 GPIO 21 (SDA)
OLED SCL ──────────────► ESP32 GPIO 22 (SCL)
OLED GND ──────────────► ESP32 GND
OLED VCC ──────────────► ESP32 3.3V
```

### SD Card Module (SPI) - For Future Enhancement
```
SD CS   ──────────────► ESP32 GPIO 5
SD MOSI ──────────────► ESP32 GPIO 23
SD MISO ──────────────► ESP32 GPIO 19
SD SCK  ──────────────► ESP32 GPIO 18
SD GND  ──────────────► ESP32 GND
SD VCC  ──────────────► ESP32 3.3V
```

## AD8232 Sensor Configuration

### Reference Electrode Connection
The RL (Right Leg) electrode should be connected to the AD8232 COM pin via a 1 kΩ resistor:
```
Electrode RL ─────[1kΩ]─────► AD8232 COM
```

This provides a driven right leg (DRL) configuration for improved noise rejection.

## Power Supply Specifications

### USB Power (Recommended for Testing)
- Voltage: 5V USB (regulated to 3.3V on ESP32)
- Current Requirement: ~500mA maximum
- Use good quality USB cable and power adapter

### Battery Power (For Wearable)
- Battery Type: 18650 Li-ion or LiPo battery
- Voltage: 3.7V nominal (3.0V - 4.2V range)
- Capacity: 2000-3000 mAh for 4-6 hours operation
- Protection: Battery protection circuit required

## Signal Quality Optimization

### Noise Reduction

1. **Shielding**
   - Use shielded cables for ECG signal lines
   - Ground the shield at the ESP32 side only
   - Keep signal cables away from power lines

2. **Cable Length**
   - Keep electrode leads short (< 1 meter preferred)
   - Twist signal cables together
   - Avoid sharp bends in cables

3. **Power Supply**
   - Use filtered and regulated power supply
   - Add 100µF capacitor near AD8232 power pins
   - Add 10µF capacitor near ESP32 power pins

4. **Electrode Quality**
   - Use medical-grade electrodes
   - Ensure good skin contact (90-100% coverage)
   - Replace electrodes if conductivity drops
   - Check expiration date on electrode packs

## Troubleshooting Connections

### No Signal Detected
- ✓ Check electrode placement on skin
- ✓ Verify electrode conductivity (fresh gel)
- ✓ Check GPIO 35 is properly connected
- ✓ Verify AD8232 power connections
- ✓ Check for loose jumper wire connections

### Noisy or Unstable Signal
- ✓ Check LO+ and LO- connections (electrode contact detection)
- ✓ Ensure proper electrode skin preparation (clean, dry)
- ✓ Move away from EMI sources (WiFi router, microwave)
- ✓ Add shielding and grounding
- ✓ Check capacitor placement

### High Baseline Drift
- ✓ Verify RL (ground) electrode contact
- ✓ Check 1kΩ resistor in RL line
- ✓ Ensure COM pin connection to AD8232
- ✓ Wait 30 seconds after electrode placement

### Lead-off Detection Not Working
- ✓ Check GPIO 26 and GPIO 25 connections
- ✓ Verify GPIO configuration in Arduino code
- ✓ Test electrodes for proper contact
- ✓ Check AD8232 LO+ and LO- pins

## Testing the Circuit

### Pre-Deployment Checklist

- [ ] All power connections verified (3.3V, GND)
- [ ] Signal connections tested with multimeter
- [ ] Electrode impedance checked (< 1kΩ)
- [ ] Lead-off detection tested
- [ ] Serial monitor showing ECG values
- [ ] WiFi connecting successfully
- [ ] MQTT data transmitting to cloud
- [ ] Heart rate calculation working

### Quick Test Procedure

1. Upload Arduino code to ESP32
2. Open Serial Monitor (115200 baud)
3. Verify startup messages
4. Without electrodes, check for baseline noise
5. Apply electrodes to RA, LA, RL positions
6. Observe ECG waveform in serial output
7. Verify heart rate calculation (60-100 BPM typical)
8. Check cloud dashboard for data points

## Component Sourcing

### Recommended Suppliers (India)

| Component | Supplier | Notes |
|-----------|----------|-------|
| ESP32 | AliExpress, Amazon | ₹300-500 |
| AD8232 Module | AliExpress | ₹500-800 |
| Disposable Electrodes | Medical suppliers | ₹10-20 per pack |
| Jumper Wires | Amazon, Local shops | ₹100-200 per pack |
| USB Cable | Local shops | ₹50-100 |

**Total Hardware Cost: ₹1000-2000**

## Safety Considerations

⚠️ **Important Safety Notes:**

1. **Electrical Safety**
   - Always use isolated USB power supply
   - Do not connect to mains power directly
   - Keep circuit dry and away from water
   - Turn off before removing electrodes

2. **Skin Safety**
   - Test electrode sensitivity on small patch first
   - Discontinue if skin irritation occurs
   - Do not use on broken or irritated skin
   - Change electrodes every 24 hours

3. **Medical Safety**
   - This is not a medical device
   - Not for diagnosis or treatment
   - Consult healthcare professional for medical concerns
   - Do not use on patients with pacemakers

## References

- AD8232 Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ad8232.pdf
- ESP32 Pin Configuration: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32_pinouts.html
- ECG Electrode Placement: https://www.wikiwand.com/en/Electrocardiography#Lead_placement
- Arduino ESP32 Pinout Guide: https://github.com/espressif/arduino-esp32/blob/master/variants/esp32/pins_arduino.h

---

**Document Version:** 1.0  
**Last Updated:** 2025  
**Author:** Gokul  
