#include "InteractionManager.h"
#include "Game.h"
#include "Pet.h" // Incluir aquí para acceder a los métodos de Pet
#include <WiFi.h>
//#include "ota_config.h"
#include "ota_update.h"
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
            selectedMenuItem = (selectedMenuItem + 1) % 6; // Circular entre 0 y 5
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
            show_clothes();
            break;
        case 5:
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

    const char* menuItems[] = {"Alimentar", "Jugar", aux_dormir, "Limpiar","Cambiar Ropa","Wifi"};
    displayManager.getTFT().setTextSize(2); // Tamaño de las letras

    for (int i = 0; i < 6; i++) {
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

void InteractionManager::show_clothes(){
    delay(200); // Debounce
    int numClothes = 4; // Número total de prendas disponibles
    int selectedCloth = 0; // Índice de la prenda seleccionada
    bool redrawNeeded = true;
    int oldSelectedCloth = -1; // Para controlar cambios
    const char* menuItems[] = {"Scout", "Argentina", "ssj (demo)","nada"};
    while(true) {
        // Redibujar solo si hubo cambios
        if(redrawNeeded || selectedCloth != oldSelectedCloth) {
            displayManager.clearScreen(TFT_PINK);
            displayManager.getTFT().setTextSize(2);
            
            for(int i = 0; i < numClothes; i++) {
                if(i == selectedCloth) {
                    displayManager.getTFT().setTextColor(TFT_BLACK, TFT_YELLOW);
                } else {
                    displayManager.getTFT().setTextColor(TFT_WHITE, TFT_PINK);
                }
                displayManager.getTFT().drawString(menuItems[i], 10, 20 + i * 30);
            }
            
            oldSelectedCloth = selectedCloth;
            redrawNeeded = false;
        }
        delay(200);
        // Manejo de botones
        if(digitalRead(5) == LOW) { // medio - siguiente prenda
            selectedCloth = (selectedCloth + 1) % numClothes;
            delay(150); // Debounce
        }
        
        if(digitalRead(19) == LOW) { // derecha - volver
            return;

        }
        
        if(digitalRead(15) == LOW) { // izq - confirmar 
            pet.updateClothe(selectedCloth);
            return; // Salir del menú de ropa
        }
        
        delay(50); // Pequeña pausa para no saturar
    }
}
void InteractionManager::show_wifi_networks() {
    // ------------------------------------------------------------
    // 1. CONFIGURACIÓN INICIAL
    // ------------------------------------------------------------
    // Limpiamos la pantalla y mostramos mensaje inicial
    displayManager.clearScreen(TFT_BLACK);
    displayManager.getTFT().setTextSize(2);
    displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);
    displayManager.getTFT().drawString("Escaneando WiFi...", 10, 10);
    // --- RESET COMPLETO DEL WIFI --- (¡AGREGA ESTO!)
    WiFi.disconnect(true);  // Fuerza desconexión (true = apaga WiFi)
    delay(1000);            // Espera 1 segundo
    WiFi.mode(WIFI_OFF);    // Apaga completamente el WiFi

    // ------------------------------------------------------------
    // 2. ESCANEO DE REDES WIFI
    // ------------------------------------------------------------
    // Asegurar que el WiFi esté en modo estación (STA)
    WiFi.mode(WIFI_STA);
    // Elimino resultados de escaneos anteriores
    WiFi.scanDelete();
    // Inicia un nuevo escaneo (true = asincrono, false = mostrar redes ocultas)
    int scanResult = WiFi.scanNetworks(true, false);
    
    // Esperamos hasta que termine el escaneo (máximo 15 segundos)
    unsigned long scanStartTime = millis();
    while(WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
        // Si pasa el tiempo máximo, salimos del bucle
        if(millis() - scanStartTime > 15000) {
            break;
        }
        delay(200);
        
        // puntos animados 
        int dots = (millis()/500) % 4;
        displayManager.getTFT().fillRect(200, 10, 30, 20, TFT_BLACK);
        for(int i=0; i<dots; i++) {
            displayManager.getTFT().drawString(".", 200+ i*10, 10);
        }
    }

    // Obtengo el numero de redes encontradas
    int numNetworks = WiFi.scanComplete();

    // ------------------------------------------------------------
    // 3. MANEJO DE REDES ENCONTRADAS
    // ------------------------------------------------------------
    if(numNetworks <= 0) {
        // Si no encuentro redes, muestro mensaje y opción para reintentar
        displayManager.clearScreen(TFT_BLACK);
        displayManager.getTFT().drawString("No hay redes WiFi", 10, 30);
        displayManager.getTFT().drawString("Central: Reintentar", 10, 60);
        displayManager.getTFT().drawString("Izquierdo: Salir", 10, 90);
        
        // Esperamos la decisión del usuario
        while(true) {
            if(digitalRead(15) == LOW) { // Botón central - reintentar
                show_wifi_networks(); // Llamada recursiva
                return;
            }
            if(digitalRead(19) == LOW) { // Botón izquierdo - salir
                return;
            }
            delay(100);
        }
    }

    // ------------------------------------------------------------
    // 4. SELECCIÓN DE RED WIFI
    // ------------------------------------------------------------
    int selectedNetwork = 0; // Empezamos seleccionando la primera red
    bool redrawNetwork = true;
    int old_selectedNetwork = 1;
    while(true) { // Bucle infinito hasta que se seleccione una red
        if (selectedNetwork != old_selectedNetwork){
            redrawNetwork=true;
            old_selectedNetwork = selectedNetwork;
        }

        if(redrawNetwork){
        redrawNetwork = false;
        displayManager.clearScreen(TFT_BLACK);
        displayManager.getTFT().drawString("Seleccione red:", 10, 10);
        // Muestro todas las redes encontradas (máximo 5)
        for(int i = 0; i < numNetworks && i < 5; i++) {
            // Si esta red está seleccionada, muestro con colores invertidos
            if(i == selectedNetwork) {
                displayManager.getTFT().setTextColor(TFT_BLACK, TFT_YELLOW); // Texto negro, fondo amarillo
            } else {
                displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK); // Texto blanco, fondo negro
            }
            
            // Muestro el nombre de la red (SSID)
            displayManager.getTFT().drawString(WiFi.SSID(i), 10, 40 + i * 25);
        }
        }

        // --------------------------------------------------------
        // 4.1. MANEJO DE BOTONES PARA SELECCION
        // --------------------------------------------------------
        // Boton izquierdo - mover seleccion hacia arriba
        if(digitalRead(19) == LOW) {
            selectedNetwork--;
            if(selectedNetwork < 0) {
                selectedNetwork = numNetworks - 1; // Volvemos al final si pasamos el inicio
            }
            delay(250); // Pequeña pausa para evitar rebotes
        }
        
        // Boton derecho - mover seleccion hacia abajo
        if(digitalRead(5) == LOW) {
            selectedNetwork++;
            if(selectedNetwork >= numNetworks) {
                selectedNetwork = 0; // Volvemos al inicio si pasamos el final
            }
            delay(250);
        }
        
        // Boton central - confirmar seleccion
        if(digitalRead(15) == LOW) {
            break; // Salimos del bucle de seleccion
        }
        
        delay(50); // pausa para no saturar el procesador
    }

    // ------------------------------------------------------------
    // 5. CONEXIÓN A LA RED SELECCIONADA
    // ------------------------------------------------------------
    String ssid = WiFi.SSID(selectedNetwork); // Obtenemos el nombre de la red seleccionada
    char password[64] = "micontraseña"; // Contraseña temporal (debería implementar inputPassword())
    inputPassword(password);
    // Mostramos mensaje de conexion
    displayManager.clearScreen(TFT_BLACK);
    displayManager.getTFT().drawString("Conectando a:", 10, 10);
    displayManager.getTFT().drawString(ssid, 10, 40);
    
    // Intentamos conectar
    WiFi.begin(ssid.c_str(), password);
    
    // Esperamos hasta 20 segundos para la conexion
    unsigned long connectionStartTime = millis();
    bool connected = false;
    
    while(!connected && (millis() - connectionStartTime < 20000)) {
        if(WiFi.status() == WL_CONNECTED) {
            connected = true;
            break;
        }
        
        delay(500);
        
        // Mostrar puntos animados (...) para indicar que sigue intentando
        int dots = (millis()/500) % 4;
        displayManager.getTFT().fillRect(200, 40, 30, 20, TFT_BLACK);
        for(int i=0; i<dots; i++) {
            displayManager.getTFT().drawString(".", 200 + i*10, 40);
        }
    }

    // ------------------------------------------------------------
    // 6. RESULTADO DE LA CONEXION
    // ------------------------------------------------------------
    if(connected) {
        // Conexion exitosa
        displayManager.getTFT().drawString("Conectado!", 10, 70);
        displayManager.getTFT().drawString("IP: " + WiFi.localIP().toString(), 10, 100);
        // Aca iria la actualización de firmware 
        check_and_update_firmware(FIRMWARE_VERSION);
        
        delay(3000); // Mostrar mensaje por 3 segundos
    } else {
        // Fallo en la conexion
        displayManager.getTFT().drawString("Fallo conexion", 10, 70);
        displayManager.getTFT().drawString("Central: Reintentar", 10, 100);
        displayManager.getTFT().drawString("Izquierdo: Salir", 10, 130);
        
        // Esperamos la decisión del usuario
        while(true) {
            if(digitalRead(15) == LOW) { // Boton central - reintentar
                show_wifi_networks(); // Llamada recursiva
                return;
            }
            if(digitalRead(19) == LOW) { // Botón izquierdo - salir
                return;
            }
            delay(100);
        }
    }
}


