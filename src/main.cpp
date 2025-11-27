/**
 * @file main.cpp
 * @brief SPUR RF Detector - Main Firmware
 * 
 * T-Beam S3 Core based RF detector for drone frequency detection
 * Uses RadioLib for RF scanning in 900MHz and 2.4GHz bands
 * Meshtastic-style UI for display
 * 
 * Hardware:
 * - LilyGO T-Beam S3 Core (ESP32-S3)
 * - SX1262 LoRa module (900MHz band)
 * - SX1280 module (2.4GHz band, optional external)
 * - SSD1306 128x64 OLED display
 */

#include <Arduino.h>
#include "config.h"
#include "rf_scanner.h"
#include "display_ui.h"

// Global instances
RFScanner rfScanner;
DisplayUI displayUI;

// Button handling
volatile bool buttonPressed = false;
unsigned long lastScanTime = 0;
bool scanning = false;

// Interrupt handler for button
void IRAM_ATTR buttonISR() {
    buttonPressed = true;
}

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(1000);  // Wait for serial
    
    Serial.println();
    Serial.println("================================");
    Serial.println(DEVICE_NAME);
    Serial.println("Firmware Version: " FIRMWARE_VERSION);
    Serial.println("================================");
    Serial.println();
    
    // Initialize display
    if (!displayUI.begin()) {
        Serial.println("[ERROR] Display initialization failed!");
    } else {
        displayUI.showSplash();
        delay(2000);
    }
    
    // Initialize RF scanner
    if (!rfScanner.begin()) {
        Serial.println("[ERROR] RF Scanner initialization failed!");
        Serial.println("[ERROR] No radio modules available");
    }
    
    // Setup button interrupt
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    
    Serial.println();
    Serial.println("[READY] SPUR RF Detector initialized");
    Serial.println("[INFO] Press button to navigate menu");
    Serial.println();
}

void loop() {
    // Handle button press
    if (buttonPressed) {
        buttonPressed = false;
        displayUI.handleButton();
        Serial.println("[UI] Button pressed");
    }
    
    // Get current menu state
    MenuState state = displayUI.getMenuState();
    
    // Perform scanning based on current view
    if (millis() - lastScanTime > SCAN_INTERVAL_MS) {
        lastScanTime = millis();
        
        switch (state) {
            case MENU_SCAN_900:
                if (rfScanner.is900MHzAvailable()) {
                    int detected = rfScanner.scan900MHz();
                    if (detected > 0) {
                        Serial.print("[SCAN] 900MHz: ");
                        Serial.print(detected);
                        Serial.println(" signals detected");
                        
                        // Print detected signals to serial
                        DetectedSignal* signals = rfScanner.getDetectedSignals();
                        for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
                            if (signals[i].active && signals[i].band == 0) {
                                Serial.print("  -> ");
                                Serial.print(signals[i].frequency, 2);
                                Serial.print(" MHz, RSSI: ");
                                Serial.print(signals[i].rssi, 1);
                                Serial.print(" dBm, Mod: ");
                                Serial.println(signals[i].modType);
                            }
                        }
                    }
                }
                break;
                
            case MENU_SCAN_2400:
                if (rfScanner.is2400MHzAvailable()) {
                    int detected = rfScanner.scan2400MHz();
                    if (detected > 0) {
                        Serial.print("[SCAN] 2.4GHz: ");
                        Serial.print(detected);
                        Serial.println(" signals detected");
                        
                        DetectedSignal* signals = rfScanner.getDetectedSignals();
                        for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
                            if (signals[i].active && signals[i].band == 1) {
                                Serial.print("  -> ");
                                Serial.print(signals[i].frequency, 2);
                                Serial.print(" MHz, RSSI: ");
                                Serial.print(signals[i].rssi, 1);
                                Serial.print(" dBm, Mod: ");
                                Serial.println(signals[i].modType);
                            }
                        }
                    }
                }
                break;
                
            default:
                // Not actively scanning in other menus
                break;
        }
    }
    
    // Update display
    displayUI.update(&rfScanner);
    
    // Small delay to prevent tight loop
    delay(10);
}
