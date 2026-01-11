#ifndef CLASH_OF_COLOSSEUM_STATSVIEW_H
#define CLASH_OF_COLOSSEUM_STATSVIEW_H

#include <SDL.h>
#include <vector>
#include "SimulationStats.h"
#include "Menu/Graph.h"

class StatsView {
public:
    StatsView(SDL_Renderer* r, int screenW, int screenH);
    ~StatsView();

    void showStats(SimulationStats& stats);

private:
    SDL_Renderer* renderer = nullptr;
    int width, height;
    TTF_Font* titleFont = nullptr;

    std::vector<float> extractAttribute(SimulationStats& stats, std::string type, std::string attr);
    std::vector<float> extractPopulation(SimulationStats& stats, std::string className);
};


#endif //CLASH_OF_COLOSSEUM_STATSVIEW_H