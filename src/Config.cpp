#include "Config.h"
#include <SDL_ttf.h>
#include <string>
#include <iostream>


Menu::Menu(SDL_Renderer* r) : renderer(r) {

    font = TTF_OpenFont("../assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Erreur chargement police: " << TTF_GetError() << std::endl;
    }
}

Menu::~Menu() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}

void Menu::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // fond noir
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    // Affiche le menu
    std::string options[2] = {
        "Nb Guerriers: " + std::to_string(nbGuerriers),
        "PV Base: " + std::to_string(pvBase)
    };

    for (int i = 0; i < 2; i++) {
        SDL_Surface* surface = TTF_RenderText_Solid(
            font,
            options[i].c_str(),
            i == selectedOption ? yellow : white
        );

        if (!surface) {  // ⚠️ vérification
            std::cerr << "Erreur TTF_RenderText_Solid: " << TTF_GetError() << std::endl;
            continue; // on saute cet affichage pour éviter le crash
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            std::cerr << "Erreur SDL_CreateTextureFromSurface: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            continue;
        }

        SDL_Rect rect = {50, 50 + i * 50, surface->w, surface->h};
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
                selectedOption = (selectedOption + 1) % 2;
                break;
            case SDL_SCANCODE_DOWN:
                selectedOption = (selectedOption + 2 - 1) % 2; // descend
                break;
            case SDL_SCANCODE_RIGHT:
                if (selectedOption == 0) nbGuerriers++;
                else if (selectedOption == 1) pvBase += 10;
                break;
            case SDL_SCANCODE_LEFT:
                if (selectedOption == 0 && nbGuerriers > 1) nbGuerriers--;
                else if (selectedOption == 1 && pvBase > 10) pvBase -= 10;
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
