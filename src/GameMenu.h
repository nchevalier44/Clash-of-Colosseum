#ifndef CLASH_OF_COLOSSEUM_GAMEMENU_H
#define CLASH_OF_COLOSSEUM_GAMEMENU_H

#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Entities/Entity.h"

class GameMenu {
public:
    GameMenu(SDL_Renderer* renderer, SDL_Window* window);
    ~GameMenu();
    void draw(const std::vector<Entity*>& entities, int generation);
    void faster();
    void lower();
    int getTimeSpeed() const { return time_options[time_index]; }
private:
    void createBackground();
    void displayTimeSpeed();
    void drawStatsTable(const std::vector<Entity*>& entities, int generation);

    TTF_Font* font = nullptr;
    TTF_Font* statFont = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    int time_index = 0;
    std::vector<int> time_options;
    std::vector<SDL_Texture*> time_textures;

};


#endif //CLASH_OF_COLOSSEUM_GAMEMENU_H
