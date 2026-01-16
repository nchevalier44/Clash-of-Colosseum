#ifndef CLASH_OF_COLOSSEUM_SIMULATIONSTATS_H
#define CLASH_OF_COLOSSEUM_SIMULATIONSTATS_H

#include <map>
#include <string>
#include <vector>

#include "Entities/Entity.h"

// Structure utilitaire pour stocker les métriques d'un attribut (ex: Force, Vitesse).
// Permet de tracer les courbes Min, Moyenne et Max sur les graphiques.
struct AttributStats {
    float minimum = 0;
    float mean = 0;
    float maximum = 0;
};

// Contient toutes les données collectées au début et à la fin d'une génération.
struct GenerationStats {
    // Utilisation de std::chrono pour une précision temporelle élevée
    std::chrono::duration<float, std::milli> duration_milliseconds = std::chrono::duration<float, std::milli>::zero();

    int generation_id = 0;
    int nb_initial_entities = 0;
    int nb_remaining_entities = 0;

    // Compteur de population par classe
    std::map<std::string, int> type_counts = {{"Guerrier", 0},{"Archer", 0}, {"Mage", 0}, {"Golem", 0}};

    // Composition : Chaque génération possède ses propres stats pour chaque attribut
    AttributStats damage;
    AttributStats hp;
    AttributStats speed;
    AttributStats age;
};

class SimulationStats {
public:
    SimulationStats(){};
    ~SimulationStats();

    // Capture l'état de la population au DÉBUT d'une génération
    void addNewGeneration(std::vector<Entity*> entities, int id_generation);

    // Met à jour les données (durée, survivants) à la FIN d'une génération
    void updateLastGeneration(int nb_entities);

    void setStartTime(std::time_t start_time) { start_sim_time = start_time; };

    // Stocke la durée théorique (temps simulé), prenant en compte l'accélération (x1, x5...)
    void setDuration(std::chrono::duration<float, std::milli> duration) { theoretical_sim_duration = duration; };

    void setEndTime(std::time_t end_time) { end_sim_time = end_time; };
    std::time_t getStartTime() { return start_sim_time; };
    std::chrono::duration<float, std::milli> getDuration(){ return theoretical_sim_duration; }
    std::time_t getEndTime() { return end_sim_time; };

    // Retourne l'historique complet pour l'affichage des graphiques
    std::vector<GenerationStats*> getGenerations(){ return generations; };

    void setMutationTypeRate(int rate) { mutation_type_rate = rate; }
    int getMutationTypeRate() const { return mutation_type_rate; }

    void setMutationStatsRate(int rate) { mutation_stats_rate = rate; }
    int getMutationStatsRate() const { return mutation_stats_rate; }

    void setSameTypePeace(bool peace) { same_type_peace = peace; }
    bool getSameTypePeace() const { return same_type_peace; }

private:
    std::vector<GenerationStats*> generations;

    std::time_t start_sim_time;
    std::time_t end_sim_time;

    // Cumul du temps écoulé dans le monde du jeu (pas le temps réel)
    std::chrono::duration<float, std::milli> theoretical_sim_duration = std::chrono::duration<float, std::milli>::zero();

    // Paramètres de la partie
    bool same_type_peace = false;
    int mutation_type_rate = 0;
    int mutation_stats_rate = 0;
};

#endif //CLASH_OF_COLOSSEUM_SIMULATIONSTATS_H