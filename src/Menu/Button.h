#ifndef CLASH_OF_COLOSSEUM_BUTTON_H
#define CLASH_OF_COLOSSEUM_BUTTON_H
#include <functional>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>


class Button {
public:
    // std::function<void(Button*)> : Permet de passer une fonction en paramètre.
    // C'est ce qui définit l'action du bouton (ex: "Quitter", "Lancer", "Pause").
    Button(std::string text, TTF_Font* font, SDL_Renderer* renderer, std::function<void(Button*)> onClick);
    ~Button();

    SDL_Rect getRect() const;
    void setX(int x){ rect.x = x; }
    void setY(int y){ rect.y = y; }
    void draw(SDL_Window* window, SDL_Renderer *renderer);
    void setHovering(bool hover);
    void setText(std::string text);
    std::function<void(Button*)> onClick;

private:
    // Calcule la largeur/hauteur du rectangle en fonction de la police et du texte
    void initRect();

    SDL_Rect rect;
    std::string text;
    TTF_Font* font;
    bool is_hovering = false;
    SDL_Texture* texture = nullptr;
    SDL_Color color = {255, 255, 255};
};

#endif //CLASH_OF_COLOSSEUM_BUTTON_H