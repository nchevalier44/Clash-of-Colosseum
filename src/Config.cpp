#include "Config.h"
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <SDL_image.h>
#include <vector>

Menu::Menu(SDL_Renderer* r, std::map<std::string, std::string>* parameters) : renderer(r) {
    font = TTF_OpenFont("../assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Erreur chargement police: " << TTF_GetError() << std::endl;
    }

    nbGuerriers = std::stoi((*parameters)["nb_guerriers"]);
    mutationTypeRate = std::stoi((*parameters)["mutation_type_rate"]);
    mutationStatsRate = std::stoi((*parameters)["mutation_stats_rate"]);
    showHealthBars = (*parameters)["show_healthbars"] == "true";
    speedIndex = std::stoi((*parameters)["projectile_speed_multiplier_index"]);
    musiqueOn = (*parameters)["musique_on"] == "true";
    sameTypePeace = (*parameters)["same_type_peace"] == "true";
    selectedOption = 0;
    optionsCount = 7;

    menuMusic = Mix_LoadMUS("../assets/menumusic.mp3");
    if (!menuMusic) {
        std::cerr << "Erreur chargement musique menu: " << Mix_GetError() << std::endl;
    } else if (musiqueOn) {
        Mix_PlayMusic(menuMusic, -1); // -1 = boucle infinie
    }
    // Charger le fond une seule fois
    SDL_Surface* fondSurface = IMG_Load("../assets/menufond.png");
    if (!fondSurface) {
        std::cerr << "Erreur IMG_Load: " << IMG_GetError() << std::endl;
        background = nullptr;
    } else {
        background = SDL_CreateTextureFromSurface(renderer, fondSurface);
        SDL_FreeSurface(fondSurface);

        if (!background) {
            std::cerr << "Erreur SDL_CreateTextureFromSurface (fond): " << SDL_GetError() << std::endl;
        }
    }
}

float Menu::getProjectileSpeedMultiplier() const {
    if (speedIndex == 0) return 0.5f; // Lent
    if (speedIndex == 2) return 2.0f; // Rapide
    return 1.0f;                      // Normal
}

Menu::~Menu() {
    if (background) {
        SDL_DestroyTexture(background);
        background = nullptr;
    }
    if (menuMusic) {
        Mix_HaltMusic(); // stoppe la musique
        Mix_FreeMusic(menuMusic);
        menuMusic = nullptr;
    }
}

void Menu::render() {
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
    }

    SDL_Color white = {220, 220, 220, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Clash of Colosseum", red);
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {150, 20, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTex);

    std::vector<std::string> options;
    options.push_back("Nb Guerriers: " + std::to_string(nbGuerriers));
    options.push_back("Mut. Type (Classe): " + std::to_string(mutationTypeRate) + "%");
    options.push_back("Mut. Stats (Force): " + std::to_string(mutationStatsRate) + "%");
    options.push_back("Barres de Vie: " + std::string(showHealthBars ? "OUI" : "NON"));
    options.push_back("Vitesse Tirs: " + speedLabels[speedIndex]);
    options.push_back("Musique: " + std::string(musiqueOn ? "ON" : "OFF"));
    options.push_back("Paix meme type: " + std::string(sameTypePeace ? "ON" : "OFF")); // Nouvelle option

    for (int i = 0; i < (int)options.size(); i++) {
        SDL_Color color = (i == selectedOption ? yellow : white);
        SDL_Surface* surface = TTF_RenderText_Solid(font, options[i].c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
            SDL_Rect highlight = {40, 80 + i * 50, 450, surface->h + 10};
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_Rect rect = {50, 85 + i * 50, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

void Menu::handleEvent(SDL_Event& event) {
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
                else if (selectedOption == 1) { if (mutationTypeRate < 100) mutationTypeRate += 5; }
                else if (selectedOption == 2) { if (mutationStatsRate < 100) mutationStatsRate += 5; } // Nouveau
                else if (selectedOption == 3) showHealthBars = !showHealthBars;
                else if (selectedOption == 4) speedIndex = (speedIndex + 1) % 3;
                else if (selectedOption == 5) { musiqueOn = !musiqueOn; if(musiqueOn) Mix_PlayMusic(menuMusic,-1); else Mix_HaltMusic(); }
                else if (selectedOption == 6) sameTypePeace = !sameTypePeace;
                break;

            case SDL_SCANCODE_LEFT:
                if (selectedOption == 0 && nbGuerriers > 1) nbGuerriers--;
                else if (selectedOption == 1) { if (mutationTypeRate > 0) mutationTypeRate -= 5; }
                else if (selectedOption == 2) { if (mutationStatsRate > 0) mutationStatsRate -= 5; } // Nouveau
                else if (selectedOption == 3) showHealthBars = !showHealthBars;
                else if (selectedOption == 4) speedIndex = (speedIndex + 2) % 3;
                else if (selectedOption == 5) { musiqueOn = !musiqueOn; if(musiqueOn) Mix_PlayMusic(menuMusic,-1); else Mix_HaltMusic(); }
                else if (selectedOption == 6) sameTypePeace = !sameTypePeace;
                break;
            default: break;
        }
    }
}


void Menu::configureParameters() {
    bool running = true;
    SDL_Event event;
    while (running) {
        render();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                exit(0);
            }
            handleEvent(event);
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                running = false;
            }
        }
    }
}
