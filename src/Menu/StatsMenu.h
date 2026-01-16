#ifndef CLASH_OF_COLOSSEUM_STATSVIEW_H
#define CLASH_OF_COLOSSEUM_STATSVIEW_H

#include <SDL.h>
#include <vector>
#include <string>
#include <ctime>
#include "../SimulationStats.h"
#include "Graph.h"
#include "Button.h"

class StatsMenu {
public:
    StatsMenu(SDL_Renderer* r, SDL_Window* window, SDL_Texture* background, bool* keep_playing);
    ~StatsMenu();

    void showStats(SimulationStats& stats);

    void draw(SimulationStats &stats);

private:
    SDL_Renderer* renderer = nullptr;

    // Polices
    TTF_Font* titleFont = nullptr;
    TTF_Font* infoFont = nullptr;
    TTF_Font* buttonFont = nullptr;

    SDL_Window* window = nullptr;

    // Interface
    std::vector<Button*> buttons;
    Graph* currentGraph = nullptr;
    std::string currentMode = "";
    SDL_Texture * background = nullptr;

    // Méthodes internes
    void initButtons(SimulationStats& stats);

    void makeBtn(SimulationStats& stats, ::string label, std::string modeKey, int x, int y);

    void updateGraphData(SimulationStats& stats, std::string mode);
    void drawInfoPanel(SimulationStats& stats);

    void drawText(std::string text, int x, int y, int lineHeight);

    std::string formatDateTime(std::time_t time);
    std::vector<float> extractAttribute(SimulationStats& stats, std::string category, std::string field);

    bool* keep_playing = nullptr;
    bool* running = nullptr;
};

#endif //CLASH_OF_COLOSSEUM_STATSVIEW_H