#ifndef CLASH_OF_COLOSSEUM_HISTORYMENU_H
#define CLASH_OF_COLOSSEUM_HISTORYMENU_H

#include <ctime>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "../SimulationStats.h"
#include "Button.h"


class HistoryMenu {
    public:
        HistoryMenu(SDL_Window* window, SDL_Renderer* renderer, std::vector<SimulationStats*>* sim_stats, SDL_Texture* background, bool* keep_playing);

        int searchIdFromButton(Button *button);

        ~HistoryMenu();

        void handleEvent();
        void draw();

        std::string formatDate(std::time_t time);

    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        std::vector<SimulationStats*>* sim_stats = nullptr;
        TTF_Font* font = nullptr;
        std::vector<std::pair<std::string, Button*>> list_sim;
        SDL_Texture* background = nullptr;

        bool* keep_playing = nullptr;
        bool* running = nullptr;

};


#endif //CLASH_OF_COLOSSEUM_HISTORYMENU_H