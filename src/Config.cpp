#include "Config.h"
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <SDL_image.h>

Menu::Menu(SDL_Renderer* r) : renderer(r) {
    font = TTF_OpenFont("../assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Erreur chargement police: " << TTF_GetError() << std::endl;
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
    pvBase = 100;
    dureeCombat = 60;
    selectedOption = 0;
    typeGuerriers = "Guerrier";  // type statique
}

Menu::~Menu() {
    if (background) {
        SDL_DestroyTexture(background);
        background = nullptr;
    }
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void Menu::render() {
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // fallback noir
        SDL_RenderClear(renderer);
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    std::string options[3] = {
        "Nb Guerriers: " + std::to_string(nbGuerriers),
        "PV Guerriers: " + std::to_string(pvBase),
        "Duree combat: " + std::to_string(dureeCombat)
    };

    for (int i = 0; i < 3; i++) {
        SDL_Surface* surface = TTF_RenderText_Solid(
            font,
            options[i].c_str(),
            i == selectedOption ? yellow : white
        );

        if (!surface) {
            std::cerr << "Erreur TTF_RenderText_Solid: " << TTF_GetError() << std::endl;
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "Erreur SDL_CreateTextureFromSurface: " << SDL_GetError() << std::endl;
            continue;
        }

        SDL_Rect rect = {50, 50 + i * 50, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

void Menu::handleEvent(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                selectedOption = (selectedOption + 3 - 1) % 3; // remonte
                break;
            case SDL_SCANCODE_DOWN:
                selectedOption = (selectedOption + 1) % 3; // descend
                break;
            case SDL_SCANCODE_RIGHT:
                if (selectedOption == 0) nbGuerriers++;
                else if (selectedOption == 1) pvBase += 10;
                else if (selectedOption == 2) dureeCombat += 10;
                break;
            case SDL_SCANCODE_LEFT:
                if (selectedOption == 0 && nbGuerriers > 1) nbGuerriers--;
                else if (selectedOption == 1 && pvBase > 10) pvBase -= 10;
                else if (selectedOption == 2 && dureeCombat > 10) dureeCombat -= 10;
                break;
            default:
                break;
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
