#include "SimulationStats.h"

#include "Graphics.h"

void SimulationStats::addNewGeneration(std::vector<Entity*> entities, int id_generation) {
    if (entities.size() == 0) return;

    GenerationStats* generation_stats = new GenerationStats();
    generations.push_back(generation_stats);

    generation_stats->nb_initial_entities = entities.size();
    generation_stats->generation_id = id_generation;

    AttributStats damage_stats;
    AttributStats hp_stats;
    AttributStats speed_stats;
    AttributStats age_stats;

    // Initialisation des minimums avec la première entité trouvée
    // C'est plus sûr que d'initialiser à 0, car si toutes les entités ont 100 PV, le min resterait à 0 (faux).
    damage_stats.minimum = entities[0]->getWeapon()->getDamage();
    hp_stats.minimum = entities[0]->getMaxHp();
    speed_stats.minimum = entities[0]->getMoveSpeed();
    age_stats.minimum = entities[0]->getAge();

    float sum_damage = 0;
    float sum_hp = 0;
    float sum_speed = 0;
    float sum_age = 0;

    // Parcours unique pour calculer Min, Max et Sommes (pour la moyenne) en une seule passe
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

    // Calcul final des moyennes
    damage_stats.mean = sum_damage / entities.size();
    hp_stats.mean = sum_hp / entities.size();
    age_stats.mean = sum_age / entities.size();
    speed_stats.mean = sum_speed / entities.size();

    generation_stats->damage = damage_stats;
    generation_stats->hp = hp_stats;
    generation_stats->age = age_stats;
    generation_stats->speed = speed_stats;
}

void SimulationStats::updateLastGeneration(int nb_entities) {
    // Force la mise à jour du timer global pour récupérer le temps exact écoulé
    Graphics::changeGameSpeed();

    GenerationStats* gen = generations[generations.size()-1];
    gen->nb_remaining_entities = nb_entities;

    // Stocke la durée "simulée" (ex: si on joue en x10 pendant 1s réelle, ça compte pour 10s de simulation)
    gen->duration_milliseconds = theoretical_sim_duration;

    // Reset du compteur pour la prochaine génération
    theoretical_sim_duration = std::chrono::duration<float, std::milli>::zero();
}

SimulationStats::~SimulationStats() {
    for (GenerationStats* gen : generations) {
        delete gen;
    }
}