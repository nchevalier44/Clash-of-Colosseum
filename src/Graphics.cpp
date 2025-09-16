#include "Graphics.h"
#include <iostream>

Graphics::Graphics(){
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");

    //Générer 5 entités aléatoires
    srand(time(NULL));
    for(int i=0; i<5;i++){
        int x = (rand() % 590) + 10;
        int y = (rand() % 440) + 10;
        Entity* entity = new Entity(x, y, 20, 100, 100);
        entities.push_back(entity);
    }
}
SDL_Renderer* Graphics::getRenderer() const { return renderer; }
Graphics::~Graphics() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        this->renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void Graphics::update(bool* running) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
    SDL_RenderClear(renderer);

    for(Entity* e : entities){
        Entity* closest = e->findClosestEntity(entities);
        if(closest != nullptr){
            e->move(closest->getX(), closest->getY());
        }
        e->drawHealthBar(renderer);
        e->getWeapon()->draw(e->getX()+e->getSize(), e->getY(), renderer);
        filledCircleRGBA(renderer, (short) e->getX(), (short) e->getY(), (short) e->getSize(), 255, 0, 0, 255);
    }

    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }

        if(event.type == SDL_KEYDOWN && !entities.empty()) {
            Entity* e = entities[0]; // on déplace seulement le premier pour test
            switch(event.key.keysym.scancode){
                case SDL_SCANCODE_UP: e->setY(e->getY() - 5); break;
                case SDL_SCANCODE_DOWN: e->setY(e->getY() + 5); break;
                case SDL_SCANCODE_RIGHT: e->setX(e->getX() + 5); break;
                case SDL_SCANCODE_LEFT: e->setX(e->getX() - 5); break;
            }
        }
    }
}