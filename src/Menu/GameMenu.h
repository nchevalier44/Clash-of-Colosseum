#ifndef CLASH_OF_COLOSSEUM_GAMEMENU_H
#define CLASH_OF_COLOSSEUM_GAMEMENU_H

#include <SDL2/SDL_ttf.h>
#include <vector>
#include "../Entities/Entity.h"
#include <iomanip>
#include <sstream>

#include "Button.h"

// Elle permet de convertir un float en string avec un nombre précis de décimales (n=2 par défaut).
// C'est indispensable pour afficher proprement "Vitesse: 1.25" au lieu de "1.250000".
inline std::string roundingFloatToString(float value, int n = 2) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(n) << value;
    return stream.str();
}

// Eviter l'import circulaire
class Graphics;

class GameMenu {
public:
    GameMenu(SDL_Renderer* renderer, SDL_Window* window);
    ~GameMenu();
    void draw(const std::vector<Entity*>& entities, int generation, bool is_game_paused);
    void faster();
    void lower();
    void setSelectedEntity(Entity* e) {selected_entity = e;}
    Entity* getSelectedEntity(){return selected_entity;}
    std::vector<Button*> getButtons(){ return buttons; }
    bool isSimulationStopped() const { return stop_simulation; }

    void displayEndSimulation(std::pair<std::string, int> pair, int generation);

    void setSimulationStopped(bool cond){ stop_simulation = cond; }

private:
    void createBackground();
    void displayTimeSpeed(bool is_game_paused);
    void drawStatsTable(const std::vector<Entity*>& entities, int generation);
    void drawEntityStats();

    Entity* selected_entity = nullptr;
    TTF_Font* font = nullptr;
    TTF_Font* statFont = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    int time_index = 0;
    std::vector<int> time_options; // Liste des vitesses disponibles (1, 2, 5, 10...)
    std::vector<SDL_Texture*> time_textures;
    std::vector<Button*> buttons;
    bool hide_stats = false;
    bool stop_simulation = false;

};


#endif //CLASH_OF_COLOSSEUM_GAMEMENU_H