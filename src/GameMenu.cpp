#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "GameMenu.h"

GameMenu::GameMenu(SDL_Renderer* renderer, SDL_Window* window) : window(window), renderer(renderer){
    font = TTF_OpenFont("../assets/arial.ttf", 16);
    statFont = TTF_OpenFont("../assets/arial.ttf", 14);
    if (!statFont) {
        std::cerr << "Erreur chargement police stats: " << TTF_GetError() << std::endl;
    }
    time_options = {1, 2, 5, 10, 20, 50, 100};
    for(int value : time_options){
        std::string string_time = "x" + std::to_string(value);

        SDL_Surface* surface = TTF_RenderText_Solid(font, string_time.c_str(), {255, 255, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        time_textures.push_back(texture);
        SDL_FreeSurface(surface);
    }
}

GameMenu::~GameMenu(){
    for(SDL_Texture* texture : time_textures){
        SDL_DestroyTexture(texture);
    }
    if (statFont) TTF_CloseFont(statFont);
}

void GameMenu::draw(const std::vector<Entity*>& entities, int generation){
    createBackground();
    displayTimeSpeed();
    drawStatsTable(entities, generation);
}

void GameMenu::drawStatsTable(const std::vector<Entity*>& entities, int generation) {
    if (!statFont) return;

    int nbGuerrier = 0, nbArcher = 0, nbMage = 0, nbGolem = 0;
    float totalHp = 0;

    for (const auto& e : entities) {
        std::string t = e->getType();
        if (t == "Guerrier") nbGuerrier++;
        else if (t == "Archer") nbArcher++;
        else if (t == "Mage") nbMage++;
        else if (t == "Tank") nbGolem++;
        totalHp += e->getHp();
    }
    float avgHp = entities.empty() ? 0 : totalHp / entities.size();

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    int rectW = 170;
    int rectH = 160;
    int margin = 10;

    SDL_Rect bgRect = {winW - rectW - margin, winH - rectH - margin, rectW, rectH};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 254);
    SDL_RenderFillRect(renderer, &bgRect);

    std::vector<std::string> lines;
    lines.push_back("Generation: " + std::to_string(generation));
    lines.push_back("Vivants: " + std::to_string(entities.size()));
    lines.push_back("PV Moyens: " + std::to_string((int)avgHp));
    lines.push_back("----------------");
    lines.push_back("Guerriers: " + std::to_string(nbGuerrier));
    lines.push_back("Archers:   " + std::to_string(nbArcher));
    lines.push_back("Mages:     " + std::to_string(nbMage));
    lines.push_back("Golems:    " + std::to_string(nbGolem));

    SDL_Color textColor = {255, 255, 255, 255};

    int xOffset = bgRect.x + 10;
    int yOffset = bgRect.y + 5;

    for (const auto& line : lines) {
        SDL_Surface* surf = TTF_RenderText_Blended(statFont, line.c_str(), textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dest = {xOffset, yOffset, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dest);
            yOffset += 18;
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}
void GameMenu::createBackground(){
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    SDL_Rect rect = {0, 0, width, 50};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 230);

    SDL_RenderFillRect(renderer, &rect);
}

void GameMenu::displayTimeSpeed(){
    SDL_Texture* texture = time_textures[time_index];

    int width_text, height_text, width_window, height_window;
    SDL_GetWindowSize(window, &width_window, &height_window);
    SDL_QueryTexture(texture, nullptr, nullptr, &width_text, &height_text);

    SDL_Rect text_rect = {width_window/2 - width_text/2, 25 - height_text/2, width_text, height_text};

    SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
}

void GameMenu::faster(){
    if(time_index < time_options.size()-1){
        time_index += 1;
    }
}

void GameMenu::lower(){
    if(time_index > 0){
        time_index -= 1;
    }
}