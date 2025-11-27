/**
 * @file config.h
 * @brief Configuration settings for T-Beam S3 Core RF Detector
 * 
 * Hardware configuration for the LilyGO T-Beam S3 Core with SX1262/SX1280
 */

#ifndef CONFIG_H
#define CONFIG_H

// Board identification
#define DEVICE_NAME "SPUR RF Detector"
#define FIRMWARE_VERSION "1.0.0"

// Display configuration (SSD1306 128x64 OLED)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

// RF Scanning configuration
#define SCAN_INTERVAL_MS 50          // Time between frequency scans
#define RSSI_THRESHOLD -100          // Minimum RSSI to consider a signal detected
#define SIGNAL_HOLD_TIME_MS 3000     // How long to hold a detected signal

// 900MHz band configuration (SX1262 - LoRa module on T-Beam S3)
#define FREQ_900_START 860.0         // Start frequency in MHz
#define FREQ_900_END 930.0           // End frequency in MHz
#define FREQ_900_STEP 0.5            // Step size in MHz

// 2.4GHz band configuration (SX1280 - if connected)
#define FREQ_2400_START 2400.0       // Start frequency in MHz
#define FREQ_2400_END 2500.0         // End frequency in MHz
#define FREQ_2400_STEP 1.0           // Step size in MHz

// Common drone control frequencies (MHz)
// 900MHz band drones
#define DRONE_FREQ_900_1 902.0       // FCC 900MHz ISM band
#define DRONE_FREQ_900_2 915.0       // Common 915MHz frequency
#define DRONE_FREQ_900_3 868.0       // EU 868MHz frequency

// 2.4GHz band drones
#define DRONE_FREQ_2400_1 2405.0     // Common drone channel 1
#define DRONE_FREQ_2400_2 2440.0     // Common drone channel center
#define DRONE_FREQ_2400_3 2475.0     // Common drone channel high

// Modulation types detected
enum ModulationType {
    MOD_UNKNOWN = 0,
    MOD_FSK,
    MOD_GFSK,
    MOD_LORA,
    MOD_FHSS,
    MOD_DSSS,
    MOD_OFDM
};

// Menu states for UI
enum MenuState {
    MENU_MAIN = 0,
    MENU_SCAN_900,
    MENU_SCAN_2400,
    MENU_DETECTED,
    MENU_SETTINGS,
    MENU_INFO
};

// Signal detection result structure
struct DetectedSignal {
    float frequency;           // Detected frequency in MHz
    float rssi;               // Signal strength in dBm
    ModulationType modType;   // Detected modulation type
    uint32_t timestamp;       // Detection timestamp
    bool active;              // Is signal currently active
    uint8_t band;             // 0 = 900MHz, 1 = 2.4GHz
};

// Maximum number of signals to track
#define MAX_DETECTED_SIGNALS 10

#endif // CONFIG_H
