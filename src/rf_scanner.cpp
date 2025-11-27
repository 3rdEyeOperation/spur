/**
 * @file rf_scanner.cpp
 * @brief RF Scanner implementation for drone frequency detection
 */

#include "rf_scanner.h"
#include <cmath>

// Create module instances for RadioLib
#if defined(LORA_CS) && defined(LORA_IRQ) && defined(LORA_RST) && defined(LORA_BUSY)
static Module mod900(LORA_CS, LORA_IRQ, LORA_RST, LORA_BUSY);
#else
static Module mod900(10, 21, 17, 13);  // Default T-Beam S3 Core pins
#endif

#if defined(SX1280_CS) && defined(SX1280_IRQ) && defined(SX1280_RST) && defined(SX1280_BUSY)
static Module mod2400(SX1280_CS, SX1280_IRQ, SX1280_RST, SX1280_BUSY);
#else
static Module mod2400(5, 39, 14, 15);  // Default 2.4GHz module pins
#endif

RFScanner::RFScanner() {
    radio900 = nullptr;
    radio2400 = nullptr;
    signalCount = 0;
    currentFreq = 0;
    sx1262Available = false;
    sx1280Available = false;
    
    // Initialize signals array
    for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
        signals[i].active = false;
        signals[i].frequency = 0;
        signals[i].rssi = -120;
        signals[i].modType = MOD_UNKNOWN;
        signals[i].timestamp = 0;
        signals[i].band = 0;
    }
}

bool RFScanner::begin() {
    Serial.println("[RF] Initializing RF Scanner...");
    
    // Initialize SX1262 for 900MHz band
    radio900 = new SX1262(&mod900);
    
    Serial.print("[RF] Initializing SX1262 (900MHz)... ");
    int state = radio900->begin(915.0, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0, false);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("Success!");
        sx1262Available = true;
        
        // Set to standby mode for scanning
        radio900->standby();
    } else {
        Serial.print("Failed, code ");
        Serial.println(state);
        sx1262Available = false;
    }
    
    // Initialize SX1280 for 2.4GHz band (if available)
    radio2400 = new SX1280(&mod2400);
    
    Serial.print("[RF] Initializing SX1280 (2.4GHz)... ");
    state = radio2400->begin(2450.0, 1600.0, 7, 9, 0x12, 13);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("Success!");
        sx1280Available = true;
        radio2400->standby();
    } else {
        Serial.print("Not available, code ");
        Serial.println(state);
        sx1280Available = false;
    }
    
    return sx1262Available || sx1280Available;
}

int RFScanner::scan900MHz() {
    if (!sx1262Available) return 0;
    
    int detected = 0;
    
    // Scan across the 900MHz band
    for (float freq = FREQ_900_START; freq <= FREQ_900_END; freq += FREQ_900_STEP) {
        currentFreq = freq;
        
        float rssi = measureRSSI(freq, 0);
        
        if (rssi > RSSI_THRESHOLD) {
            ModulationType mod = analyzeModulation(freq, 0);
            addSignal(freq, rssi, mod, 0);
            detected++;
        }
    }
    
    cleanupSignals();
    return detected;
}

int RFScanner::scan2400MHz() {
    if (!sx1280Available) return 0;
    
    int detected = 0;
    
    // Scan across the 2.4GHz band
    for (float freq = FREQ_2400_START; freq <= FREQ_2400_END; freq += FREQ_2400_STEP) {
        currentFreq = freq;
        
        float rssi = measureRSSI(freq, 1);
        
        if (rssi > RSSI_THRESHOLD) {
            ModulationType mod = analyzeModulation(freq, 1);
            addSignal(freq, rssi, mod, 1);
            detected++;
        }
    }
    
    cleanupSignals();
    return detected;
}

float RFScanner::measureRSSI(float freq, uint8_t band) {
    float rssi = -120.0;  // Default very low
    
    if (band == 0 && sx1262Available) {
        // Set frequency on SX1262
        int state = radio900->setFrequency(freq);
        if (state == RADIOLIB_ERR_NONE) {
            // Put radio in receive mode briefly to measure RSSI
            // Note: Minimal delay needed for RSSI settling; consider async 
            // scanning for higher performance in future versions
            radio900->startReceive();
            delay(2);  // Minimal settling time
            rssi = radio900->getRSSI();
            radio900->standby();
        }
    } else if (band == 1 && sx1280Available) {
        // Set frequency on SX1280
        int state = radio2400->setFrequency(freq);
        if (state == RADIOLIB_ERR_NONE) {
            radio2400->startReceive();
            delay(2);  // Minimal settling time
            rssi = radio2400->getRSSI();
            radio2400->standby();
        }
    }
    
    return rssi;
}

