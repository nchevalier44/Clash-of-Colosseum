#ifndef CLASH_OF_COLOSSEUM_BUTTON_H
#define CLASH_OF_COLOSSEUM_BUTTON_H
#include <functional>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>


class Button {
    public:
        Button(std::string text, TTF_Font* font, int x, int y, SDL_Renderer* renderer, std::function<void(Button*)> onClick);
        ~Button();

        SDL_Rect getRect() const;
        void draw(SDL_Window* window, SDL_Renderer *renderer);
        void setHovering(bool hover);
        void setText(std::string text);
        std::function<void(Button*)> onClick;


    private:
        SDL_Rect rect;
        std::string text;
        TTF_Font* font;
        bool is_hovering = false;
};

#endif //CLASH_OF_COLOSSEUM_BUTTON_H