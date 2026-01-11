#include "./GameMenu.h"


GameMenu::GameMenu(SDL_Renderer* renderer, SDL_Window* window) : window(window), renderer(renderer){
    font = TTF_OpenFont("../assets/arial.ttf", 16);
    statFont = TTF_OpenFont("../assets/arial.ttf", 14);
    if (!statFont) {
        std::cerr << "Erreur chargement police stats: " << TTF_GetError() << std::endl;
    }
    time_options = {1, 2, 5, 10, 20, 50, 100};
    for(int value : time_options) {
        std::string string_time = "x" + std::to_string(value);

        SDL_Surface *surface = TTF_RenderUTF8_Solid(font, string_time.c_str(), {255, 255, 255});
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        time_textures.push_back(texture);
        SDL_FreeSurface(surface);
    }

    int width_window, height_window;
    SDL_GetWindowSize(window, &width_window, &height_window);

    Button* stats_visible_button = new Button("Hide Stats", font, renderer,  [this](Button* button) {
        hide_stats = !hide_stats;
        if (hide_stats) {
            button->setText("Show Stats");
        } else {
            button->setText("Hide Stats");
        }
    });
    Button* exit_button = new Button("Exit", font, renderer, [this](Button* button) {
        stop_simulation = true; //ensuite dans Graphics::update(), ça arrête tout
    });
    stats_visible_button->setX(4*width_window/9 - stats_visible_button->getRect().w);
    exit_button->setX(5*width_window/9);

    stats_visible_button->setY(16);
    exit_button->setY(16);

    buttons.push_back(stats_visible_button);
    buttons.push_back(exit_button);
}

GameMenu::~GameMenu(){
    for(SDL_Texture* texture : time_textures){
        SDL_DestroyTexture(texture);
    }
    if (statFont) TTF_CloseFont(statFont);
    if (font) TTF_CloseFont(font);
    for (Button* b : buttons) {
        delete b;
    }
}

