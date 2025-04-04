#include "Game.h"
#include <esp_task_wdt.h>

// Definiciones de colores para la pantalla
#define PLAYER_COLOR TFT_RED
#define OBJECT_COLOR TFT_BLACK
#define BACKGROUND_COLOR TFT_WHITE

// Tamaño de la cuadrícula virtual
#define GRID_ROWS 12
#define GRID_COLS 12
#define CELL_SIZE 20  // Celda más pequeña para mantener tamaño de pantalla

Game::Game(TFT_eSPI& tft) : tft(tft), playerPos(GRID_COLS / 2), score(0) {
    initGameScreen();  // Inicializa la cuadrícula virtual
}

void Game::initGameScreen() {
    // Limpia la pantalla TFT y la matriz virtual
    tft.fillScreen(BACKGROUND_COLOR);
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            screen[i][j] = ' ';  // Vacío
        }
    }
}

// Función para dibujar el personaje con forma de animal
void Game::drawPlayer(int x, int y) {
    // Cuerpo
    tft.fillRect(x + 4, y + 4, CELL_SIZE - 8, CELL_SIZE - 4, PLAYER_COLOR);
    
    // Orejas
    tft.fillRect(x + 2, y + 2, 4, 4, PLAYER_COLOR);
    tft.fillRect(x + CELL_SIZE - 6, y + 2, 4, 4, PLAYER_COLOR);
    
    // Brazos
    tft.fillRect(x, y + 8, 4, 8, PLAYER_COLOR);
    tft.fillRect(x + CELL_SIZE - 4, y + 8, 4, 8, PLAYER_COLOR);
    
    // Ojos
    tft.fillRect(x + 8, y + 8, 4, 4, TFT_BLACK);
    tft.fillRect(x + CELL_SIZE - 12, y + 8, 4, 4, TFT_BLACK);
}



void Game::drawGameScreen() {
    tft.fillScreen(BACKGROUND_COLOR);  // Limpia la pantalla
    
    // Recorre la cuadrícula virtual y dibuja cada celda
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            // Dibuja el jugador en la última fila, siempre que no haya objeto en esa celda
            if (i == GRID_ROWS - 1 && j == playerPos) {
                drawPlayer(j * CELL_SIZE, i * CELL_SIZE);  // Dibuja el personaje
            }
            // Si en la celda hay un objeto, dibuja el objeto
            else if (screen[i][j] == '#') {
                tft.fillRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, OBJECT_COLOR);
            }
        }
    }

    // Muestra la puntuación
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print("Puntuacion: ");
    tft.println(score);
}




void Game::updateGame() {
    // Mueve los objetos hacia abajo en la cuadrícula virtual
    for (int i = GRID_ROWS - 2; i >= 0; i--) {
        for (int j = 0; j < GRID_COLS; j++) {
            screen[i + 1][j] = screen[i][j];
        }
    }
    // Limpia la primera fila
    for (int j = 0; j < GRID_COLS; j++) {
        screen[0][j] = ' ';
    }

    // Con una probabilidad (por ejemplo, 20%) crea un nuevo objeto en la parte superior
    if (random(0, 100) < 20) {
        int dropPos = random(0, GRID_COLS);
        screen[0][dropPos] = '#';
    }

    // Verifica si el jugador colisionó con un objeto
    // NOTA: Como el jugador se dibuja siempre en la última fila,
    // si en la matriz virtual hay '#' en esa posición, consideramos colisión.
    if (screen[GRID_ROWS - 1][playerPos] == '#') {
        tft.fillScreen(TFT_RED);  // Pantalla roja al perder
        tft.setCursor(60, 100);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.print("GAME OVER");
        delay(2000);  // Muestra el mensaje 2 segundos
        score = 0;   // Reinicia la puntuación
        initGameScreen();  // Reinicia la cuadrícula virtual
    }
}

void Game::movePlayer(char move) {
    static unsigned long lastMoveTime = 0;
    const unsigned long moveDelay = 200; // 200ms entre movimientos
    
    // Verifica si ha pasado el tiempo suficiente desde el último movimiento
    if(millis() - lastMoveTime >= moveDelay) {
        if(digitalRead(move == 'a' ? 15 : 19) == LOW) {
            // Mueve solo una celda
            if(move == 'a' && playerPos > 0) {
                playerPos--;
            }
            else if(move == 'd' && playerPos < GRID_COLS - 1) {
                playerPos++;
            }
            
            lastMoveTime = millis(); // Registra el momento del movimiento
        }
    }
}

void Game::playGame() {
    unsigned long lastUpdate = millis();
    const unsigned long gameUpdateInterval = 100; //actualiza cada 100 ms 
    
    while (true) {
        unsigned long currentMillis = millis();
        
        // Controles mejorados
        movePlayer('a'); // Para izquierda (pin 15)
        movePlayer('d'); // Para derecha (pin 19)

        // No permite salir hasta que no termine el juego:
        if (digitalRead(5) == LOW) {
            delay(130);
            if(digitalRead(5) == LOW) {
            // Limpia la pantalla y vuelve al menú, solo al finalizar el juego.
            tft.fillScreen(BACKGROUND_COLOR);
            break;
            }
        }

        // Actualiza y dibuja el juego cada 100 ms
        if (currentMillis - lastUpdate >= gameUpdateInterval) {
            lastUpdate = currentMillis;
            updateGame();
            drawGameScreen();
            score++;  // Incrementa la puntuación (opcional)
        }

        // Permite que otras tareas se ejecuten y resetea el watchdog
        esp_task_wdt_reset();
        vTaskDelay(1);  // Pequeño delay para ceder tiempo
    }
}