ModulationType RFScanner::analyzeModulation(float freq, uint8_t band) {
    // Modulation analysis based on signal characteristics
    // This is a simplified heuristic - real implementation would need
    // more sophisticated signal analysis
    
    ModulationType detectedMod = MOD_UNKNOWN;
    
    // Check for common drone frequency patterns
    if (band == 0) {  // 900MHz band
        // LoRa is common in this band for longer range drones
        if (freq >= 902.0 && freq <= 928.0) {
            // FCC 900MHz ISM band - often FHSS or LoRa
            // Check bandwidth characteristics
            if (sx1262Available) {
                // Sample multiple times to check for frequency hopping
                // Delay between samples to detect signal variance
                float rssi1 = measureRSSI(freq, 0);
                delay(5);  // Brief interval for FHSS detection
                float rssi2 = measureRSSI(freq, 0);
                
                if (fabs(rssi1 - rssi2) > 20) {
                    detectedMod = MOD_FHSS;  // Signal varies - likely FHSS
                } else if (rssi1 > -60) {
                    detectedMod = MOD_LORA;  // Strong steady signal
                } else {
                    detectedMod = MOD_GFSK;  // Moderate signal
                }
            }
        } else if (freq >= 868.0 && freq <= 870.0) {
            // EU 868MHz band - typically LoRa
            detectedMod = MOD_LORA;
        } else {
            detectedMod = MOD_FSK;
        }
    } else {  // 2.4GHz band
        // Most consumer drones use this band with FHSS or DSSS
        if (freq >= 2400.0 && freq <= 2483.0) {
            // Check signal characteristics
            float rssi1 = measureRSSI(freq, 1);
            delay(5);  // Brief interval for FHSS detection
            float rssi2 = measureRSSI(freq, 1);
            
            if (fabs(rssi1 - rssi2) > 15) {
                detectedMod = MOD_FHSS;  // Frequency hopping detected
            } else if (rssi1 > -50) {
                // Strong signal - could be DSSS or OFDM
                detectedMod = MOD_DSSS;
            } else {
                detectedMod = MOD_GFSK;
            }
        }
    }
    
    return detectedMod;
}

void RFScanner::addSignal(float freq, float rssi, ModulationType mod, uint8_t band) {
    // Check if signal already exists
    for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
        if (signals[i].active && 
            fabs(signals[i].frequency - freq) < 1.0 &&
            signals[i].band == band) {
            // Update existing signal
            signals[i].rssi = rssi;
            signals[i].modType = mod;
            signals[i].timestamp = millis();
            return;
        }
    }
    
    // Find empty slot for new signal
    for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
        if (!signals[i].active) {
            signals[i].frequency = freq;
            signals[i].rssi = rssi;
            signals[i].modType = mod;
            signals[i].band = band;
            signals[i].timestamp = millis();
            signals[i].active = true;
            signalCount++;
            return;
        }
    }
    
    // If no empty slot, replace oldest signal
    uint32_t oldestTime = millis();
    int oldestIdx = 0;
    for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
        if (signals[i].timestamp < oldestTime) {
            oldestTime = signals[i].timestamp;
            oldestIdx = i;
        }
    }
    
    signals[oldestIdx].frequency = freq;
    signals[oldestIdx].rssi = rssi;
    signals[oldestIdx].modType = mod;
    signals[oldestIdx].band = band;
    signals[oldestIdx].timestamp = millis();
    signals[oldestIdx].active = true;
}

void RFScanner::cleanupSignals() {
    uint32_t now = millis();
    signalCount = 0;
    
    for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
        if (signals[i].active) {
            if (now - signals[i].timestamp > SIGNAL_HOLD_TIME_MS) {
                signals[i].active = false;
            } else {
                signalCount++;
            }
        }
    }
}

DetectedSignal* RFScanner::getDetectedSignals() {
    return signals;
}

int RFScanner::getSignalCount() {
    return signalCount;
}

void RFScanner::clearSignals() {
    for (int i = 0; i < MAX_DETECTED_SIGNALS; i++) {
        signals[i].active = false;
    }
    signalCount = 0;
}

bool RFScanner::is900MHzAvailable() {
    return sx1262Available;
}

bool RFScanner::is2400MHzAvailable() {
    return sx1280Available;
}

float RFScanner::getCurrentFrequency() {
    return currentFreq;
}
