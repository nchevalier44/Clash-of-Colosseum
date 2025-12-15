#ifndef CLASH_OF_COLOSSEUM_CONFIG_H
#define CLASH_OF_COLOSSEUM_CONFIG_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <SDL_mixer.h>
#include <vector>

class Menu {
public:
    Menu(SDL_Renderer* r);
    ~Menu();

    void render();
    void handleEvent(SDL_Event& event);
    void configureParameters();

    // Getters
    int getNbGuerriers() const { return nbGuerriers; }
    int getMutationRate() const { return mutationRate; }
    bool getShowHealthBars() const {
        return showHealthBars;
    }
    bool getSameTypePeace() const { return sameTypePeace; }
    float getProjectileSpeedMultiplier() const; // On calculera le float basé sur l'index

private:
    SDL_Renderer* renderer;
    TTF_Font* font = nullptr;
    Mix_Music* menuMusic = nullptr;
    bool musiqueOn = true;
    SDL_Texture* background = nullptr;

    // Options du jeu
    int nbGuerriers = 10;
    int mutationRate = 15;      // 0 à 100
    bool showHealthBars = true; // Vrai/Faux
    bool sameTypePeace = false;
    int speedIndex = 1;         // 0: Lent, 1: Normal, 2: Rapide

    // Textes pour la vitesse
    std::vector<std::string> speedLabels = {"Lent", "Normal", "Rapide"};

    // Gestion du menu
    int optionsCount = 6;
    int selectedOption = 0;
};

#endif // CLASH_OF_COLOSSEUM_CONFIG_H