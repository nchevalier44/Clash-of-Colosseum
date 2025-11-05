#include "Graphics.h"
#include <iostream>

Graphics::Graphics() {
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    gameMusic = Mix_LoadMUS("../assets/game_music.mp3");
    if (!gameMusic) {
        std::cerr << "Erreur chargement musique jeu: " << Mix_GetError() << std::endl;
    }

}

SDL_Renderer* Graphics::getRenderer() const {
    return renderer;
}

Graphics::~Graphics() {

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        this->renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (!entities.empty()) {
        entities.clear();
    }
    if (gameMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(gameMusic);
        gameMusic = nullptr;
    }

}

void Graphics::setEntities(const std::vector<Entity*>& ents) {
    entities = ents;
}

void Graphics::update(bool* running) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
    SDL_RenderClear(renderer);

    //Move and attack entities
    for(Entity* e : entities){
        Entity* closest = e->findClosestEntity(entities);
        if(closest != nullptr){
            if(e->canAttack(closest)){
                e->getWeapon()->attack(closest);

                if(closest->getHp() == 0 ){
                    deleteEntity(closest);
                }
            } else{
                e->moveInDirection(closest->getX(), closest->getY());
            }
        }
        e->drawHealthBar(renderer);

        if (e->getWeapon()) {
            e->getWeapon()->draw(e->getX() + e->getSize(), e->getY(), renderer);
        }

        filledCircleRGBA(renderer, (short)e->getX(), (short)e->getY(),
                         (short)e->getSize(), 255, 0, 0, 255);
    }

    //Projectiles
    /*for(Projectile* p : projectiles){
        p->move();
        p->draw(renderer);
    }*/

    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }

        if (event.type == SDL_KEYDOWN && !entities.empty()) {
            Entity* e = entities[0]; // on déplace seulement le premier pour test
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_UP:    e->setY(e->getY() - 5); break;
                case SDL_SCANCODE_DOWN:  e->setY(e->getY() + 5); break;
                case SDL_SCANCODE_RIGHT: e->setX(e->getX() + 5); break;
                case SDL_SCANCODE_LEFT:  e->setX(e->getX() - 5); break;
                default: break;
            }
        }
        if (entities.size() <= 1)
        {
            std::cout << "Le combat est terminé !" << std::endl;
            *running = false;
        }
    }
}

void Graphics::deleteEntity(Entity* entity){
    for (auto it = entities.begin(); it != entities.end(); it++) {
        if (*it == entity) {
            delete *it;
            entities.erase(it);
            return;
        }
    }
}