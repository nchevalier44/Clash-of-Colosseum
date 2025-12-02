#ifndef CLASH_OF_COLOSSEUM_ENTITY_H
#define CLASH_OF_COLOSSEUM_ENTITY_H

#include <SDL2/SDL.h>
#include <vector>
#include <SDL2/SDL_image.h>
#include "Weapon.h"

using namespace std;
class Entity {
public:
    Entity(float x, float y, SDL_Renderer* renderer);
    virtual ~Entity();

    void setX(float new_x){ x = new_x; };
    void setY(float new_y){ y = new_y; };
    void setHp(int new_hp);
    void setMaxHp(int new_max_hp) { max_hp = new_max_hp; }
    void setSize(int new_size);
    float getX() const { return x; };
    float getY() const { return y; };
    int getSize() const { return size; };
    int getHp() const { return hp; }
    int getMaxHp() const { return max_hp; }
    float getMoveSpeed() const { return move_speed; }
    void setMoveSpeed(float new_speed){ move_speed = new_speed; }
    void setRandomSize(int minSize, int maxSize);
    Weapon* getWeapon() { return weapon; }
    void resetAttackTimer(){ attack_timer -= attack_cooldown; }
    void addAttackTimer(){ attack_timer += 16; }

    bool canAttackDistance(Entity* entity);
    bool canAttackTime();

    double distance(float x2, float y2);
    Entity* findClosestEntity(vector<Entity*> entities);
    void moveInDirection(float x, float y);
    void drawHealthBar(SDL_Renderer* renderer);

    //Sprites
    virtual void loadSprites(SDL_Renderer* renderer); // à redéfinir
    virtual void updateAnimation();
    virtual void draw(SDL_Renderer* renderer, int time_speed);
    void setState(const string& new_state); // "idle", "run", "attack"
    void setDirection(const string& new_dir); // "left" ou "right"

    std::string getType() const { return type; };
    std::string getDirection() const { return direction; }


protected:
    int hp;
    int max_hp;
    int size = 1.0f;
    int foot_offset = 10;  // ajuste entre 5 et 20 selon ton sprite
    double sprite_scale; // échelle d’affichage du sprite
    float x;
    float y;
    Weapon* weapon;
    float attack_timer = 0.0f; // Compteur interne
    float attack_cooldown = 1000.0f;
    float move_speed = 1.0f;
    float baseSpriteSize = 1.0f;

    vector<SDL_Texture*> frames;
    string state = "idle";
    string direction = "right";
    int current_frame = 0;
    Uint32 frame_delay = 120;
    float anim_timer = 0;
    SDL_Renderer* current_renderer = nullptr;

    string type = "Entity";

};

class Guerrier : public Entity {
public:
    Guerrier(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer);
};

class Archer : public Entity {
public:
    Archer(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer);
};

class Golem : public Entity {
public:
    Golem(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer) override;
};


class Mage : public Entity {
public:
    Mage(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer);
};


#endif //CLASH_OF_COLOSSEUM_ENTITY_H
