#ifndef CLASH_OF_COLOSSEUM_CONFIG_H
#define CLASH_OF_COLOSSEUM_CONFIG_H

#include <SDL.h>
#include <SDL_ttf.h>

class Menu {
public:
    Menu(SDL_Renderer* r);
    ~Menu();
    void render();
    void handleEvent(SDL_Event& event);  // Pour gérer le clavier
    void configureParameters();           // Lance le menu et récupère les valeurs

    int getNbGuerriers() const { return nbGuerriers; }
    int getPvBase() const { return pvBase; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    int nbGuerriers = 5;  // valeur par défaut
    int pvBase = 100;     // valeur par défaut

    int selectedOption = 0;  // 0 = nbGuerriers, 1 = pvBase
};

#endif
