#ifndef INTERACTION_MANAGER_H
#define INTERACTION_MANAGER_H

#include "DisplayManager.h"
#include <string.h>
class Pet; // Declaración adelantada (en lugar de #include "Pet.h")

class InteractionManager {
public:
    InteractionManager(DisplayManager& displayManager, Pet& pet);
    bool isMenuOpen() const; // Nueva función para verificar si el menú está abierto
    void init();
    void update();


private:
    DisplayManager& displayManager;
    Pet& pet;
    bool menuIsOpen;
    int selectedMenuItem;
    void show_wifi_networks();
    void openMenu();
    void displayMenu();
    void goBack();
    void selectMenuItem();
    void showStatus();
    void inputPassword(char* password);
};
#endif
