#include "Config.h"
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <SDL_image.h>
#include <vector>

Menu::Menu(SDL_Renderer* r) : renderer(r) {
    font = TTF_OpenFont("../assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Erreur chargement police: " << TTF_GetError() << std::endl;
    }
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

    nbGuerriers = 10;
    selectedOption = 0;
    optionsCount = 5;
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
    // Fond
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
    }

    // Titre
    SDL_Color white = {220, 220, 220, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color yellow = {255, 255, 0, 255}; // Couleur pour la sélection

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Clash of Colosseum", red);
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {150, 20, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTex);

    // Construction dynamique des textes des options
    std::vector<std::string> options;
    options.push_back("Nb Guerriers: " + std::to_string(nbGuerriers));
    options.push_back("Taux Mutation: " + std::to_string(mutationRate) + "%");
    options.push_back("Barres de Vie: " + std::string(showHealthBars ? "OUI" : "NON"));
    options.push_back("Vitesse Tirs: " + speedLabels[speedIndex]);
    options.push_back("Musique: " + std::string(musiqueOn ? "ON" : "OFF"));

    for (int i = 0; i < (int)options.size(); i++) {
        // Si l'option est sélectionnée, on l'affiche en jaune, sinon en blanc
        SDL_Color color = (i == selectedOption ? yellow : white);

        SDL_Surface* surface = TTF_RenderText_Solid(font, options[i].c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Petit rectangle gris derrière la sélection pour bien la voir
        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 150);
            SDL_Rect highlight = {40, 80 + i * 50, 450, surface->h + 10}; // Espacement un peu plus grand
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
            // Navigation Haut / Bas
            case SDL_SCANCODE_UP:
                selectedOption = (selectedOption + optionsCount - 1) % optionsCount;
                break;
            case SDL_SCANCODE_DOWN:
                selectedOption = (selectedOption + 1) % optionsCount;
                break;

            // Modification Valeur Droite
            case SDL_SCANCODE_RIGHT:
                if (selectedOption == 0) { // Nb Guerriers
                    nbGuerriers++;
                }
                else if (selectedOption == 1) { // Taux Mutation
                    if (mutationRate < 100) mutationRate += 5; // Par pas de 5
                }
                else if (selectedOption == 2) { // Barres de Vie
                    showHealthBars = !showHealthBars;
                }
                else if (selectedOption == 3) { // Vitesse Tirs
                    speedIndex = (speedIndex + 1) % 3; // Cycle 0 -> 1 -> 2 -> 0
                }
                else if (selectedOption == 4) { // Musique
                    musiqueOn = !musiqueOn;
                    if (musiqueOn) Mix_PlayMusic(menuMusic, -1);
                    else Mix_HaltMusic();
                }
                break;

            // Modification Valeur Gauche
            case SDL_SCANCODE_LEFT:
                if (selectedOption == 0 && nbGuerriers > 1) {
                    nbGuerriers--;
                }
                else if (selectedOption == 1) { // Taux Mutation
                    if (mutationRate > 0) mutationRate -= 5;
                }
                else if (selectedOption == 2) { // Barres de Vie
                    showHealthBars = !showHealthBars;
                }
                else if (selectedOption == 3) { // Vitesse Tirs
                    speedIndex = (speedIndex + 2) % 3; // Cycle inverse
                }
                else if (selectedOption == 4) { // Musique
                    musiqueOn = !musiqueOn;
                    if (musiqueOn) Mix_PlayMusic(menuMusic, -1);
                    else Mix_HaltMusic();
                }
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
                exit(0); // Quitter brutalement si on ferme la fenêtre
            }
            handleEvent(event);
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                running = false;
            }
        }
    }
}
