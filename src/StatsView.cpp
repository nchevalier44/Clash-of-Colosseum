#include "StatsView.h"

#include <SDL_ttf.h>

StatsView::StatsView(SDL_Renderer* r, int w, int h) : renderer(r), width(w), height(h) {
    titleFont = TTF_OpenFont("../assets/arial.ttf", 24);
}

StatsView::~StatsView() {
    TTF_CloseFont(titleFont);
}

// Fonction utilitaire pour transformer vos objets complexes en liste de float simples
std::vector<float> StatsView::extractAttribute(SimulationStats& stats, std::string category, std::string field) {
    std::vector<float> result;
    for (auto* gen : stats.getGenerations()) {
        if (category == "hp") {
            if (field == "mean") result.push_back(gen->hp.mean);
            if (field == "max") result.push_back(gen->hp.maximum);
        } else if (category == "damage") {
            if (field == "mean") result.push_back(gen->damage.mean);
        } else if (category == "speed") {
            if (field == "mean") result.push_back(gen->speed.mean);
        }
    }
    return result;
}

std::vector<float> StatsView::extractPopulation(SimulationStats& stats, std::string className) {
    std::vector<float> result;
    for (auto* gen : stats.getGenerations()) {
        // On utilise la map type_counts
        result.push_back((float)gen->type_counts[className]);
    }
    return result;
}

void StatsView::showStats(SimulationStats& stats) {
    bool viewing = true;
    SDL_Event e;

    // --- CONFIGURATION DE LA GRILLE (2x2) ---
    int pad = 20;
    int gW = (width / 2) - (pad * 2); // Largeur d'un graphe
    int gH = (height / 2) - (pad * 2); // Hauteur d'un graphe

    // 1. GRAPHIQUE DÉMOGRAPHIE (Haut-Gauche)
    Graph popGraph(pad, pad + 30, gW, gH, renderer);
    popGraph.setLabels("Génération", "Population");
    // On ajoute les séries basées sur votre Map
    popGraph.addSerie(Serie(extractPopulation(stats, "Guerrier"), {0, 255, 0, 255}, "Guerrier")); // Vert
    popGraph.addSerie(Serie(extractPopulation(stats, "Mage"), {0, 255, 255, 255}, "Mage"));       // Cyan
    popGraph.addSerie(Serie(extractPopulation(stats, "Archer"), {255, 255, 0, 255}, "Archer"));   // Jaune
    popGraph.addSerie(Serie(extractPopulation(stats, "Golem"), {128, 128, 128, 255}, "Golem"));   // Gris

    // 2. GRAPHIQUE ATTRIBUTS MOYENS (Haut-Droit)
    Graph attrGraph(width/2 + pad, pad + 30, gW, gH, renderer);
    popGraph.setLabels("Génération", "Moyennes");
    attrGraph.addSerie(Serie(extractAttribute(stats, "hp", "mean"), {0, 200, 0, 255}, "HP Moy."));    // Vert Foncé
    attrGraph.addSerie(Serie(extractAttribute(stats, "damage", "mean"), {200, 0, 0, 255}, "Dmg Moy.")); // Rouge


    // 3. GRAPHIQUE PERFORMANCE (Bas-Gauche)
    Graph perfGraph(pad, height/2 + pad + 30, gW, gH, renderer);
    popGraph.setLabels("Génération", "Vitesse / Age");
    perfGraph.addSerie(Serie(extractAttribute(stats, "speed", "mean"), {0, 0, 255, 255}, "Vitesse Moy.")); // Bleu
    // Vous pourriez ajouter l'âge ici si vous l'avez dans les stats

    // 4. GRAPHIQUE SURVIE (Bas-Droit)
    Graph survGraph(width/2 + pad, height/2 + pad + 30, gW, gH, renderer);
    popGraph.setLabels("Génération", "Entitées");

    std::vector<float> initial, remaining;
    for(auto* gen : stats.getGenerations()) {
        initial.push_back((float)gen->nb_initial_entities);
        remaining.push_back((float)gen->nb_remaining_entities);
    }
    survGraph.addSerie(Serie(initial, {200, 200, 200, 255}, "Total Debut"));
    survGraph.addSerie(Serie(remaining, {255, 100, 0, 255}, "Survivants"));

        // Fond Blanc
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        // Titre Principal
        SDL_Surface* titleSurf = TTF_RenderUTF8_Blended(titleFont, "Statistiques de Simulation", {0,0,0,255});
        SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
        SDL_Rect titleRect = {width/2 - titleSurf->w/2, 5, titleSurf->w, titleSurf->h};
        SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
        SDL_FreeSurface(titleSurf);
        SDL_DestroyTexture(titleTex);

        // Dessin des 4 graphiques (SANS Present !)
        popGraph.draw();
        attrGraph.draw();
        perfGraph.draw();
        survGraph.draw();

        // Légendes manuelles si besoin (drawLegend est appelé dans draw() ?)
        popGraph.drawLegend();
        attrGraph.drawLegend();
        perfGraph.drawLegend();
        survGraph.drawLegend();

        SDL_RenderPresent(renderer);

    // --- BOUCLE D'AFFICHAGE MODALE ---
    while (viewing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                viewing = false;
            }
        }
    }
}