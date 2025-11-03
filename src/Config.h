#ifndef CLASH_OF_COLOSSEUM_CONFIG_H
#define CLASH_OF_COLOSSEUM_CONFIG_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Menu {
public:
    Menu(SDL_Renderer* r);
    ~Menu();

    void render();
    void handleEvent(SDL_Event& event);
    void configureParameters();

    // Getters pour récupérer les valeurs choisies dans le menu
    int getNbGuerriers() const { return nbGuerriers; }
    int getPvBase() const { return pvBase; }
    std::string getTypeGuerriers() const { return typeGuerriers; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font = nullptr;
    SDL_Texture* background = nullptr;

    int nbGuerriers = 5;       // valeur par défaut
    int pvBase = 100;          // valeur par défaut
    std::string typeGuerriers = "Guerrier";
    std::string difficulte = "Normal";
    bool musiqueOn = true;
    int optionsCount = 6; // nb total d'options

    int selectedOption = 0;    // 0 = nbGuerriers, 1 = pvBase, 2 = dureeCombat
};

#endif // CLASH_OF_COLOSSEUM_CONFIG_H
