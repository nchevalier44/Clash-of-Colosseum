#ifndef CLASH_OF_COLOSSEUM_CONFIG_H
#define CLASH_OF_COLOSSEUM_CONFIG_H

#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <SDL_mixer.h>
#include <vector>

class SettingsMenu {
public:
    SettingsMenu(SDL_Renderer* r, SDL_Window* window, std::map<std::string, std::string>* parameters, bool* keep_playing);
    ~SettingsMenu();

    void render(SDL_Texture* background);
    void handleEvent(SDL_Event& event, Mix_Music* music);
    void configureParameters(Mix_Music* music, SDL_Texture* background);

    // Getters
    int getNbGuerriers() const { return nbGuerriers; }
    int getMutationTypeRate() const { return mutationTypeRate; }
    int getMutationStatsRate() const { return mutationStatsRate; }
    bool getShowHealthBars() const { return showHealthBars; }
    bool getMusicOn() const { return musiqueOn; }
    bool getSameTypePeace() const { return sameTypePeace; }
    float getProjectileSpeedMultiplier() const; // On calculera le float basé sur l'index
    int getSpeedIndex() const { return speedIndex; }

    int getMinNumberEntity() const { return min_number_entity; };

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    bool musiqueOn = true;
    SDL_Texture* background = nullptr;

    // Options du jeu
    int nbGuerriers = 10;
    int mutationTypeRate = 15;      // 0 à 100
    int mutationStatsRate = 15;
    bool showHealthBars = true; // Vrai/Faux
    bool sameTypePeace = false;
    int speedIndex = 1;         // 0: Lent, 1: Normal, 2: Rapide
    int min_number_entity = 5;

    // Textes pour la vitesse
    std::vector<std::string> speedLabels = {"Lent", "Normal", "Rapide"};

    // Gestion du menu
    int optionsCount = 7;
    int selectedOption = 0;
    bool* keep_playing = nullptr;
    bool * running = nullptr;
};

#endif // CLASH_OF_COLOSSEUM_CONFIG_H