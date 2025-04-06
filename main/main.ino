#include "Pet.h"
#include "game.h"
#include <WiFi.h>
#include "ota_config.h"
#include "ota_update.h"

DisplayManager displayManager;
Pet* pet = nullptr; // Declarar un puntero a Pet
InteractionManager* interactionManager = nullptr; // Declarar un puntero a InteractionManager

void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando setup...");
    
    //setup de OTA
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
     while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
     }
    Serial.println("\nWiFi conectado");
    check_and_update_firmware(FIRMWARE_VERSION);

    //Setup del Pet
    displayManager.init();
    Serial.println("DisplayManager inicializado");

    // Dibujar el fondo de la habitación
    displayManager.drawRoomBackground();
    Serial.println("Fondo de la habitación dibujado");

    // Crear la instancia de InteractionManager
    interactionManager = new InteractionManager(displayManager, *pet);
    Serial.println("InteractionManager creado");

    // Crear la instancia de Pet
    pet = new Pet(displayManager, *interactionManager);
    Serial.println("Pet creado");

    // Renderizar el pet
    pet->render();
    Serial.println("Pet renderizado");

    // Inicializar InteractionManager
    interactionManager->init();
    Serial.println("InteractionManager inicializado");
}

void loop() { 
    pet->update(); // Actualizar el estado del pet
    interactionManager->update(); // Actualizar la interacción con los botones
    delay(100); // Pequeño delay para evitar rebotes
}