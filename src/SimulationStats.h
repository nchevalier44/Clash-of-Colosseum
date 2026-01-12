#ifndef CLASH_OF_COLOSSEUM_SIMULATIONSTATS_H
#define CLASH_OF_COLOSSEUM_SIMULATIONSTATS_H

#include <map>
#include <string>
#include <vector>

#include "Entities/Entity.h"

struct AttributStats {
    float minimum = 0;
    float mean = 0;
    float maximum = 0;
};

struct GenerationStats {
    Uint32 duration_milliseconds = 0;
    int generation_id = 0;
    int nb_initial_entities = 0;
    int nb_remaining_entities = 0;
    std::map<std::string, int> type_counts = {{"Guerrier", 0},{"Archer", 0}, {"Mage", 0}, {"Golem", 0}};
    AttributStats damage;
    AttributStats hp;
    AttributStats speed;
    AttributStats age;
};

class SimulationStats {
public:
    SimulationStats(){};
    ~SimulationStats();
    void addNewGeneration(std::vector<Entity*> entities, int id_generation);
    void updateLastGeneration(int nb_entities, Uint32 duration);
    void setStartTime(std::time_t start_time) { start_sim_time = start_time; };
    void setEndTime(std::time_t end_time) { end_sim_time = end_time; };
    std::time_t getStartTime() { return start_sim_time; };
    std::time_t getEndTime() { return end_sim_time; };
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
    bool same_type_peace = false;
    int mutation_type_rate = 0;
    int mutation_stats_rate = 0;
};

#endif //CLASH_OF_COLOSSEUM_SIMULATIONSTATS_H