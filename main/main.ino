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
    
    // Intentar conectarse al WiFi (con timeout)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando a WiFi");
    unsigned long start_time_wifi = millis();
    const unsigned long wifi_timeout = 5000; // 5 segundos de intento

    while (WiFi.status() != WL_CONNECTED && millis() - start_time_wifi < wifi_timeout) {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi conectado");
        check_and_update_firmware(FIRMWARE_VERSION);
    } else {
        Serial.println("\nNo se pudo conectar al WiFi, iniciando sin conexión.");
    }

    //Setup del Pet
    displayManager.init();
    Serial.println("DisplayManager inicializado");

    // Dibujar el fondo de la habitacion
    displayManager.drawRoomBackground();
    Serial.println("Fondo de la habitación dibujado");

    // Crear instancia InteractionManager
    interactionManager = new InteractionManager(displayManager, *pet);
    Serial.println("InteractionManager creado");

    // Crear instancia Pet
    pet = new Pet(displayManager, *interactionManager);
    Serial.println("Pet creado");

    // Renderizar pet
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