#ifndef CLASH_OF_COLOSSEUM_WEAPON_H
#define CLASH_OF_COLOSSEUM_WEAPON_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

// Eviter l'import circulaire
class Projectile;
class Entity;

class Weapon {
public:
    Weapon(int damage=1, int range=40);
    ~Weapon() = default;

    int getRange() const { return range; };
    void setRange(int new_range);
    int getDamage() const { return damage; };
    void setDamage(int new_damage);

    // 1. Attaque au Corps-à-Corps (Spear, Punch)
    // Modifie directement les PV de la cible (impact immédiat).
    virtual void attack(Entity* target);

    // 2. Attaque à Distance (Bow, Fireball)
    // Surcharge virtuelle : elle ne touche pas la cible, mais crée un "Projectile".
    // Elle est vide {} par défaut ici car une épée n'a pas besoin de créer de projectile.
    virtual void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y){};

    virtual std::string type() { return "Weapon"; }

protected:
    // 'protected' pour que les classes filles (Bow, Spear...) puissent modifier ces valeurs
    int damage;
    int range;
    int min_range;
    int max_range;
};

#endif //CLASH_OF_COLOSSEUM_WEAPON_H