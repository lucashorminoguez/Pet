#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <TFT_eSPI.h>

class DisplayManager {
public:
    DisplayManager();
    void init();
    void clearScreen(uint16_t color = TFT_BLACK);
    void drawRoomBackground();
    TFT_eSPI& getTFT();

private:
    TFT_eSPI tft;
};

#endif