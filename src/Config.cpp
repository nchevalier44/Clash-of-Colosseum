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
    menuMusic = Mix_LoadMUS("../assets/musiqueJeu.mp3");
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

    // Initialisation des options
    nbGuerriers = 5;
    selectedOption = 0;
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
    SDL_Color white = {220, 220, 220, 255};
    SDL_Color red = {255, 0, 0, 255};

    // Titre du menu
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Clash of Colosseum", red);
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {150, 20, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTex);

    // Liste des options
    std::vector<std::string> options = {
        "Nb Guerriers: " + std::to_string(nbGuerriers),
        "Difficulte: " + difficulte,
        std::string("Musique: ") + (musiqueOn ? "ON" : "OFF")
    };

    for (int i = 0; i < (int)options.size(); i++) {
        SDL_Color color = (i == selectedOption ? red : white);

        SDL_Surface* surface = TTF_RenderText_Solid(font, options[i].c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Rectangle highlight derrière l'option sélectionnée
        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 200);
            SDL_Rect highlight = {40, 80 + i * 40, 400, surface->h + 10};
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_Rect rect = {50, 80 + i * 40, surface->w, surface->h};
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
                else if (selectedOption == 2) { // Difficulté
                    if (difficulte == "Facile") difficulte = "Normal";
                    else if (difficulte == "Normal") difficulte = "Difficile";
                    else difficulte = "Facile";
                }
                else if (selectedOption == 3) { // Musique ON/OFF
                    musiqueOn = !musiqueOn;
                    if (musiqueOn) {
                        Mix_PlayMusic(menuMusic, -1);
                    } else {
                        Mix_HaltMusic();
                    }
                }
                break;
            case SDL_SCANCODE_LEFT:
                if (selectedOption == 0 && nbGuerriers > 1) nbGuerriers--;
                // Gauche = inverse des cycles type/difficulte
                else if (selectedOption == 2) {
                    if (difficulte == "Facile") difficulte = "Difficile";
                    else if (difficulte == "Normal") difficulte = "Facile";
                    else difficulte = "Normal";
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
            if (event.type == SDL_QUIT) running = false;
            handleEvent(event);

            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                running = false; // ENTER pour valider
            }
        }
    }
}