void InteractionManager::inputPassword(char* password) {
    const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
    const int charsCount = strlen(chars);
    int selectedChar = 0;
    int passwordLength = 0;
    bool editing = true;
    const int maxLength = 63;
    
    // Variables para control de redibujado
    int lastSelectedChar = -1;
    int lastPasswordLength = -1;
    bool needsRedraw = true;
    
    memset(password, 0, 64);

    while(editing) {
        // Verificar si necesitamos redibujar
        if (selectedChar != lastSelectedChar || passwordLength != lastPasswordLength) {
            needsRedraw = true;
            lastSelectedChar = selectedChar;
            lastPasswordLength = passwordLength;
        }

        if (needsRedraw) {
            displayManager.clearScreen(TFT_BLACK);
            
            // 1. Mostrar carácter actual seleccionado (Tamaño 3)
            displayManager.getTFT().setTextSize(3);
            displayManager.getTFT().setTextColor(TFT_YELLOW, TFT_BLACK);
            displayManager.getTFT().drawString(String(chars[selectedChar]), 50, 30);
            
            // 2. Mostrar contraseña (Tamaño 2)
            displayManager.getTFT().setTextSize(2);
            String hiddenPass;
            for(int i = 0; i < passwordLength; i++) hiddenPass += "*";
            
            displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);
            displayManager.getTFT().drawString("Contraseña:", 10, 70);
            displayManager.getTFT().drawString(hiddenPass, 10, 100);
            
            // 3. Instrucciones 
            displayManager.getTFT().drawString("Cent: Anterior", 10, 130);  // Botón CENTRAL (5)
            displayManager.getTFT().drawString("Der: Siguiente", 10, 150);  // Botón DERECHO (19)
            displayManager.getTFT().drawString("Izq: Agregar", 10, 170);    // Botón IZQUIERDO (15)
            displayManager.getTFT().drawString("Izq+Der: Borrar", 10, 190);
            displayManager.getTFT().drawString("Izq+Cent: Terminar", 10, 210);

            needsRedraw = false;
        }
        
        bool centralBtn = digitalRead(5) == LOW;   // CENTRAL
        bool derechoBtn = digitalRead(19) == LOW;  // DERECHO
        bool izquierdoBtn = digitalRead(15) == LOW;// IZQUIERDO 
        
        if(centralBtn && !derechoBtn && !izquierdoBtn) {
            selectedChar = (selectedChar - 1 + charsCount) % charsCount;
            delay(150);
        }
        else if(derechoBtn && !centralBtn && !izquierdoBtn) {
            selectedChar = (selectedChar + 1) % charsCount;
            delay(150);
        }
        else if(izquierdoBtn) {
            unsigned long pressTime = millis();
            bool centralPressed = false;
            bool derechoPressed = false;
            
            while(digitalRead(15) == LOW) {
                if(digitalRead(5) == LOW) centralPressed = true;
                if(digitalRead(19) == LOW) derechoPressed = true;
                delay(50);
            }
            
            if(derechoPressed) { // Combinacion Izq+Der: Borrar
                if(passwordLength > 0) {
                    password[--passwordLength] = '\0';
                    needsRedraw = true;
                }
            }
            else if(centralPressed) { // Combinacion Izq+Cent: Terminar
                editing = false;
            }
            else { // Solo Izquierdo: Agregar caracter
                if(passwordLength < maxLength) {
                    password[passwordLength++] = chars[selectedChar];
                    needsRedraw = true;
                }
            }
            delay(200);
        }
        
        delay(30);
    }
}