#include "MainMenu.h"

#include "HistoryMenu.h"

MainMenu::MainMenu(SDL_Window* window, SDL_Renderer* renderer, Graphics* graphics, std::map<std::string, std::string>* parameters, std::vector<SimulationStats*>* sim_stats, bool* keep_playing) : window(window), renderer(renderer), graphics(graphics), parameters(parameters), keep_playing(keep_playing) {
    font = TTF_OpenFont("../assets/arial.ttf", 24);
    title_font = TTF_OpenFont("../assets/arial.ttf", 48);
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    play_button = new Button("Lancer la simulation", font, renderer, [this](Button* button) {
        *running = false;
    });
   settings_button = new Button("Paramètres", font, renderer, [this, parameters, renderer, keep_playing](Button* button) {
       SettingsMenu menu(renderer, this->window, parameters, keep_playing);
       menu.configureParameters(menuMusic, background);
       this->initGame(&menu);
    });
    history_button = new Button("Historique", font, renderer, [this, window, renderer, keep_playing, sim_stats](Button* button) {
        HistoryMenu menu(window, renderer, sim_stats, background, keep_playing);
    });
    exit_button = new Button("Quitter", font, renderer, [](Button* button) {
        IMG_Quit();
        TTF_Quit();
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
        exit(0);
    });

    play_button->setX(window_width/2 - play_button->getRect().w/2);
    play_button->setY(3*window_height/6 - play_button->getRect().h/2);

    settings_button->setX(window_width/2 - settings_button->getRect().w/2);
    settings_button->setY(4*window_height/6 - settings_button->getRect().h/2);

    history_button->setX(window_width/2 - history_button->getRect().w/2);
    history_button->setY(5*window_height/7 - history_button->getRect().h/2);

    exit_button->setX(window_width/2 - exit_button->getRect().w/2);
    exit_button->setY(5*window_height/6 - exit_button->getRect().h/2);

    buttons.push_back(play_button);
    buttons.push_back(settings_button);
    buttons.push_back(history_button);
    buttons.push_back(exit_button);

    //Musique
    menuMusic = Mix_LoadMUS("../assets/menumusic.mp3");
    if (!menuMusic) {
        std::cerr << "Erreur chargement musique menu: " << Mix_GetError() << std::endl;
    } else if ((*parameters)["music"] == "1") {
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
        if (!background) std::cerr << "Erreur SDL_CreateTextureFromSurface (fond): " << SDL_GetError() << std::endl;
    }

    draw();
    handleEvent();
}

void MainMenu::initGame(SettingsMenu* menu) {
    Projectile::globalSpeedMultiplier = menu->getProjectileSpeedMultiplier();
    graphics->setMutationTypeRate(menu->getMutationTypeRate());
    graphics->setMutationStatsRate(menu->getMutationStatsRate());
    graphics->setShowHealthBars(menu->getShowHealthBars());
    graphics->setMinNumberEntity(menu->getMinNumberEntity());
    graphics->setSameTypePeace(menu->getSameTypePeace());

    (*parameters)["nb_guerriers"] = std::to_string(menu->getNbGuerriers());
    (*parameters)["mutation_type_rate"] = std::to_string(menu->getMutationTypeRate());
    (*parameters)["mutation_stats_rate"] = std::to_string(menu->getMutationStatsRate());
    (*parameters)["show_healthbars"] = std::to_string(menu->getShowHealthBars());
    (*parameters)["projectile_speed_multiplier_index"] = std::to_string(menu->getSpeedIndex());
    (*parameters)["music"] = std::to_string(menu->getMusicOn());
    (*parameters)["same_type_peace"] = std::to_string(menu->getSameTypePeace());
    (*parameters)["min_number_entity"] = std::to_string(menu->getMinNumberEntity());
}

MainMenu::~MainMenu() {
    for (Button* b : buttons) {
        delete b;
    }
    TTF_CloseFont(font);
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

void MainMenu::draw() {

    drawBackground();

    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    SDL_Surface* titleSurface = TTF_RenderText_Solid(title_font, "Clash of Colosseum", {255, 255, 255});
    SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {int(window_width/2 - titleSurface->w/2), int(window_height/7 - titleSurface->h/2), titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTex);

    play_button->setX(window_width/2 - play_button->getRect().w/2);
    play_button->setY(3*window_height/7 - play_button->getRect().h/2);

    settings_button->setX(window_width/2 - settings_button->getRect().w/2);
    settings_button->setY(4*window_height/7 - settings_button->getRect().h/2);

    history_button->setX(window_width/2 - history_button->getRect().w/2);
    history_button->setY(5*window_height/7 - history_button->getRect().h/2);

    exit_button->setX(window_width/2 - exit_button->getRect().w/2);
    exit_button->setY(6*window_height/7 - exit_button->getRect().h/2);

    for (Button* b : buttons) {
        b->draw(window, renderer);
    }

    SDL_RenderPresent(renderer);
}

void MainMenu::drawBackground() {
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
    }
}

void MainMenu::handleEvent() {
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

                    //Click on a button ?
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

                //Hover a button = change color ?
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
        draw();
        SDL_Delay(16);
    }
}
