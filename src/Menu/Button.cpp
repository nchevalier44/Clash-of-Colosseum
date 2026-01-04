#include "../Button.h"

#include <iostream>

Button::Button(std::string text, TTF_Font* font, int x, int y, SDL_Renderer* renderer, std::function<void(Button*)> onClick) : font(font), text(text), onClick(onClick) {
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
}

void Button::draw(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Color color = {255, 255, 255};
    if (is_hovering) color = {255, 255, 0};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);

    int width_window, height_window;
    SDL_GetWindowSize(window, &width_window, &height_window);
    if (text == "Exit") {
        rect.x = 5*width_window/9;
    } else {
        rect.x = 4*width_window/9 - surface->w;
    }
    rect.y = 16;

    if (surface) {
        rect.w = surface->w;
        rect.h = surface->h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}