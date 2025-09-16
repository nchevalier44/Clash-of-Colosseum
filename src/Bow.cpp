#include "Bow.h"
#include "Projectile.h"

Bow::Bow(int damage, int range) : Weapon(damage, range){
}

void Bow::attack(Entity* entity) {
    //launch a projectile in direction of the entity
    //Projectile* projectile = new Projectile();
}