void GameMenu::displayEndSimulation(std::pair<std::string, int> pair, int generation) {

    //Background
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    int rect_width = 450;
    int rect_height = 150;
    SDL_Rect back_rect = {window_width/2 - rect_width/2, window_height/2 - rect_height/2, rect_width, rect_height};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 230);
    SDL_RenderFillRect(renderer, &back_rect);

    //Title

    TTF_Font* title_font = TTF_OpenFont("../assets/arial.ttf", 24);
    SDL_Surface* title_surface = TTF_RenderUTF8_Solid(title_font, "Fin de la simulation", {255, 255, 255});
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(renderer, title_surface);
    int width_title, height_title;
    SDL_QueryTexture(title_texture, nullptr, nullptr, &width_title, &height_title);
    SDL_Rect title_rect = {int(back_rect.x + back_rect.w/2 - width_title/2), int(back_rect.y + 0.25*back_rect.h - height_title/2), width_title, height_title};
    SDL_RenderCopy(renderer, title_texture, nullptr, &title_rect);
    SDL_FreeSurface(title_surface);
    TTF_CloseFont(title_font);
    SDL_DestroyTexture(title_texture);

    //Text
    std::vector<std::string> lines;
    lines.push_back("Le type " + pair.first + " à gagné avec " + std::to_string(pair.second) + " entitées restantes.");
    lines.push_back("Ils leur aura fallu " + std::to_string(generation) + " générations pour gagner.");
    lines.push_back("Appuyez sur entrée pour revenir au menu principal.");
    int i = 0; // Pour la position en hauteur de chaque ligne
    for (auto line : lines) {
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font, line.c_str(), {255, 255, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        //i*(height+5) est pour mettre chaque ligne en dessous de l'autre avec un interligne de 5 pixels
        SDL_Rect rect = {int(back_rect.x + back_rect.w/2 - width/2), int(back_rect.y + 0.5*back_rect.h - height/2 + i*(height+5)), width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        i++;
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

void GameMenu::draw(const std::vector<Entity*>& entities, int generation, bool is_game_paused){
    createBackground();
    displayTimeSpeed(is_game_paused);
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    buttons[0]->setX(4*window_width/9 - buttons[0]->getRect().w);
    buttons[1]->setX(5*window_width/9);

    buttons[0]->setY(16);
    buttons[1]->setY(16);

    for (Button* b : buttons) {
        b->draw(window, renderer);
    }
    drawStatsTable(entities, generation);
    drawEntityStats();
}

void GameMenu::drawEntityStats() {
    if (!statFont) return;
    if (!selected_entity) return;
    std::vector<std::string> lines;
    lines.push_back("Type : " + selected_entity->getType());
    lines.push_back("Age : " + std::to_string(selected_entity->getAge()));
    lines.push_back("Hp : " + std::to_string(selected_entity->getHp()));
    lines.push_back("Max Hp : " + std::to_string(selected_entity->getMaxHp()));
    lines.push_back("Range : " + std::to_string(selected_entity->getWeapon()->getRange()));
    lines.push_back("Damage : " + std::to_string(selected_entity->getWeapon()->getDamage()));
    lines.push_back("Speed : " + roundingFloatToString(selected_entity->getMoveSpeed()));

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    int rectW = 140;
    int rectH = 140;
    int margin = 10;

    SDL_Rect bgRect = {winW - rectW - margin, winH - 3 * rectH + 4*margin, rectW, rectH};

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 230);
    SDL_RenderFillRect(renderer, &bgRect);

    SDL_Color textColor = {255, 255, 255, 255};

    int xOffset = bgRect.x + 10;
    int yOffset = bgRect.y + 5;

    for (const auto& line : lines) {
        SDL_Surface* surf = TTF_RenderText_Blended(statFont, line.c_str(), textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dest = {xOffset, yOffset, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dest);
            yOffset += 18;
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }

}

void GameMenu::drawStatsTable(const std::vector<Entity*>& entities, int generation) {
    if (!statFont || hide_stats) return;

    int nbGuerrier = 0, nbArcher = 0, nbMage = 0, nbGolem = 0;
    float totalHp = 0;
    float avgDamage = 0;
    float avgSpeed = 0;
    float avgAge = 0;

    for (const auto& e : entities) {
        std::string t = e->getType();
        if (t == "Guerrier") nbGuerrier++;
        else if (t == "Archer") nbArcher++;
        else if (t == "Mage") nbMage++;
        else if (t == "Golem") nbGolem++;
        totalHp += e->getMaxHp();
        avgDamage += e->getWeapon()->getDamage();
        avgSpeed += e->getMoveSpeed();
        avgAge += e->getAge();
    }
    float avgHp = entities.empty() ? 0 : totalHp / entities.size();
    avgDamage = entities.empty() ? 0 : avgDamage / entities.size();
    avgSpeed = entities.empty() ? 0 : avgSpeed / entities.size();
    avgAge = entities.empty() ? 0 : avgAge / entities.size();

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    int rectW = 170;
    int rectH = 220;
    int margin = 10;

    SDL_Rect bgRect = {winW - rectW - margin, winH - rectH - margin, rectW, rectH};

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 230);
    SDL_RenderFillRect(renderer, &bgRect);

    std::vector<std::string> lines;
    lines.push_back("Génération: " + std::to_string(generation));
    lines.push_back("Vivants: " + std::to_string(entities.size()));
    lines.push_back("PV Max Moyens: " + std::to_string((int)avgHp));
    lines.push_back("Dégats Moyens: " + std::to_string((int)avgDamage));
    lines.push_back("Vitesse Moyennes: " + roundingFloatToString(avgSpeed));
    lines.push_back("Âge Moyens: " + roundingFloatToString(avgAge));
    lines.push_back("----------------");
    lines.push_back("Guerriers: " + std::to_string(nbGuerrier));
    lines.push_back("Archers:   " + std::to_string(nbArcher));
    lines.push_back("Mages:     " + std::to_string(nbMage));
    lines.push_back("Golems:    " + std::to_string(nbGolem));

    SDL_Color textColor = {255, 255, 255, 255};

    int xOffset = bgRect.x + 10;
    int yOffset = bgRect.y + 5;

    for (const auto& line : lines) {
        SDL_Surface* surf = TTF_RenderUTF8_Blended(statFont, line.c_str(), textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dest = {xOffset, yOffset, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, nullptr, &dest);
            yOffset += 18;
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

void GameMenu::createBackground(){
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    SDL_Rect rect = {0, 0, width, 50};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 230);

    SDL_RenderFillRect(renderer, &rect);
}

void GameMenu::displayTimeSpeed(bool is_game_paused){
    SDL_Texture* texture;
    if (!is_game_paused) {
        texture = time_textures[time_index];
    } else {
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font, "Pause", {255, 255, 255});
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }


    int width_text, height_text, width_window, height_window;
    SDL_GetWindowSize(window, &width_window, &height_window);
    SDL_QueryTexture(texture, nullptr, nullptr, &width_text, &height_text);

    SDL_Rect text_rect = {width_window - 50, 16, width_text, height_text};
    SDL_RenderCopy(renderer, texture, nullptr, &text_rect);
}

void GameMenu::faster(){
    if(time_index < time_options.size()-1){
        time_index += 1;
    }
}

void GameMenu::lower(){
    if(time_index > 0){
        time_index -= 1;
    }
}