#include "Weapon.h"
#include "Entity.h"

Weapon::Weapon(int damage, int range) : damage(damage), range(range){
}

void Weapon::attack(Entity* entity){
    entity->setHp(entity->getHp() - this->damage);
}