#include "InteractionManager.h"
#include "Game.h"
#include "Pet.h" // Incluir aquí para acceder a los métodos de Pet
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
            selectedMenuItem = (selectedMenuItem + 1) % 4; // Circular entre 0 y 3
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

    const char* menuItems[] = {"Alimentar", "Jugar", aux_dormir, "Limpiar"};
    displayManager.getTFT().setTextSize(2); // Tamaño de las letras

    for (int i = 0; i < 4; i++) {
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