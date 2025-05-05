#ifndef GAME_H
#define GAME_H

#include <TFT_eSPI.h>  // Asegúrate de incluir la librería TFT

class Game {
public:
    // Constructor: inicializa la pantalla TFT y la posición del jugador
    Game(TFT_eSPI& tft);  

    // Función para inicializar la pantalla virtual (matriz y limpieza de pantalla)
    void initGameScreen();  

    void drawPlayer(int x, int y);
    // Función para dibujar la pantalla con los objetos
    void drawGameScreen();  

    // Función que actualiza el estado del juego (movimiento de objetos, verificación de colisiones)
    void updateGame();  

    // Mueve al jugador a la izquierda o derecha
    void movePlayer(char move);  

    // Función principal del juego, donde se ejecuta el ciclo del juego
    void playGame();  
    void initControls();
    void showGameOver(); 
private:
    TFT_eSPI& tft;  // Referencia al objeto TFT_eSPI para dibujar en pantalla
    int playerPos;  // Posición del jugador en la pantalla (eje X)
    int score;  // Puntuación del jugador
    static int highScore; 
    int16_t lives;
    char screen[24][24];  // Matriz que simula la pantalla del juego (con objetos y jugador)
};

#endif
