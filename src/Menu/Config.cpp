#include "./Config.h"
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <SDL_image.h>
#include <vector>

Menu::Menu(SDL_Renderer* r, std::map<std::string, std::string>* parameters, bool* keep_playing) : renderer(r), keep_playing(keep_playing) {
    font = TTF_OpenFont("../assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Erreur chargement police: " << TTF_GetError() << std::endl;
    }

    nbGuerriers = std::stoi((*parameters)["nb_guerriers"]);
    mutationTypeRate = std::stoi((*parameters)["mutation_type_rate"]);
    mutationStatsRate = std::stoi((*parameters)["mutation_stats_rate"]);
    showHealthBars = (*parameters)["show_healthbars"] == "1";
    speedIndex = std::stoi((*parameters)["projectile_speed_multiplier_index"]);
    musiqueOn = (*parameters)["music"] == "1";
    sameTypePeace = (*parameters)["same_type_peace"] == "1";
    min_number_entity = std::stoi((*parameters)["min_number_entity"]);
    selectedOption = 0;
    optionsCount = 8;
}

float Menu::getProjectileSpeedMultiplier() const {
    if (speedIndex == 0) return 0.5f; // Lent
    if (speedIndex == 2) return 2.0f; // Rapide
    return 1.0f;                      // Normal
}

Menu::~Menu() {
}

void Menu::render(SDL_Texture* background) {
    //Background
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    SDL_Color white = {220, 220, 220, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    std::vector<std::string> options;
    options.push_back("Nb Guerriers: " + std::to_string(nbGuerriers));
    options.push_back("Min number entity: " + std::to_string(min_number_entity));
    options.push_back("Mut. Type (Classe): " + std::to_string(mutationTypeRate) + "%");
    options.push_back("Mut. Stats (Force): " + std::to_string(mutationStatsRate) + "%");
    options.push_back("Barres de Vie: " + std::string(showHealthBars ? "OUI" : "NON"));
    options.push_back("Vitesse Tirs: " + speedLabels[speedIndex]);
    options.push_back("Musique: " + std::string(musiqueOn ? "ON" : "OFF"));
    options.push_back("Paix meme type: " + std::string(sameTypePeace ? "ON" : "OFF"));

    // Calculs pour le centrage VERTICAL global
    int itemSpacing = 50;
    int totalMenuHeight = options.size() * itemSpacing;
    int startY = (winH - totalMenuHeight) / 2; // (Hauteur Fenêtre - Hauteur Menu) / 2

    // Largeur de la barre de sélection (pour qu'elle soit uniforme)
    int highlightWidth = 500;

    for (int i = 0; i < (int)options.size(); i++) {
        SDL_Color color = (i == selectedOption ? yellow : white);
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font, options[i].c_str(), color);

        if (!surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Position Y de l'élément courant
        int currentY = startY + (i * itemSpacing);

        // Dessin de la barre de sélection (centrée horizontalement)
        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
            SDL_Rect highlight;
            highlight.w = highlightWidth;
            highlight.h = surface->h + 10;
            highlight.x = (winW - highlightWidth) / 2; // Centrage X de la barre
            highlight.y = currentY - 5;
            SDL_RenderFillRect(renderer, &highlight);
        }

        // Dessin du texte (centré horizontalement)
        SDL_Rect rect;
        rect.w = surface->w;
        rect.h = surface->h;
        rect.x = (winW - surface->w) / 2; // Centrage X du texte
        rect.y = currentY;

        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

void Menu::handleEvent(SDL_Event& event, Mix_Music* music) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                selectedOption = (selectedOption + optionsCount - 1) % optionsCount;
                break;
            case SDL_SCANCODE_DOWN:
                selectedOption = (selectedOption + 1) % optionsCount;
                break;

            case SDL_SCANCODE_RIGHT:
                if (selectedOption == 0) nbGuerriers++;
                else if (selectedOption == 1) { min_number_entity++; }
                else if (selectedOption == 2) { if (mutationTypeRate < 100) mutationTypeRate += 5; }
                else if (selectedOption == 3) { if (mutationStatsRate < 100) mutationStatsRate += 5; } // Nouveau
                else if (selectedOption == 4) showHealthBars = !showHealthBars;
                else if (selectedOption == 5) speedIndex = (speedIndex + 1) % 3;
                else if (selectedOption == 6) { musiqueOn = !musiqueOn; if(musiqueOn) Mix_PlayMusic(music,-1); else Mix_HaltMusic(); }
                else if (selectedOption == 7) sameTypePeace = !sameTypePeace;
                break;

            case SDL_SCANCODE_LEFT:
                if (selectedOption == 0 && nbGuerriers > 2) nbGuerriers--;
                else if (selectedOption == 1) { if (min_number_entity > 2) min_number_entity--; }
                else if (selectedOption == 2) { if (mutationTypeRate > 0) mutationTypeRate -= 5; }
                else if (selectedOption == 3) { if (mutationStatsRate > 0) mutationStatsRate -= 5; } // Nouveau
                else if (selectedOption == 4) showHealthBars = !showHealthBars;
                else if (selectedOption == 5) speedIndex = (speedIndex + 2) % 3;
                else if (selectedOption == 6) { musiqueOn = !musiqueOn; if(musiqueOn) Mix_PlayMusic(music,-1); else Mix_HaltMusic(); }
                else if (selectedOption == 7) sameTypePeace = !sameTypePeace;
                break;
            default: break;
        }
    }
}


void Menu::configureParameters(SDL_Window* window, Mix_Music* music, SDL_Texture* background) {
    bool running = true;
    this->window = window;

    SDL_Event event;
    while (running) {
        render(background);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                *keep_playing = false;
            }
            handleEvent(event, music);
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                running = false;
            }
        }
    }
}
