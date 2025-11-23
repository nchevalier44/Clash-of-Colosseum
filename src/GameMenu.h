#ifndef CLASH_OF_COLOSSEUM_GAMEMENU_H
#define CLASH_OF_COLOSSEUM_GAMEMENU_H

#include <SDL2/SDL_ttf.h>
#include <vector>

class GameMenu {
public:
    GameMenu(SDL_Renderer* renderer, SDL_Window* window);
    ~GameMenu();
    void draw();
    void faster();
    void lower();
    int getTimeSpeed() const { return time_options[time_index]; }
private:
    void createBackground();
    void displayTimeSpeed();

    TTF_Font* font = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    int time_index = 0;
    std::vector<int> time_options;
    std::vector<SDL_Texture*> time_textures;

};


#endif //CLASH_OF_COLOSSEUM_GAMEMENU_H
