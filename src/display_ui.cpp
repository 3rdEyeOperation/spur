/**
 * @file display_ui.cpp
 * @brief Display UI implementation with Meshtastic-style menu system
 */

#include "display_ui.h"

DisplayUI::DisplayUI() {
    display = nullptr;
    currentState = MENU_MAIN;
    selectedItem = 0;
    lastButtonPress = 0;
}

bool DisplayUI::begin() {
    Serial.println("[UI] Initializing display...");
    
    // Initialize I2C for display
    Wire.begin(OLED_SDA, OLED_SCL);
    
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
    
    if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("[UI] SSD1306 allocation failed!");
        return false;
    }
    
    display->clearDisplay();
    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    
    Serial.println("[UI] Display initialized successfully");
    return true;
}

void DisplayUI::showSplash() {
    display->clearDisplay();
    
    // Draw border
    display->drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    
    // Title
    display->setTextSize(2);
    display->setCursor(25, 10);
    display->print("SPUR");
    
    // Subtitle
    display->setTextSize(1);
    display->setCursor(18, 30);
    display->print("RF Detector");
    
    // Version
    display->setCursor(35, 45);
    display->print("v");
    display->print(FIRMWARE_VERSION);
    
    display->display();
}

void DisplayUI::update(RFScanner* scanner) {
    display->clearDisplay();
    
    // Draw status bar at top
    drawStatusBar();
    
    // Draw content based on current state
    switch (currentState) {
        case MENU_MAIN:
            drawMainMenu();
            break;
        case MENU_SCAN_900:
            drawScan900(scanner);
            break;
        case MENU_SCAN_2400:
            drawScan2400(scanner);
            break;
        case MENU_DETECTED:
            drawDetected(scanner);
            break;
        case MENU_SETTINGS:
            drawSettings();
            break;
        case MENU_INFO:
            drawInfo();
            break;
    }
    
    display->display();
}

void DisplayUI::drawStatusBar() {
    // Draw line under status bar
    display->drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
    
    // Show current time/uptime
    display->setTextSize(1);
    display->setCursor(2, 1);
    
    unsigned long uptime = millis() / 1000;
    int hours = uptime / 3600;
    int mins = (uptime % 3600) / 60;
    int secs = uptime % 60;
    
    char timeStr[12];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", hours, mins, secs);
    display->print(timeStr);
    
    // Show mode indicator on right
    display->setCursor(90, 1);
    switch (currentState) {
        case MENU_MAIN:
            display->print("MENU");
            break;
        case MENU_SCAN_900:
            display->print("900M");
            break;
        case MENU_SCAN_2400:
            display->print("2.4G");
            break;
        case MENU_DETECTED:
            display->print("DET");
            break;
        case MENU_SETTINGS:
            display->print("SET");
            break;
        case MENU_INFO:
            display->print("INFO");
            break;
    }
}

void DisplayUI::drawMainMenu() {
    display->setTextSize(1);
    
    const char* menuItems[] = {
        "Scan 900MHz",
        "Scan 2.4GHz",
        "Detected",
        "Settings",
        "Info"
    };
    
    int numItems = 5;
    int startY = 14;
    int itemHeight = 10;
    
    for (int i = 0; i < numItems; i++) {
        drawMenuItem(startY + (i * itemHeight), menuItems[i], i == selectedItem);
    }
}

void DisplayUI::drawMenuItem(int y, const char* text, bool selected) {
    if (selected) {
        display->fillRect(0, y, SCREEN_WIDTH, 9, SSD1306_WHITE);
        display->setTextColor(SSD1306_BLACK);
    } else {
        display->setTextColor(SSD1306_WHITE);
    }
    
    display->setCursor(4, y + 1);
    if (selected) {
        display->print("> ");
    } else {
        display->print("  ");
    }
    display->print(text);
    
    display->setTextColor(SSD1306_WHITE);
}

void DisplayUI::drawScan900(RFScanner* scanner) {
    display->setTextSize(1);
    display->setCursor(2, 14);
    display->print("Scanning 900MHz Band");
    
    // Show current frequency
    display->setCursor(2, 26);
    display->print("Freq: ");
    display->print(scanner->getCurrentFrequency(), 1);
    display->print(" MHz");
    
    // Draw scan progress bar
    float progress = (scanner->getCurrentFrequency() - FREQ_900_START) / 
                     (FREQ_900_END - FREQ_900_START) * 100;
    drawProgressBar(2, 38, 124, 8, (int)progress);
    
    // Show detected count
    display->setCursor(2, 52);
    display->print("Detected: ");
    display->print(scanner->getSignalCount());
    display->print(" signals");
    
    // Indicate if radio available
    if (!scanner->is900MHzAvailable()) {
        display->setCursor(2, 56);
        display->print("Radio unavailable!");
    }
}

void DisplayUI::drawScan2400(RFScanner* scanner) {
    display->setTextSize(1);
    display->setCursor(2, 14);
    display->print("Scanning 2.4GHz Band");
    
    // Show current frequency
    display->setCursor(2, 26);
    display->print("Freq: ");
    display->print(scanner->getCurrentFrequency(), 1);
    display->print(" MHz");
    
    // Draw scan progress bar
    float progress = (scanner->getCurrentFrequency() - FREQ_2400_START) / 
                     (FREQ_2400_END - FREQ_2400_START) * 100;
    drawProgressBar(2, 38, 124, 8, (int)progress);
    
    // Show detected count
    display->setCursor(2, 52);
    display->print("Detected: ");
    display->print(scanner->getSignalCount());
    display->print(" signals");
    
    // Indicate if radio available
    if (!scanner->is2400MHzAvailable()) {
        display->setCursor(2, 56);
        display->print("2.4GHz not connected");
    }
}

