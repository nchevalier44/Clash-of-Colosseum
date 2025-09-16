#ifndef CLASH_OF_COLOSSEUM_WEAPON_H
#define CLASH_OF_COLOSSEUM_WEAPON_H

#include <SDL2/SDL.h>
#include <string>

class Entity;

class Weapon {
public:
    Weapon(int damage=1, int range=10);
    ~Weapon() = default;

    int getRange() const { return range; };
    int getDamage() const { return damage; };

    virtual void attack(Entity* entity);
    virtual void draw(int x, int y, SDL_Renderer* renderer) = 0;

protected:
    int damage;
    int range;
};

// --- Armes spécifiques ---
class Epee : public Weapon {
public:
    Epee() : Weapon(10, 30) {}
    void draw(int x, int y, SDL_Renderer* renderer) override;
};

class Arc : public Weapon {
public:
    Arc() : Weapon(7, 150) {}
    void draw(int x, int y, SDL_Renderer* renderer) override;
};

class Catapulte : public Weapon {
public:
    Catapulte() : Weapon(20, 200) {}
    void draw(int x, int y, SDL_Renderer* renderer) override;
};

class BouleDeFeu : public Weapon {
public:
    BouleDeFeu() : Weapon(15, 120) {}
    void draw(int x, int y, SDL_Renderer* renderer) override;
};

#endif //CLASH_OF_COLOSSEUM_WEAPON_H
