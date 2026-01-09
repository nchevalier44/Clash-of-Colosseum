#include "./Button.h"

#include <iostream>

Button::Button(std::string text, TTF_Font* font, SDL_Renderer* renderer, std::function<void(Button*)> onClick) : font(font), text(text), onClick(onClick) {
    initRect();
}

Button::~Button() {}


SDL_Rect Button::getRect() const {
    return rect;
}

void Button::setHovering(bool hovering) {
    is_hovering = hovering;
}

void Button::setText(std::string text) {
    this->text = text;
    initRect();
}

void Button::initRect() {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
}

void Button::draw(SDL_Window* window, SDL_Renderer* renderer) {
    if (is_hovering){
        color = {255, 255, 0};
    } else {
        color = {255, 255, 255};
    }

    int width_window, height_window;
    SDL_GetWindowSize(window, &width_window, &height_window);
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);

    if (surface) {
        // Background
        int padding = 10;
        SDL_Rect rect_background = rect;
        rect_background.w += 2 * padding;
        rect_background.h += 2 * padding;
        rect_background.x -= padding;
        rect_background.y -= padding;
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 230);
        SDL_RenderFillRect(renderer, &rect_background);

        rect.w = surface->w;
        rect.h = surface->h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}