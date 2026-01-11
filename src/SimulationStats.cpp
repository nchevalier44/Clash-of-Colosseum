#include "SimulationStats.h"

void SimulationStats::addNewGeneration(std::vector<Entity*> entities, int id_generation) {
    GenerationStats* generation_stats = new GenerationStats();
    generations.push_back(generation_stats);

    generation_stats->nb_initial_entities = entities.size();
    generation_stats->generation_id = id_generation;

    AttributStats damage_stats;
    AttributStats hp_stats;
    AttributStats speed_stats;
    AttributStats age_stats;
    float sum_damage = 0;
    float sum_hp = 0;
    float sum_speed = 0;
    float sum_age = 0;

    for (Entity* e : entities) {
        generation_stats->type_counts[e->getType()]++;

        float damage = e->getWeapon()->getDamage();
        sum_damage += damage;
        if (damage < damage_stats.minimum) damage_stats.minimum = damage;
        if (damage > damage_stats.maximum) damage_stats.maximum = damage;

        float hp = e->getMaxHp();
        sum_hp += hp;
        if (hp < hp_stats.minimum) hp_stats.minimum = hp;
        if (hp > hp_stats.maximum) hp_stats.maximum = hp;

        float age = e->getAge();
        sum_age += age;
        if (age < age_stats.minimum) age_stats.minimum = age;
        if (age > age_stats.maximum) age_stats.maximum = age;

        float speed = e->getMoveSpeed();
        sum_speed += speed;
        if (speed < speed_stats.minimum) speed_stats.minimum = speed;
        if (speed > speed_stats.maximum) speed_stats.maximum = speed;
    }

    damage_stats.mean = sum_damage / entities.size();
    hp_stats.mean = sum_hp / entities.size();
    age_stats.mean = sum_age / entities.size();
    speed_stats.mean = sum_speed / entities.size();

    generation_stats->damage = damage_stats;
    generation_stats->hp = hp_stats;
    generation_stats->age = age_stats;
    generation_stats->speed = speed_stats;
}

void SimulationStats::updateLastGeneration(int nb_entities, Uint32 duration) {
    GenerationStats* gen = generations[generations.size()-1];
    gen->nb_remaining_entities = nb_entities;
    gen->duration_milliseconds = duration;
}

SimulationStats::~SimulationStats() {
    for (GenerationStats* gen : generations) {
        delete gen;
    }
}