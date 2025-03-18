#include "Pet.h"
#include "Mapache.h" // Incluye el archivo con el array de la imagen

// Inicializamos la variable estática
Pet* Pet::instance = nullptr;
int Pet::happiness = 100;
int Pet::hunger = 100;
int Pet::toilet = 100;
int Pet::sleep = 100;

Pet::Pet(DisplayManager& displayManager, InteractionManager& interactionManager) 
    : displayManager(displayManager), interactionManager(interactionManager), // Inicializar interactionManager
      currentState(HAPPY), xpos(0), ypos(0) {
    petImage = mapache; // mapache es el array de la imagen
    petImageSize = sizeof(mapache);
  if (instance == nullptr) {
    instance = this;
  }
}

void Pet::feed() {
    hunger = min(100, hunger + 30); // Aumentar el hambre
}

void Pet::play() {
    happiness = min(100, happiness + 20); // Aumentar la felicidad
}

void Pet::goToSleep() {  // Renombrado de sleep a goToSleep
    sleep = min(100, sleep + 30); // Aumentar el sueño
}

void Pet::clean() {
    toilet = min(100, toilet + 30); // Aumentar el baño
}

void Pet::setState(State newState) {
    currentState = newState;
    render();
}

Pet::State Pet::getState() const {
    return currentState;
}

void Pet::render() {
    //Solo dibujar la mascota si el menu no esta cerrado 
    if (!interactionManager.isMenuOpen()) {
        drawPet(); // Dibuja la mascota
        // Dibujar los indicadores
        displayManager.getTFT().setTextSize(1);
        displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);

        // Felicidad
        displayManager.getTFT().drawString("Felicidad: " + String(happiness) + "%", 10, 10);

        // Hambre
        displayManager.getTFT().drawString("Hambre: " + String(hunger) + "%", 10, 30);

        // Baño
        displayManager.getTFT().drawString("Baño: " + String(toilet) + "%", 10, 50);

        // Sueño
        displayManager.getTFT().drawString("Sueño: " + String(sleep) + "%", 10, 70);
    }
}


void Pet::drawPet() {
    uint16_t pngw = 0, pngh = 0;

    // Usamos la función de callback estática
    int16_t rc = png.openFLASH((uint8_t *)petImage, petImageSize, [](PNGDRAW *pDraw) {
        if (Pet::instance) {
            Pet::instance->pngDraw(pDraw); // Llamamos a la función de instancia
        }
    });

    if (rc == PNG_SUCCESS) {
        pngw = png.getWidth();
        pngh = png.getHeight();
        displayManager.getTFT().startWrite();
        png.decode(NULL, 0);
        displayManager.getTFT().endWrite();
    }
}

void Pet::pngDraw(PNGDRAW *pDraw) {
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];          // Line buffer for rendering
    uint8_t  maskBuffer[1 + MAX_IMAGE_WIDTH / 8];  // Mask buffer

    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

    if (png.getAlphaMask(pDraw, maskBuffer, 255)) {
        displayManager.getTFT().pushMaskedImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer, maskBuffer);
    }
}

void Pet::moveLeft() {
    xpos = max(0, xpos - 10); // Mover 10 píxeles a la izquierda (sin salir de la pantalla)
    render(); // Redibujar el pet en la nueva posición
}

void Pet::moveRight() {
    xpos = min(displayManager.getTFT().width() - png.getWidth(), xpos + 10); // Mover 10 píxeles a la derecha (sin salir de la pantalla)
    render(); // Redibujar el pet en la nueva posición
}


// Función auxiliar local para convertir el estado en una cadena legible
String getStateName(Pet::State state) {
    switch (state) {
        case Pet::HAPPY: return "FELIZ";
        case Pet::SAD: return "TRISTE";
        case Pet::HUNGRY: return "HAMBRIENTO";
        case Pet::SLEEPY: return "SOÑOLIENTO";
        case Pet::DIRTY: return "SUCIO";
        default: return "DESCONOCIDO";
    }
}

void Pet::update() {
    static int counter = 0; // Contador para reducir estados
    static unsigned long lastDebugTime = 0; // Último momento en que se enviaron mensajes de depuración
    counter++;

    // Reducir los indicadores solo cada 150 llamadas a update()
    if (counter >= 150) {
        counter = 0; // Reiniciar el contador

        // Reducir los indicadores con el tiempo
        happiness = max(0, happiness - 1); // La felicidad disminuye con el tiempo
        hunger = max(0, hunger - 1);       // El hambre aumenta con el tiempo
        toilet = max(0, toilet - 1);       // La necesidad de ir al baño aumenta
        sleep = max(0, sleep - 1);         // El sueño aumenta con el tiempo

        // Depuración: Mostrar los valores de los indicadores
        Serial.println("Contador reiniciado. Valores actuales:");
        Serial.println("  Felicidad: " + String(happiness));
        Serial.println("  Hambre: " + String(hunger));
        Serial.println("  Baño: " + String(toilet));
        Serial.println("  Sueño: " + String(sleep));
    }

    // Guardar el estado anterior
    State previousState = getState();

    // Cambiar el estado de la mascota según los indicadores
    if (hunger <= 20) {
        setState(HUNGRY);
    } else if (toilet <= 20) {
        setState(DIRTY);
    } else if (sleep <= 20) {
        setState(SLEEPY);
    } else if (happiness >= 80) {
        setState(HAPPY);
    } else {
        setState(SAD);
    }

    // Depuración: Mostrar el cambio de estado solo si es diferente al anterior
    if (getState() != previousState) {
        Serial.println("Estado cambiado a: " + getStateName(getState()));
    }

    // Renderizar el pet
    render();
}