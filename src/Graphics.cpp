#include "Graphics.h"
#include "Bow.h"
#include "GameMenu.h"
#include <iostream>

Graphics::Graphics() {
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    gameMusic = Mix_LoadMUS("../assets/gamemusic.mp3");
    if (!gameMusic) {
        std::cerr << "Erreur chargement musique jeu: " << Mix_GetError() << std::endl;
    }
    backgroundTexture = IMG_LoadTexture(renderer, "../assets/bgimg.png");
    if (!backgroundTexture) {
        std::cerr << "Erreur chargement background: " << IMG_GetError() << std::endl;
    }
    game_menu = new GameMenu(renderer, window);
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

void Graphics::updateEntities(bool draw){
    std::vector<Entity*> toDelete;

    for (Entity* e : entities) {
        Entity* closest = e->findClosestEntity(entities);
        if (!closest) continue;

        if (e->canAttackDistance(closest)) {
            // --- AJOUT : ORIENTATION AUTOMATIQUE ---
            // On force l'entité à regarder sa cible avant d'attaquer
            if (closest->getX() < e->getX()) {
                e->setDirection("left");
            } else {
                e->setDirection("right");
            }
            // ---------------------------------------

            if (e->canAttackTime()) {
                e->setState("attack");
                if (e->getWeapon()->type() == "Bow" || e->getWeapon()->type() == "Fireball") {
                    e->getWeapon()->attack(closest, e, &projectiles, e->getX(), e->getY());
                } else {
                    e->getWeapon()->attack(closest);
                }

                e->resetAttackTimer();

                if (closest->getHp() == 0) {
                    toDelete.push_back(closest); // suppression différée
                }
            }
        } else {
            e->setState("run");
            e->moveInDirection(closest->getX(), closest->getY());
        }

        e->updateAnimation();
        e->addAttackTimer();

        if(draw) e->draw(renderer, game_menu->getTimeSpeed());
        if(draw) e->drawHealthBar(renderer);

        if (e->getType() == "Archer") {
            Bow* bow = dynamic_cast<Bow *>(e->getWeapon());
            if(draw) bow->draw(e->getX() + e->getSize(), e->getY(), renderer);
        }
    }

    for (Entity* d : toDelete) {
        deleteEntity(d);
    }
}

void Graphics::updateProjectiles(bool draw){
    for (auto p = projectiles.begin(); p != projectiles.end(); ) {
        Projectile* proj = *p;
        if(SDL_GetTicks() >= proj->getEndTime()){
            delete proj;
            p = projectiles.erase(p);
            continue;
        }
        proj->move();
        if(draw) proj->draw(renderer, game_menu->getTimeSpeed());

        bool hit = false;

        for (Entity* e : entities) {
            if(proj->getOwner() != nullptr && e != proj->getOwner()){
                //Collision
                float dx = e->getX() - proj->getX();
                float dy = e->getY() - proj->getY();
                float distance = std::sqrt(dx * dx + dy * dy);
                float sumRadius = e->getSize() + 10;

                if (distance <= sumRadius) {
                    e->setHp(e->getHp() - proj->getDamage()); // inflige les dégâts
                    hit = true;
                    delete proj;
                    p = projectiles.erase(p);
                    break;
                }
            }
        }
        if(!hit){
            p++;
        }
    }
}

void Graphics::update(bool* running) {
    handleEvent(running);

    // On nettoie d'abord l'écran (noir par défaut)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // On dessine l'image de fond. Le NULL, NULL signifie "toute l'image" sur "toute la fenêtre"
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    } else {
        // Si l'image n'a pas chargé, on garde ton ancienne couleur jaune 'sable' en fallback
        SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
        SDL_RenderClear(renderer);
    }

    for(int i = 0; i<game_menu->getTimeSpeed(); i++) { // On répète l'action plusieurs fois si le temps est plus rapide
        bool draw = i == game_menu->getTimeSpeed()-1; //On dessine que si c'est la dernière update de la boucle
        updateEntities(draw);
        updateProjectiles(draw);

        if (entities.size() <= 5){
            deleteAllProjectiles();

            std::vector<Entity*> new_entities; //Temporary vector

            for(int i = 0; i+1<entities.size(); i+= 2){
                //We have now 2 parents
                int number_of_children = std::rand() % 5 + 1;
                //The 2 parents make between 1 and 5 children
                for(int j = 0; j<number_of_children; j++){
                    Entity* new_entity = this->createNewEntityFromParents(entities[i], entities[i+1]);
                    new_entities.push_back(new_entity);
                }
            }
            entities.insert(entities.end(), new_entities.begin(), new_entities.end());

        }
    }

    if(game_menu) game_menu->draw();
    SDL_RenderPresent(renderer);
}

void Graphics::handleEvent(bool* running){
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT: game_menu->faster(); break;
                case SDL_SCANCODE_LEFT:  game_menu->lower(); break;
                default: break;
            }
        }
    }
}

Entity* Graphics::createNewEntityFromParents(Entity* e1, Entity* e2){
    Entity* new_entity = nullptr;
    e1->setHp(e1->getMaxHp());
    e2->setHp(e2->getMaxHp());

    //Random position
    int width, height;
    SDL_GetWindowSize(getWindow(), &width, &height);
    int x = std::rand() % (width-50) + 50; // entre 50 et width-50
    int y = std::rand() % (height-50) + 50; // entre 50 et height-50

    Entity* e_temp = nullptr;
    //Choose the type of entity (e1 or e2)
    (std::rand() % 2) ? e_temp = e1: e_temp = e2;

    if(e_temp->getType() == "Guerrier"){
        new_entity = new Guerrier(x, y, renderer);
    } else if(e_temp->getType() == "Archer"){
        new_entity = new Archer(x, y, renderer);
    } else if(e_temp->getType() == "Mage"){
        new_entity = new Mage(x, y, renderer);
    } else if(e_temp->getType() == "Tank"){
        new_entity = new Golem(x, y, renderer);
    } else{
        new_entity = new Entity(x, y, renderer);
    }

    float X = (std::rand() % 101) / 100.0f;
    int new_max_hp = (e1->getMaxHp() * X) + (e2->getMaxHp() * (1.0f - X)); // X hp de e1 et 1-X hp de e2

    // 15% de chance de muter
    if ((std::rand() % 100) <= 15){
        float variation = std::rand() % 21 / 100.0f; // Donne un nombre entre 0% ou 20%

        if (std::rand() % 2 == 0) variation = -variation; // Si on a 0, on transforme 0.08 en -0.08 ; si on a 1, on fait rien

        float mutation_factor = 1.0f + variation; // Donne 0.92 (si -0.08) ou 1.08 (si +0.08)
        new_max_hp *= mutation_factor;
    }
    new_entity->setHp(new_max_hp);
    new_entity->setMaxHp(new_max_hp);

    return new_entity;
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

void Graphics::deleteAllProjectiles(){
    for (Projectile* p : projectiles) {
        delete p;
        p = nullptr;
    }

    projectiles.clear();
}