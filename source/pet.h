#ifndef PET_H
#define PET_H

#include <PNGdec.h>
#include <TFT_eSPI.h>
#include "DisplayManager.h"
#include "InteractionManager.h"

#define MAX_IMAGE_WIDTH 240

class Pet {
public:
    enum State { HAPPY, SAD, HUNGRY, SLEEPY, DIRTY };

    Pet(DisplayManager& displayManager, InteractionManager& interactionManager);
    void update();
    void feed();
    void play();
    void goToSleep();
    void clean();
    void setState(State newState);
    State getState() const;
    void render();
    void moveLeft();
    void moveRight();

    // Indicadores
    int getHappiness() const;
    int getHunger() const;
    int getToilet() const;
    int getSleep() const;

private:
    DisplayManager& displayManager;
    InteractionManager& interactionManager; // Declarar como miembro
    State currentState;
    int16_t xpos, ypos;
    PNG png;
    const unsigned char* petImage;
    size_t petImageSize;

    // Indicadores
    static int happiness;
    static int hunger;
    static int toilet;
    static int sleep;

    void drawPet();
    void pngDraw(PNGDRAW *pDraw);

    // Variable estatica para almacenar el contexto
    static Pet* instance;
};

#endif