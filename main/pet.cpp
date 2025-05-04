#include "Pet.h"
#include "Mapache.h" 
#include "shit.h" 
#include "remeras.h"

// Ini variables 
Pet* Pet::instance = nullptr;
int Pet::happiness = 100;
int Pet::hunger = 100;
int Pet::toilet = 100;
int Pet::sleep = 100;
int xpos_anterior = 0;
bool shit_done = false;
int Pet::shit_posx = 0;
int Pet::shit_posy = 0;
extern const uint8_t feliz_a[];
extern const uint8_t mapache[];
extern const uint8_t normal[];
extern const uint8_t comiendo_a[];


int alimentar = 0;
int stop_sleep = 0;
int dormir = 0;
bool first_open = true;

Pet::Pet(DisplayManager& displayManager, InteractionManager& interactionManager) 
    : displayManager(displayManager), interactionManager(interactionManager), // Inicializo interactionManager
      currentState(HAPPY), xpos(0), ypos(10) {
    petImage = feliz_a; // nombre del array de la imagen
    ropaImage = remera_arg;
    shitImage = shit;
    shitImageSize = sizeof(shit);
    petImageSize = sizeof(comiendo_a);
  if (instance == nullptr) {
    instance = this;
  }
}

void Pet::feed() {
    if (dormir == 0){
    hunger = min(100, hunger + 30); // Aumentar el hambre
    alimentar = 1;
    }
}

void Pet::play() {
    happiness = min(100, happiness + 20); // Aumentar la felicidad
}

void Pet::goToSleep() {  // Renombrado de sleep a goToSleep
    if (dormir>0){
      stop_sleep = 1;
      dormir = 1;
    } else {
    sleep = min(100, sleep + 30); // Aumentar el sueño
    dormir = 1;
    }
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
    if (!interactionManager.isMenuOpen()) {
    //Solo dibujar la mascota si el menu no esta cerrado 
        // Dibujar los indicadores
        displayManager.getTFT().setTextSize(1);
        displayManager.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);

        // Felicidad
        displayManager.getTFT().drawString("Felicidad: " + String(happiness) + "%", 10, 10);

        // Hambre
        displayManager.getTFT().drawString("Hambre: " + String(hunger) + "%", 10, 30);

        // Baño
        displayManager.getTFT().drawString("Higiene: " + String(toilet) + "%", 120, 30);

        // Sueño
        displayManager.getTFT().drawString("Sueño: " + String(sleep) + "%", 120, 10);
        if (first_open == true ){
            if (toilet < 90){
                drawShit(20,100);
                Serial.println("\nSe prendio por first_open");
                first_open = false ;
            }
            if (toilet<60){
              drawShit(160,100);
              first_open = false;
            }
            if(toilet<30){
                drawShit(97,100);
                first_open= false;
            }
        }
        if (xpos_anterior != xpos ){
          if(dormir>0){
            xpos = xpos_anterior;
          }else{
          displayManager.clearPet(xpos_anterior, ypos-10);
          xpos_anterior = instance -> xpos ;
            if(toilet<90){
                drawShit(20,100);  
                Serial.println("\nSe prendio por cambio de posicion");
            }
            if(toilet<60){
                drawShit(160,100);
            }
            if(toilet<30){
                drawShit(97,100);
            }
          }
        }
        drawPet(); // Dibuja la mascota
    }else {
        first_open = true;
    }
}

void Pet::drawShit(int posx, int posy) {
    // Asignamos las posiciones antes de llamar al callback
    shit_posx = posx;
    shit_posy = posy;
    
    int16_t rc = png.openFLASH((uint8_t *)shitImage, shitImageSize, [](PNGDRAW *pDraw) {
        if (instance) {
            instance->pngDrawxy(pDraw, shit_posx, shit_posy);
        }
    });
    
    if (rc == PNG_SUCCESS) {
        uint16_t pngw = png.getWidth();
        uint16_t pngh = png.getHeight();
        displayManager.getTFT().startWrite();
        png.decode(NULL, 0);
        displayManager.getTFT().endWrite();
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
    
    // Dibujo de la ropa 
    rc = png.openFLASH((uint8_t *)ropaImage, petImageSize, [](PNGDRAW *pDraw) {
        if (Pet::instance) {
            Pet::instance->pngDraw(pDraw);
        }
    });

    if (rc == PNG_SUCCESS) {
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
        displayManager.getTFT().pushMaskedImage(instance ->xpos, instance->ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer, maskBuffer);
    }
}

void Pet::pngDrawxy(PNGDRAW *pDraw,int pos_x,int pos_y) {
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];          // Line buffer for rendering
    uint8_t  maskBuffer[1 + MAX_IMAGE_WIDTH / 8];  // Mask buffer

    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

    if (png.getAlphaMask(pDraw, maskBuffer, 255)) {
        displayManager.getTFT().pushMaskedImage(pos_x, pos_y + pDraw->y, pDraw->iWidth, 1, lineBuffer, maskBuffer);
    }
}

void Pet::moveLeft() {
    Serial.println("moveLeft llamado");
    if (instance) {
        Serial.print("xpos antes: ");
        Serial.println(instance->xpos);
        instance->xpos = instance->xpos - 10;
        
        Serial.print("xpos después: ");
        Serial.println(instance->xpos);
        instance->render();
    } else {
        Serial.println("ERROR: instance es nullptr");
    }
}


void Pet::moveRight() {
    Serial.println("moveRight llamado");
    if (instance) {
        Serial.print("xpos antes: ");
        Serial.println(instance->xpos);
        
        instance->xpos += 10;  // Aumenta la posición en 10
        
        Serial.print("xpos después: ");
        Serial.println(instance->xpos);
        
        instance->render();
    } else {
        Serial.println("ERROR: instance es nullptr");
    }
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
    } else if (toilet <= 99) {
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
    updateAppearance();
    // Renderizar el pet
    instance ->render();
}


int cont_alimentar = 0;
void Pet::updateAppearance() {
    if(dormir>0){
      if (dormir == 1) {
          //delay(600);
          petImage = durmiendo_a;
          displayManager.clearPet(xpos_anterior,ypos-10);
          first_open = true ;
          dormir ++;
       if(stop_sleep == 1){
          stop_sleep=0;
          displayManager.clearPet(xpos_anterior,ypos-10);
          dormir = 0;
        }
      }else{
        //delay(600);
        //petImage = durmiendo_b;
       // displayManager.clearPet(xpos_anterior,ypos-10);
       // dormir =1;
      }
    }
    if(happiness<30 && dormir==0){
        petImage = enojado_a;
    }else if (happiness < 100 && dormir==0) {
        petImage = normal;
    } else if(happiness > 99 && dormir==0){
      petImage = feliz_a;
    }
    if (alimentar == 1) {
      cont_alimentar ++ ;
      if(cont_alimentar==1){
        displayManager.clearPet(xpos_anterior,ypos-10);
        first_open = true ;
        petImage = comiendo_a;
      }else if(cont_alimentar==2){
        delay(600);
        petImage = comiendo_b;
        displayManager.clearPet(xpos_anterior,ypos-10);
        first_open = true ;
      }else if (cont_alimentar ==3){
        delay(600);
        petImage= comiendo_a;
        displayManager.clearPet(xpos_anterior,ypos-10);
        first_open = true ;
      } else if (cont_alimentar == 4){
        delay(600);
        cont_alimentar=0;
        alimentar = 0;
      }
    }
}
