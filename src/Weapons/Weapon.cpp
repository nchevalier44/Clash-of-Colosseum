#include "Weapon.h"
#include "../Entities/Entity.h"

Weapon::Weapon(int damage, int range) : damage(damage), range(range){
}

void Weapon::attack(Entity* entity){
    entity->setHp(entity->getHp() - this->damage);
}

void Weapon::setDamage(int new_damage) {
    if (new_damage >= 5) {
        damage = new_damage;
    } else {
        damage = 5;
    }
}


void Weapon::setRange(int new_range){
    if(new_range > max_range){
        this->range = max_range;
    } else if(new_range < min_range){
        this->range = min_range;
    } else{
        this->range = new_range;
    }
}