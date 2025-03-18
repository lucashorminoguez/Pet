#include "DisplayManager.h"

DisplayManager::DisplayManager() {}

void DisplayManager::init() {
    tft.begin();
    tft.fillScreen(TFT_BLACK);
}

void DisplayManager::clearScreen(uint16_t color) {
    tft.fillScreen(color);
}

TFT_eSPI& DisplayManager::getTFT() {
    return tft;
}
void DisplayManager::drawRoomBackground() {
    // Definir colores
    uint16_t wallColor = tft.color565(245, 245, 220); // Beige (RGB: 245, 245, 220)
    uint16_t floorColor = tft.color565(139, 69, 19);  // Marron claro (RGB: 139, 69, 19)

    // Dibujar la pared (3/4 de la pantalla)
    tft.fillRect(0, 0, tft.width(), tft.height() * 3 / 4, wallColor);

    // Dibujar el piso (1/4 de la pantalla)
    tft.fillRect(0, tft.height() * 3 / 4, tft.width(), tft.height() / 4, floorColor);
}