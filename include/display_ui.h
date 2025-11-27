/**
 * @file display_ui.h
 * @brief Display UI module with Meshtastic-style menu system
 * 
 * Provides a clean, intuitive UI for displaying detected drone signals
 */

#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "rf_scanner.h"

class DisplayUI {
public:
    DisplayUI();
    
    /**
     * @brief Initialize the display
     * @return true if initialization successful
     */
    bool begin();
    
    /**
     * @brief Update the display with current state
     * @param scanner Pointer to RF scanner for signal data
     */
    void update(RFScanner* scanner);
    
    /**
     * @brief Show splash screen
     */
    void showSplash();
    
    /**
     * @brief Handle button press for menu navigation
     */
    void handleButton();
    
    /**
     * @brief Get current menu state
     * @return Current MenuState
     */
    MenuState getMenuState();
    
    /**
     * @brief Set menu state
     * @param state New menu state
     */
    void setMenuState(MenuState state);
    
    /**
     * @brief Navigate to next menu item
     */
    void nextMenu();
    
    /**
     * @brief Navigate to previous menu item
     */
    void prevMenu();
    
    /**
     * @brief Select current menu item
     */
    void selectMenu();

private:
    Adafruit_SSD1306* display;
    MenuState currentState;
    int selectedItem;
    uint32_t lastButtonPress;
    
    /**
     * @brief Draw main menu screen
     */
    void drawMainMenu();
    
    /**
     * @brief Draw 900MHz scanning screen
     */
    void drawScan900(RFScanner* scanner);
    
    /**
     * @brief Draw 2.4GHz scanning screen
     */
    void drawScan2400(RFScanner* scanner);
    
    /**
     * @brief Draw detected signals list
     */
    void drawDetected(RFScanner* scanner);
    
    /**
     * @brief Draw settings screen
     */
    void drawSettings();
    
    /**
     * @brief Draw info/about screen
     */
    void drawInfo();
    
    /**
     * @brief Draw status bar at top
     */
    void drawStatusBar();
    
    /**
     * @brief Draw signal strength indicator
     */
    void drawSignalBars(int x, int y, float rssi);
    
    /**
     * @brief Convert modulation type to string
     */
    const char* modTypeToString(ModulationType mod);
    
    /**
     * @brief Draw a progress bar
     */
    void drawProgressBar(int x, int y, int width, int height, int progress);
    
    /**
     * @brief Draw menu item with selection indicator
     */
    void drawMenuItem(int y, const char* text, bool selected);
};

#endif // DISPLAY_UI_H
