#include "Spear.h"

Spear::Spear(int damage, int range) : Weapon(damage, range){
    min_range = 18;
    max_range = 25;
}