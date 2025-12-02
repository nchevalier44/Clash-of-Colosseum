#include "Fist.h"

Fist::Fist(int damage, int range) : Weapon(damage, range){
    min_range = 5;
    max_range = 15;
}