#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "GameMenu.h"

GameMenu::GameMenu(SDL_Renderer* renderer, SDL_Window* window) : window(window), renderer(renderer){
    font = TTF_OpenFont("../assets/arial.ttf", 16);
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
}

void GameMenu::draw(){
    createBackground();
    displayTimeSpeed();
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