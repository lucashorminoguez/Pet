#include "InteractionManager.h"
#include "Game.h"
#include "Pet.h" // Incluir aquí para acceder a los métodos de Pet
#include <WiFi.h>
#include "ota_config.h"
bool dormir2 = false;

InteractionManager::InteractionManager(DisplayManager& displayManager, Pet& pet)
    : displayManager(displayManager), pet(pet), menuIsOpen(false), selectedMenuItem(0) {}

void InteractionManager::init() {
    pinMode(5, INPUT_PULLUP);   // Boton izquierdo (D5)
    pinMode(15, INPUT_PULLUP);  // Boton central (D15)
    pinMode(19, INPUT_PULLUP);  // Boton derecho (D19)
}


void InteractionManager::update() {
    bool buttonLeftPressed = digitalRead(5) == LOW;   // Boton izquierdo (D5)
    bool buttonMiddlePressed = digitalRead(15) == LOW; // Boton del medio (D15)
    bool buttonRightPressed = digitalRead(19) == LOW; // Boton derecho (D19)

    if (buttonLeftPressed) {
        if (menuIsOpen) {
            // Navegar a la siguiente opcion (circular)
            selectedMenuItem = (selectedMenuItem + 1) % 5; // Circular entre 0 y 4
            displayMenu();
        } else {
            openMenu();
        }
    }

    if (buttonMiddlePressed) {
        if (menuIsOpen) {
            // Ejecutar la acción seleccionada y salir del menu
            selectMenuItem();
            menuIsOpen = false;
            displayManager.drawRoomBackground(); // Borrar la pantalla
        } else {
            // Fuera del menu: Mover el pet a la izquierda
            pet.moveLeft();
        }
    }

    if (buttonRightPressed) {
        if (menuIsOpen) {
            // Salir del menu sin seleccionar nada
            menuIsOpen = false;
            displayManager.drawRoomBackground(); // Borrar la pantalla

        } else {
            // Fuera del menu: Mover el pet hacia la derecha
            pet.moveRight();
        }
    }
}

void InteractionManager::selectMenuItem() {
    Game game(displayManager.getTFT());  // Aquí asumimos que displayManager.getTFT() devuelve el objeto TFT
    switch (selectedMenuItem) {
        case 0:
            pet.feed(); // aumenta hambre
            break;
        case 1:
            game.playGame();  // Llama al juego
            pet.play(); // aumenta felicidad
            break;
        case 2:
            pet.goToSleep(); //aumenta sueño
            dormir2 = !dormir2;
            break;
        case 3:
            pet.clean(); // aumenta baño
            break;
        case 4:
            show_wifi_networks(); //actualiza redes disponibles
            break;
    }
}





void InteractionManager::openMenu() {
    menuIsOpen = true;
    selectedMenuItem = 0; // Reiniciar la selección al abrir el menú
    displayMenu();
}

void InteractionManager::displayMenu() {
    displayManager.clearScreen(TFT_PINK);
    char aux_dormir[20];
    if(dormir2 == true){
      strcpy(aux_dormir,"Dejar de dormir");
    } else {
      strcpy(aux_dormir,"Dormir");
    }

    const char* menuItems[] = {"Alimentar", "Jugar", aux_dormir, "Limpiar","Wifi"};
    displayManager.getTFT().setTextSize(2); // Tamaño de las letras

    for (int i = 0; i < 5; i++) {
        if (i == selectedMenuItem) {
            displayManager.getTFT().setTextColor(TFT_BLACK, TFT_YELLOW); // Texto negro sobre fondo amarillo
        } else {
            displayManager.getTFT().setTextColor(TFT_WHITE, TFT_PINK); // Texto blanco sobre fondo rosa
        }
        displayManager.getTFT().drawString(menuItems[i], 10, 20 + i * 30); // Posición (10, 20) con espaciado vertical
    }
}

void InteractionManager::goBack() {
    menuIsOpen = false;
    displayManager.clearScreen(TFT_BLACK); // Borrar la pantalla
    pet.render(); // Volver a mostrar la mascota
}



bool InteractionManager::isMenuOpen() const {
    return menuIsOpen;
}

void InteractionManager::showStatus() {
    displayManager.clearScreen(TFT_BLACK);

    const char* stateText;
    switch (pet.getState()) {
        case Pet::HAPPY:
            stateText = "Feliz";
            break;
        case Pet::SAD:
            stateText = "Triste";
            break;
        case Pet::HUNGRY:
            stateText = "Hambriento";
            break;
        case Pet::SLEEPY:
            stateText = "Soñoliento";
            break;
        case Pet::DIRTY:
            stateText = "Sucio";
            break;
    }

    displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);
    displayManager.getTFT().drawString("Estado: " + String(stateText), 10, 20);
}

void InteractionManager::show_wifi_networks() {
    displayManager.clearScreen(TFT_BLACK);
    displayManager.getTFT().setTextSize(2);
    displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);
    displayManager.getTFT().drawString("Escaneando WiFi...", 10, 10);

    WiFi.scanDelete(); // Borra resultados anteriores
    WiFi.scanNetworks(true); // Escaneo asincrónico

    unsigned long scanStart = millis();
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING && millis() - scanStart < 10000) {
        delay(100);
    }

    int numNetworks = WiFi.scanComplete(); // Obtenemos el resultado

    unsigned long start = millis();
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING && millis() - start < 10000) {
        delay(100);
    }
    numNetworks = WiFi.scanComplete(); // obtenemos resultado real del escaneo

    if (numNetworks == 0) {
        displayManager.getTFT().drawString("No se encontraron redes", 10, 40);
        delay(2000);
        return;
    }

    int selectedNetwork = 0;
    bool selecting = true;

    while (selecting) {
        displayManager.clearScreen(TFT_BLACK);
        for (int i = 0; i < numNetworks && i < 5; i++) {
            if (i == selectedNetwork) {
                displayManager.getTFT().setTextColor(TFT_BLACK, TFT_YELLOW);
            } else {
                displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);
            }
            displayManager.getTFT().drawString(WiFi.SSID(i), 10, 20 + i * 20);
        }

        bool left = digitalRead(5) == LOW;
        bool middle = digitalRead(15) == LOW;
        bool right = digitalRead(19) == LOW;

        if (left) {
            selectedNetwork = (selectedNetwork - 1 + numNetworks) % numNetworks;
            delay(200);
        }

        if (right) {
            selectedNetwork = (selectedNetwork + 1) % numNetworks;
            delay(200);
        }

        if (middle) {
            selecting = false;
        }
    }

    const char* ssid = WiFi.SSID(selectedNetwork).c_str();
    char password[64] = "";
    //inputPassword(password);
    strcpy(password, "micontraseña"); // TEMPORAL

    WiFi.begin(ssid, password);

    displayManager.clearScreen(TFT_BLACK);
    displayManager.getTFT().drawString("Conectando...", 10, 10);
    unsigned long start_wifi_verif = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start_wifi_verif < 10000) {
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
        displayManager.getTFT().drawString("Conectado!", 10, 40);
    //    check_and_update_firmware(FIRMWARE_VERSION);
    } else {
        displayManager.getTFT().drawString("Fallo conexion", 10, 40);
    }

    delay(2000);
}
