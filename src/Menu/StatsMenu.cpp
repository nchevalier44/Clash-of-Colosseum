#include "StatsMenu.h"
#include <SDL_ttf.h>
#include <iomanip>
#include <sstream>

StatsMenu::StatsMenu(SDL_Renderer* r, SDL_Window* window, SDL_Texture* background, bool* keep_playing) : window(window), background(background), keep_playing(keep_playing), renderer(r) {
    titleFont = TTF_OpenFont("../assets/arial.ttf", 24);
    infoFont = TTF_OpenFont("../assets/arial.ttf", 16);
    buttonFont = TTF_OpenFont("../assets/arial.ttf", 14);
}

StatsMenu::~StatsMenu() {
    TTF_CloseFont(titleFont);
    TTF_CloseFont(infoFont);
    TTF_CloseFont(buttonFont);
    for (auto b : buttons) delete b;
    if (currentGraph) delete currentGraph;
}

std::string StatsMenu::formatDateTime(std::time_t time) {
    if (time == 0) return "N/A";
    std::tm* tm = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(tm, "%d/%m/%Y %H:%M:%S");
    return ss.str();
}

// Fonction utilitaire pour extraire les données min/moy/max
std::vector<float> StatsMenu::extractAttribute(SimulationStats& stats, std::string category, std::string field) {
    std::vector<float> result;
    for (auto* gen : stats.getGenerations()) {
        AttributStats* target = nullptr;

        if (category == "hp") target = &gen->hp;
        else if (category == "damage") target = &gen->damage;
        else if (category == "speed") target = &gen->speed;
        else if (category == "age") target = &gen->age;

        if (target) {
            if (field == "mean") result.push_back(target->mean);
            else if (field == "max") result.push_back(target->maximum);
            else if (field == "min") result.push_back(target->minimum);
        }
    }
    return result;
}

void StatsMenu::initButtons(SimulationStats& stats) {
    // Nettoyage ancien boutons
    for (auto b : buttons) delete b;
    buttons.clear();

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    int btnY = height - 50;

    makeBtn(stats, "Âge", "age", width/7, btnY);
    makeBtn(stats, "HP", "hp", 2*width/7, btnY);
    makeBtn(stats, "Dégâts", "damage", 3*width/7, btnY);
    makeBtn(stats, "Vitesse", "speed", 4*width/7, btnY);
    makeBtn(stats, "Population", "population", 5*width/7, btnY);
    makeBtn(stats, "Durée", "duration", 6*width/7, btnY);
}

void StatsMenu::makeBtn(SimulationStats& stats, std::string label, std::string modeKey, int x, int y) {
    Button* b = new Button(label, buttonFont, renderer, [this, modeKey, &stats](Button* btn) {
            this->updateGraphData(stats, modeKey);
        });
    b->setX(x-b->getRect().w/2);
    b->setY(y);
    buttons.push_back(b);
}

void StatsMenu::updateGraphData(SimulationStats& stats, std::string mode) {

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    // Définition de la zone du graphique
    int graphX = 300; // Plus large à gauche pour afficher les infos
    int graphY = 80;
    int graphW = width - graphX - 20;
    int graphH = height - 150;

    if (currentGraph) {
        currentGraph->setWidth(graphW);
        currentGraph->setHeight(graphH);
        if (mode == currentMode) return; //Mode doesn't change
    }


    currentGraph = new Graph(graphX, graphY, graphW, graphH, renderer);
    currentGraph->setXLabel("Générations");

    // Logique basée sur les strings au lieu des enums
    if (mode == "age") {
        currentGraph->setYLabel("Âge");
        currentGraph->addSerie(Serie(extractAttribute(stats, "age", "max"), {255, 0, 0, 255}, "Max"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "age", "mean"), {255, 165, 0, 255}, "Moy"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "age", "min"), {0, 255, 0, 255}, "Min"));
    }
    else if (mode == "hp") {
        currentGraph->setYLabel("Points de Vie");
        currentGraph->addSerie(Serie(extractAttribute(stats, "hp", "max"), {255, 0, 0, 255}, "Max"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "hp", "mean"), {255, 165, 0, 255}, "Moy"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "hp", "min"), {0, 255, 0, 255}, "Min"));
    }
    else if (mode == "damage") {
        currentGraph->setYLabel("Dégâts");
        currentGraph->addSerie(Serie(extractAttribute(stats, "damage", "max"), {255, 0, 0, 255}, "Max"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "damage", "mean"), {255, 165, 0, 255}, "Moy"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "damage", "min"), {0, 255, 0, 255}, "Min"));
    }
    else if (mode == "speed") {
        currentGraph->setYLabel("Vitesse");
        currentGraph->addSerie(Serie(extractAttribute(stats, "speed", "max"), {255, 0, 0, 255}, "Max"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "speed", "mean"), {255, 165, 0, 255}, "Moy"));
        currentGraph->addSerie(Serie(extractAttribute(stats, "speed", "min"), {0, 255, 0, 255}, "Min"));
    }
    else if (mode == "population") {
        currentGraph->setYLabel("Nombre d'entitées");
        std::vector<float> initial, remaining;
        for(auto* gen : stats.getGenerations()) {
            initial.push_back((float)gen->nb_initial_entities);
            remaining.push_back((float)gen->nb_remaining_entities);
        }
        currentGraph->addSerie(Serie(initial, {100, 100, 255, 255}, "Début Gen"));
        currentGraph->addSerie(Serie(remaining, {255, 100, 100, 255}, "Fin Gen"));
    }
    else if (mode == "duration") {
        currentGraph->setYLabel("Durée (s)");
        std::vector<float> duration;
        for(auto* gen : stats.getGenerations()) {
            duration.push_back((int)gen->duration_milliseconds/1000);
        }
        currentGraph->addSerie(Serie(duration, {128, 0, 128, 255}, "Durée"));
    }
}

