#include "Game.h"
#include <esp_task_wdt.h>

// Definiciones de colores
#define PLAYER_COLOR TFT_RED
#define OBJECT_COLOR TFT_BLACK
#define BACKGROUND_COLOR TFT_WHITE
#define HEART_COLOR TFT_RED
#define HEART_EMPTY_COLOR TFT_BLUE

// Tamaño de la cuadrícula
#define GRID_ROWS 12
#define GRID_COLS 12
#define CELL_SIZE 20
int Game::highScore = 0;  // Inicializo la variable estática

Game::Game(TFT_eSPI& tft) : tft(tft), playerPos(GRID_COLS / 2), score(0), lives(3) {
    initGameScreen();
    initControls();
}

void Game::initControls() {
    pinMode(15, INPUT_PULLUP); // Izquierda
    pinMode(19, INPUT_PULLUP); // Derecha
    pinMode(5, INPUT_PULLUP);  // Salir
}

void Game::initGameScreen() {
    tft.fillScreen(BACKGROUND_COLOR);
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            screen[i][j] = ' ';
        }
    }
}

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
    tft.fillScreen(BACKGROUND_COLOR);
    
    // Dibuja corazones (vidas)
    for (int i = 0; i < 3; i++) {
        tft.fillCircle(220 - i * 20, 15, 5, i < lives ? HEART_COLOR : HEART_EMPTY_COLOR);
    }

    // Dibuja la cuadrícula
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            if (i == GRID_ROWS - 1 && j == playerPos) {
                drawPlayer(j * CELL_SIZE, i * CELL_SIZE);
            } else if (screen[i][j] == '#') {
                tft.fillRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, OBJECT_COLOR);
            }
        }
    }

    // Puntuación
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print("Score: ");
    tft.print(score);
}

void Game::updateGame() {
    // Primero: Mueve todos los objetos hacia abajo
    for (int i = GRID_ROWS - 1; i > 0; i--) {  //Arranca desde la utlima fila 
        for (int j = 0; j < GRID_COLS; j++) {
            screen[i][j] = screen[i - 1][j];  // Copia de arriba hacia abajo
        }
    }

    // Segundo: Limpia la fila superior (después de mover)
    for (int j = 0; j < GRID_COLS; j++) {
        screen[0][j] = ' ';
    }

    // Tercero: Genera nuevos objetos (con probabilidad ajustada)
    int spawnChance = 10 + (min(5, score / 100) * 10);  // 10%-60% en pasos de 10
    if (random(100) < spawnChance) {
        int col = random(GRID_COLS);
        if (screen[0][col] == ' ') {  
            screen[0][col] = '#';
        }
    }

    // Verifica colisión con el jugador (en la última fila)
    if (screen[GRID_ROWS - 1][playerPos] == '#') {
        lives--;
        screen[GRID_ROWS - 1][playerPos] = ' ';  // Elimina el objeto
        
        if (lives <= 0) {
            if (score > highScore) highScore = score;
            showGameOver();
            score = 0;
            lives = 3;
            initGameScreen();
        }
    }
    score++;
}
void Game::showGameOver() {
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(40, 80);
    tft.print("GAME OVER");
    tft.setCursor(40, 120);
    tft.print("Score: ");
    tft.print(score);
    tft.setCursor(40, 150);
    tft.print("Record: ");
    tft.print(highScore);
    delay(3000);
}

void Game::movePlayer(char move) {
    static unsigned long lastMoveTime = 0;
    const unsigned long moveDelay = 200;
    
    if (millis() - lastMoveTime >= moveDelay) {
        if (digitalRead(move == 'a' ? 15 : 19) == LOW) {
            if (move == 'a' && playerPos > 0) playerPos--;
            else if (move == 'd' && playerPos < GRID_COLS - 1) playerPos++;
            lastMoveTime = millis();
        }
    }
}

void Game::playGame() {
    unsigned long lastUpdate = millis();
    const unsigned long gameUpdateInterval = 100;
    
    while (true) {
        // Controles
        movePlayer('a');
        movePlayer('d');
        
        // Salir al menú
        if (digitalRead(5) == LOW) {
            delay(130);
            if (digitalRead(5) == LOW) {
                tft.fillScreen(BACKGROUND_COLOR);
                break;
            }
        }

        // Actualizar juego
        if (millis() - lastUpdate >= gameUpdateInterval) {
            lastUpdate = millis();
            updateGame();
            drawGameScreen();
        }

        // Mantiene estable el ESP32
        esp_task_wdt_reset();
        vTaskDelay(1);
    }
}