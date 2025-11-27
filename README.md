# SPUR - RF Drone Frequency Detector

A T-Beam S3 Core based RF detector for detecting drone control frequencies in the 900MHz and 2.4GHz bands. Uses RadioLib for RF signal scanning with modulation type identification and features a Meshtastic-style display UI.

## Features

- **Dual-Band Scanning**: Scans both 900MHz (SX1262) and 2.4GHz (SX1280) frequency bands
- **Modulation Detection**: Identifies common drone modulation types including:
  - FSK (Frequency Shift Keying)
  - GFSK (Gaussian FSK)
  - LoRa
  - FHSS (Frequency Hopping Spread Spectrum)
  - DSSS (Direct Sequence Spread Spectrum)
  - OFDM
- **Meshtastic-Style UI**: Clean, intuitive OLED display interface
- **Real-time Scanning**: Continuous frequency scanning with signal strength display
- **Signal Tracking**: Tracks up to 10 simultaneous signals

## Hardware Requirements

- **LilyGO T-Beam S3 Core** (ESP32-S3 based)
- **SX1262 LoRa Module** (built-in for 900MHz band)
- **SX1280 Module** (optional, for 2.4GHz band detection)
- **SSD1306 OLED Display** (128x64)

### Pin Configuration

| Function | GPIO Pin |
|----------|----------|
| SX1262 CS | 10 |
| SX1262 IRQ | 21 |
| SX1262 RST | 17 |
| SX1262 BUSY | 13 |
| SX1280 CS | 5 |
| SX1280 IRQ | 39 |
| SX1280 RST | 14 |
| SX1280 BUSY | 15 |
| OLED SDA | 42 |
| OLED SCL | 41 |
| Button | 0 |

## Building

This project uses PlatformIO. To build:

```bash
# Install PlatformIO Core
pip install platformio

# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

## Menu System

The UI provides a simple navigation system:

1. **Scan 900MHz** - Scan the 860-930 MHz band for drone signals
2. **Scan 2.4GHz** - Scan the 2400-2500 MHz band
3. **Detected** - View list of detected signals with frequency, RSSI, and modulation type
4. **Settings** - View current scanner settings
5. **Info** - Device information

Press the button to navigate menus. In scanning mode, press to return to main menu.

## Detected Drone Frequencies

### 900MHz Band
- 902-928 MHz: FCC ISM band (common in US)
- 868-870 MHz: EU LoRa band
- 915 MHz: Common control frequency

### 2.4GHz Band
- 2405 MHz: Common drone channel 1
- 2440 MHz: Center frequency
- 2475 MHz: High channel

## Dependencies

- [RadioLib](https://github.com/jgromes/RadioLib) - Multi-platform radio library
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) - OLED display driver
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) - Graphics library

## License

MIT License

## Acknowledgments

- Inspired by Meshtastic firmware UI design
- Uses LilyGO T-Beam S3 Core hardware platform
- RadioLib for comprehensive RF support
