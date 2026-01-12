#ifndef CLASH_OF_COLOSSEUM_GRAPH_H
#define CLASH_OF_COLOSSEUM_GRAPH_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

struct Serie {
    std::vector<float> data;
    SDL_Color color;
    std::string name;
};

class Graph {
public:
    Graph(int x, int y, int w, int h, SDL_Renderer* renderer);
    ~Graph();

    void addSerie(Serie serie);

    void setXLabel(std::string label){ x_label = label;}
    void setYLabel(std::string label){ y_label = label;}
    void setWidth(int w){ graph_rect.w = w;}
    void setHeight(int h){ graph_rect.h = h;}

    void drawGraduations();
    void drawLegend();

    void clearSeries();

    void draw();
    void drawCurves();
    void drawAxes();
    void drawLabels();

    void setLabels(std::string x, std::string y);

private:


    SDL_Renderer* renderer = nullptr;
    SDL_Rect graph_rect;
    TTF_Font* label_font = nullptr;
    std::string x_label = "Temps";
    std::string y_label = "Valeur";
    std::vector<Serie> series;
    int padding = 40;
    int arrow_size = 10;

};


#endif //CLASH_OF_COLOSSEUM_GRAPH_H