void StatsMenu::drawInfoPanel(SimulationStats& stats) {
    int x = 20;
    int y = 80;
    int lineHeight = 30;


    drawText("Début : " + formatDateTime(stats.getStartTime()), x, y, lineHeight);
    y += lineHeight;
    drawText("Fin : " + formatDateTime(stats.getEndTime()), x, y, lineHeight);
    y += lineHeight;
    y += 10; // Espace

    drawText("--- Paramètres ---",x, y, lineHeight);
    y += lineHeight;
    // Affichage des variables directes
    drawText("Mutation Type: " + std::to_string(stats.getMutationTypeRate()) + "%", x, y, lineHeight);
    y += lineHeight;
    drawText("Mutation Stats: " + std::to_string(stats.getMutationStatsRate()) + "%", x, y, lineHeight);
    y += lineHeight;

    std::string peaceTxt = stats.getSameTypePeace() ? "Oui" : "Non";
    drawText("Paix même type: " + peaceTxt, x, y, lineHeight);
    y += lineHeight;
}

void StatsMenu::drawText(std::string text, int x, int y, int lineHeight) {
    SDL_Surface* surf = TTF_RenderUTF8_Blended(infoFont, text.c_str(), {255, 255, 255});
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect r = {x, y, surf->w, surf->h};

        //highlight
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
        SDL_Rect highlight;
        highlight.w = r.w + 10;
        highlight.h = r.h + 10;
        highlight.x = r.x - 5;
        highlight.y = r.y - 5;
        SDL_RenderFillRect(renderer, &highlight);


        SDL_RenderCopy(renderer, tex, NULL, &r);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void StatsMenu::showStats(SimulationStats& stats) {
    running = new bool(true);
    SDL_Event event;
    initButtons(stats);
    updateGraphData(stats, "age");
    while (*running && *keep_playing) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *running = false;
                *keep_playing = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                *running = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT) {
                    //Click on a button
                    for (Button* b : buttons) {
                        SDL_Rect rect = b->getRect();
                        if ((rect.x <= x && x <= rect.x + rect.w) && (rect.y <= y && y <= rect.y + rect.h)) {
                            b->onClick(b);
                        }
                    }
                }
            }

            if (event.type == SDL_MOUSEMOTION) {
                int x = event.motion.x;
                int y = event.motion.y;

                //Hover a button = change color
                for (Button* b : buttons) {
                    SDL_Rect rect = b->getRect();
                    if ((rect.x <= x && x <= rect.x + rect.w) && (rect.y <= y && y <= rect.y + rect.h)) {
                        b->setHovering(true);
                    } else {
                        b->setHovering(false);
                    }
                }
            }

            // Détection du redimensionnement
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                int w = event.window.data1;
                int h = event.window.data2;

                // Si la largeur ou la hauteur est trop petite
                if (w < 490 || h < 450) {
                    // On force la fenêtre à revenir à la taille minimale
                    // std::max prend le plus grand des deux nombres
                    SDL_SetWindowSize(window, std::max(w, 490), std::max(h, 450));
                }
            }
        }
        draw(stats);
        SDL_Delay(16);
    }
}

void StatsMenu::draw(SimulationStats& stats) {
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    // Titre
    SDL_Surface* titleSurf = TTF_RenderUTF8_Blended(titleFont, "Analyse de la Simulation", {255,255,255,255});
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
    SDL_Rect titleRect = {width/2 - titleSurf->w/2, 10, titleSurf->w, titleSurf->h};
    SDL_FreeSurface(titleSurf);

    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
    SDL_Rect highlight;
    highlight.w = titleRect.w + 10;
    highlight.h = titleRect.h + 10;
    highlight.x = titleRect.x - 5;
    highlight.y = titleRect.y - 5;
    SDL_RenderFillRect(renderer, &highlight);

    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_DestroyTexture(titleTex);

    // Infos à gauche
    drawInfoPanel(stats);

    // Graphique au centre
    if (currentGraph) {
        updateGraphData(stats, currentMode);
        currentGraph->draw();
    }

    // Boutons en bas
    int button_size = buttons.size();
    for (int i =0; i<button_size; i++) {
        Button* b = buttons[i];
        b->setX((i+1)*width/(button_size+1));
        b->setY(height - 50);
        b->draw(window, renderer);
    }

    SDL_RenderPresent(renderer);

}