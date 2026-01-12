#include "HistoryMenu.h"
#include <iomanip>
#include "Graph.h"
#include "../StatsMenu.h"

HistoryMenu::HistoryMenu(SDL_Window* window, SDL_Renderer* renderer, std::vector<SimulationStats*>* sim_stats, SDL_Texture* background, bool* keep_playing) : window(window), renderer(renderer), keep_playing(keep_playing), background(background) {
    font = TTF_OpenFont("../assets/arial.ttf", 20);

    int sim_size = sim_stats->size();
    for (int i=0; i<sim_size; i++) {
        Button* button = new Button("Voir Stats", font, renderer, [this, keep_playing, background, window, renderer, sim_stats](Button* button) {
            int id = searchIdFromButton(button);
            if (id == -1) return;
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            SimulationStats* sim = (*sim_stats)[id];
            StatsMenu view(renderer, window, background, keep_playing);
            view.showStats(*sim);
        });

        std::string text = "Sim #" + std::to_string(i+1) + " - " + formatDate((*sim_stats)[i]->getStartTime());
        std::pair<std::string, Button*> pair(text, button);
        list_sim.push_back(pair);
    }

    draw();
    handleEvent();
}

int HistoryMenu::searchIdFromButton(Button* button) {
    int sim_size = list_sim.size();
    for (int i = 0; i < sim_size; i++) {
        if (list_sim[i].second == button) {
            return i;
        }
    }
    return -1;
}

HistoryMenu::~HistoryMenu() {
    for (auto pair : list_sim) {
        delete pair.second;
    }
    delete running;
    TTF_CloseFont(font);
}

std::string HistoryMenu::formatDate(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(tm, "%d/%m/%Y %H:%M:%S");
    return ss.str();
}

void HistoryMenu::draw() {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    int spacing = 50;
    int offset = window_height/6;

    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }

    int sim_size = list_sim.size();
    if (sim_size == 0) {
        SDL_Surface* texte_surface = TTF_RenderUTF8_Solid(font, "Aucune simulation effectuée", {255, 255, 255});
        SDL_Texture* texte_texture = SDL_CreateTextureFromSurface(renderer, texte_surface);
        SDL_Rect texte_rect = {int(window_width/2 - texte_surface->w/2), int(window_height/2 - texte_surface->h/2), texte_surface->w, texte_surface->h};
        SDL_RenderCopy(renderer, texte_texture, NULL, &texte_rect);
        SDL_FreeSurface(texte_surface);
        SDL_DestroyTexture(texte_texture);
    }

    for (int i = 0; i < sim_size; i++) {
        std::pair<std::string, Button*> pair = list_sim[i];
        std::string text = pair.first;
        Button* button = pair.second;

        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
        SDL_Surface* texte_surface = TTF_RenderUTF8_Solid(font, text.c_str(), {255, 255, 255});
        SDL_Texture* texte_texture = SDL_CreateTextureFromSurface(renderer, texte_surface);
        SDL_Rect texte_rect = {int(window_width / 2 - 2*texte_surface->w/3), spacing*i+offset, texte_surface->w, texte_surface->h};
        //background of the text
        SDL_Rect highlight;
        highlight.w = texte_rect.w + 10;
        highlight.h = texte_rect.h + 10;
        highlight.x = texte_rect.x - 5;
        highlight.y = texte_rect.y - 5;
        SDL_RenderFillRect(renderer, &highlight);

        SDL_RenderCopy(renderer, texte_texture, NULL, &texte_rect);
        SDL_FreeSurface(texte_surface);
        SDL_DestroyTexture(texte_texture);

        button->setX(texte_rect.x + texte_rect.w + 50);
        button->setY(texte_rect.y);
        button->draw(window, renderer);
    }
    SDL_RenderPresent(renderer);
}

void HistoryMenu::handleEvent() {
    running = new bool(true);
    SDL_Event event;
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
                    for (auto pair : list_sim) {
                        Button* b = pair.second;
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
                for (auto pair : list_sim) {
                    Button* b = pair.second;
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
        draw();
        SDL_Delay(16);
    }
}

