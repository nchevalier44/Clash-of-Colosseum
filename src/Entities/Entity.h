#ifndef CLASH_OF_COLOSSEUM_ENTITY_H
#define CLASH_OF_COLOSSEUM_ENTITY_H

#include <SDL2/SDL.h>
#include <vector>
#include <SDL2/SDL_image.h>
#include "../Weapons/Weapon.h"
#include <iostream>
#include <random>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <map>

using namespace std;

inline int randomRange(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

class Entity {
public:
    Entity(float x, float y, SDL_Renderer* renderer);
    virtual ~Entity();

    void deleteAllFrames();

    void startThread(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, int* game_time_speed, bool* same_type_peace, std::mutex* global_mutex);
    void stopThread();

    void setX(float new_x){ x = new_x; };
    void setY(float new_y){ y = new_y; };
    void setHp(int new_hp);
    void setMaxHp(int new_max_hp) { max_hp = new_max_hp; }
    void setSize(float new_size);
    void increaseAge(){age++;};
    virtual void updateAttackCooldown() { this->attack_cooldown = 500 + (weapon->getDamage() * 30); };
    float getX() const { return x; };
    float getY() const { return y; };
    float getSize() const { return size; };
    int getHp() const { return hp; }
    int getMaxHp() const { return max_hp; }
    int getAge() const { return age; }
    float getRatioHitboxWidth() const { return ratio_hitbox_width; }
    float getRatioHitboxHeight() const { return ratio_hitbox_height; }
    float getMoveSpeed() const { return move_speed; }
    void setMoveSpeed(float new_speed){ move_speed = new_speed; }
    void setRandomSize(float minSize, float maxSize);
    Weapon* getWeapon() { return weapon; }
    void resetAttackTimer(){ attack_timer -= attack_cooldown; }
    void addAttackTimer(){ attack_timer += 16; }

    bool canAttackDistance(Entity* entity);
    bool canAttackTime();

    double distance(float x2, float y2);
    Entity* findClosestEntity(vector<Entity*> entities, bool ignoreSameType = false);
    void moveInDirection(float x, float y);
    void drawHealthBar(SDL_Renderer* renderer);

    bool isFrameEmpty(SDL_Surface * surface);

    //Sprites
    virtual void loadSprites(SDL_Renderer* renderer); // à redéfinir
    virtual void updateAnimation();
    virtual void draw(SDL_Renderer* renderer);
    void setState(const string& new_state); // "idle", "run", "attack"
    void setDirection(const string& new_dir); // "left" ou "right"

    std::string getType() const { return type; };
    std::string getDirection() const { return direction; }


protected:
    int hp;
    int max_hp;
    float base_sprite_size = 1.0f;
    float ratio_hitbox_width;
    float ratio_hitbox_height;
    float size;
    float x;
    float y;
    Weapon* weapon;

    float attack_timer = 0.0f; // Compteur interne
    float attack_cooldown = 1000.0f;
    float move_speed = 1.0f;
    int age = 0;

    string sprites_file_right;
    string sprites_file_left;
    map<string, map<string, vector<SDL_Texture*>>> frames; // {"idle": {"left" : [frame_0, frame_1, ...], "right": ...}, "run": ..., ...}
    string state = "idle";
    string direction = "right";
    int current_frame = 0;
    Uint32 frame_delay = 100;
    float anim_timer = 0;
    SDL_Renderer* current_renderer = nullptr;

    // The entity attack at this number of frame
    // (for example thank to this variable, the arrow from the archer will be launch
    // at the end of his animation instead of the beginning)
    int frame_to_attack = 0;
    bool ready_to_attack = false; //This variable will be set to true when the animation is at the 'frame_to_attack' position

    string type = "Entity";

    std::thread entity_thread;
    std::atomic<bool> thread_is_running = false;
    
private:
    void threadLoop(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, int* game_time_speed, bool* same_type_peace, std::mutex* global_mutex);
    void threadUpdate(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, bool* same_type_peace, std::mutex* global_mutex);
};

#endif //CLASH_OF_COLOSSEUM_ENTITY_H
