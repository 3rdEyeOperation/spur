/**
 * @file rf_scanner.h
 * @brief RF Scanner module for drone frequency detection
 * 
 * Uses RadioLib to scan 900MHz (SX1262) and 2.4GHz (SX1280) bands
 */

#ifndef RF_SCANNER_H
#define RF_SCANNER_H

#include <Arduino.h>
#include <RadioLib.h>
#include "config.h"

class RFScanner {
public:
    RFScanner();
    
    /**
     * @brief Initialize the RF scanner hardware
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * @brief Scan 900MHz band for signals
     * @return Number of signals detected
     */
    int scan900MHz();
    
    /**
     * @brief Scan 2.4GHz band for signals
     * @return Number of signals detected
     */
    int scan2400MHz();
    
    /**
     * @brief Get detected signals array
     * @return Pointer to detected signals array
     */
    DetectedSignal* getDetectedSignals();
    
    /**
     * @brief Get number of currently detected signals
     * @return Number of active signals
     */
    int getSignalCount();
    
    /**
     * @brief Clear all detected signals
     */
    void clearSignals();
    
    /**
     * @brief Check if 900MHz radio is available
     * @return true if SX1262 is initialized
     */
    bool is900MHzAvailable();
    
    /**
     * @brief Check if 2.4GHz radio is available
     * @return true if SX1280 is initialized
     */
    bool is2400MHzAvailable();
    
    /**
     * @brief Get current scan frequency
     * @return Current frequency in MHz
     */
    float getCurrentFrequency();
    
    /**
     * @brief Analyze modulation type of detected signal
     * @param freq Frequency to analyze
     * @param band Band (0=900MHz, 1=2.4GHz)
     * @return Detected modulation type
     */
    ModulationType analyzeModulation(float freq, uint8_t band);

private:
    SX1262* radio900;           // 900MHz LoRa radio
    SX1280* radio2400;          // 2.4GHz radio (optional)
    
    DetectedSignal signals[MAX_DETECTED_SIGNALS];
    int signalCount;
    float currentFreq;
    
    bool sx1262Available;
    bool sx1280Available;
    
    /**
     * @brief Add or update a detected signal
     */
    void addSignal(float freq, float rssi, ModulationType mod, uint8_t band);
    
    /**
     * @brief Remove stale signals that are no longer active
     */
    void cleanupSignals();
    
    /**
     * @brief Measure RSSI at specific frequency
     */
    float measureRSSI(float freq, uint8_t band);
};

#endif // RF_SCANNER_H
