#ifndef CLASH_OF_COLOSSEUM_CONFIG_H
#define CLASH_OF_COLOSSEUM_CONFIG_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <SDL_mixer.h>

class Menu {
public:
    Menu(SDL_Renderer* r);
    ~Menu();

    void render();
    void handleEvent(SDL_Event& event);
    void configureParameters();

    // Getters pour récupérer les valeurs choisies dans le menu
    int getNbGuerriers() const { return nbGuerriers; }
    std::string getTypeGuerriers() const { return typeGuerriers; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font = nullptr;
    Mix_Music* menuMusic = nullptr;
    bool musiqueOn = true; // si tu veux pouvoir la désactiver
    SDL_Texture* background = nullptr;

    int nbGuerriers = 10;       // valeur par défaut
    std::string typeGuerriers = "Guerrier";
    std::string difficulte = "Normal";
    int optionsCount = 6; // nb total d'options

    int selectedOption = 0;    // 0 = nbGuerriers, 1 = pvBase, 2 = dureeCombat
};

#endif // CLASH_OF_COLOSSEUM_CONFIG_H
