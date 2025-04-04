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
    void clearPet(int x, int y);
private:
    TFT_eSPI tft;
};

#endif