void DisplayUI::drawDetected(RFScanner* scanner) {
    display->setTextSize(1);
    display->setCursor(2, 14);
    display->print("Detected Signals:");
    
    DetectedSignal* signals = scanner->getDetectedSignals();
    int count = 0;
    int y = 24;
    
    for (int i = 0; i < MAX_DETECTED_SIGNALS && count < 4; i++) {
        if (signals[i].active) {
            display->setCursor(2, y);
            
            // Frequency
            display->print(signals[i].frequency, 1);
            display->print("M ");
            
            // RSSI
            display->print((int)signals[i].rssi);
            display->print("dB ");
            
            // Modulation (abbreviated)
            const char* modStr = modTypeToString(signals[i].modType);
            display->print(modStr);
            
            // Draw signal bars
            drawSignalBars(115, y, signals[i].rssi);
            
            y += 10;
            count++;
        }
    }
    
    if (count == 0) {
        display->setCursor(20, 35);
        display->print("No signals detected");
    }
}

void DisplayUI::drawSettings() {
    display->setTextSize(1);
    display->setCursor(2, 14);
    display->print("Settings");
    
    display->setCursor(2, 28);
    display->print("RSSI Thresh: ");
    display->print(RSSI_THRESHOLD);
    display->print("dB");
    
    display->setCursor(2, 40);
    display->print("Scan Interval: ");
    display->print(SCAN_INTERVAL_MS);
    display->print("ms");
    
    display->setCursor(2, 52);
    display->print("Hold Time: ");
    display->print(SIGNAL_HOLD_TIME_MS / 1000);
    display->print("s");
}

void DisplayUI::drawInfo() {
    display->setTextSize(1);
    display->setCursor(2, 14);
    display->print(DEVICE_NAME);
    
    display->setCursor(2, 26);
    display->print("Version: ");
    display->print(FIRMWARE_VERSION);
    
    display->setCursor(2, 38);
    display->print("T-Beam S3 Core");
    
    display->setCursor(2, 50);
    display->print("RadioLib RF Scanner");
}

void DisplayUI::drawProgressBar(int x, int y, int width, int height, int progress) {
    // Draw border
    display->drawRect(x, y, width, height, SSD1306_WHITE);
    
    // Fill based on progress (0-100)
    int fillWidth = (width - 2) * progress / 100;
    if (fillWidth > 0) {
        display->fillRect(x + 1, y + 1, fillWidth, height - 2, SSD1306_WHITE);
    }
}

void DisplayUI::drawSignalBars(int x, int y, float rssi) {
    // Draw 4 signal bars based on RSSI
    // -120 to -100 = 1 bar, -100 to -80 = 2 bars, -80 to -60 = 3 bars, > -60 = 4 bars
    
    int bars = 1;
    if (rssi > -60) bars = 4;
    else if (rssi > -80) bars = 3;
    else if (rssi > -100) bars = 2;
    
    for (int i = 0; i < 4; i++) {
        int barHeight = 2 + (i * 2);
        if (i < bars) {
            display->fillRect(x + (i * 3), y + (6 - barHeight), 2, barHeight, SSD1306_WHITE);
        } else {
            display->drawRect(x + (i * 3), y + (6 - barHeight), 2, barHeight, SSD1306_WHITE);
        }
    }
}

const char* DisplayUI::modTypeToString(ModulationType mod) {
    switch (mod) {
        case MOD_FSK:   return "FSK";
        case MOD_GFSK:  return "GFSK";
        case MOD_LORA:  return "LoRa";
        case MOD_FHSS:  return "FHSS";
        case MOD_DSSS:  return "DSSS";
        case MOD_OFDM:  return "OFDM";
        default:        return "???";
    }
}

void DisplayUI::handleButton() {
    // Debounce
    if (millis() - lastButtonPress < 200) return;
    lastButtonPress = millis();
    
    if (currentState == MENU_MAIN) {
        selectMenu();
    } else {
        // Return to main menu
        currentState = MENU_MAIN;
    }
}

void DisplayUI::nextMenu() {
    if (currentState == MENU_MAIN) {
        selectedItem++;
        if (selectedItem > 4) selectedItem = 0;
    }
}

void DisplayUI::prevMenu() {
    if (currentState == MENU_MAIN) {
        selectedItem--;
        if (selectedItem < 0) selectedItem = 4;
    }
}

void DisplayUI::selectMenu() {
    switch (selectedItem) {
        case 0:
            currentState = MENU_SCAN_900;
            break;
        case 1:
            currentState = MENU_SCAN_2400;
            break;
        case 2:
            currentState = MENU_DETECTED;
            break;
        case 3:
            currentState = MENU_SETTINGS;
            break;
        case 4:
            currentState = MENU_INFO;
            break;
    }
}

MenuState DisplayUI::getMenuState() {
    return currentState;
}

void DisplayUI::setMenuState(MenuState state) {
    currentState = state;
}
