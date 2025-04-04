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
    void updateAppearance();

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
    const unsigned char* shitImage;
    size_t petImageSize;
    size_t shitImageSize;
    static int shit_posx;  // Variable estática para posición X
    static int shit_posy;  // Variable estática para posición Y
    // Indicadores
    static int happiness;
    static int hunger;
    static int toilet;
    static int sleep;
    void drawShit(int posx,int posy);
    void drawPet();
    void pngDraw(PNGDRAW *pDraw);
    void pngDrawxy(PNGDRAW *pDraw,int pos_x,int pos_y) ;

    // Variable estatica para almacenar el contexto
    static Pet* instance;
};

#endif