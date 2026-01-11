#include "Graph.h"

#include <iostream>
#include <ostream>

Graph::Graph(int x, int y, int w, int h, SDL_Renderer* renderer) : renderer(renderer) {
    label_font = TTF_OpenFont("../assets/arial.ttf", 14);
    if (w < 100) w = 100;
    if (h < 100) h = 100;
    graph_rect.x = x;
    graph_rect.y = y;
    graph_rect.w = w;
    graph_rect.h = h;
}

void Graph::setLabels(std::string x, std::string y) {
    this->x_label = x;
    this->y_label = y;
}

Graph::~Graph() {
    TTF_CloseFont(label_font);
}

void Graph::addSerie(Serie serie) {
    series.push_back(serie);
}

void Graph::draw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &graph_rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &graph_rect);
    drawGraduations();
    drawCurves();
    drawLabels();
    drawAxes();
    drawLegend();
    SDL_RenderPresent(renderer);
}

void Graph::drawGraduations() {
    if (series.empty()) return;

    SDL_Color textColor = {0, 0, 0, 255}; // Noir
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gris foncé pour les traits

    // --- 1. CALCUL DU MAX Y ---
    float maxVal = 0;
    size_t maxPoints = 0;
    for (const auto& s : series) {
        for (float v : s.data) if (v > maxVal) maxVal = v;
        if (s.data.size() > maxPoints) maxPoints = s.data.size();
    }
    if (maxVal == 0) maxVal = 1;
    if (maxPoints < 2) maxPoints = 2;

    // --- CONFIGURATION DES MARGES ---
    int originX = graph_rect.x + padding;
    int originY = graph_rect.y + graph_rect.h - padding;

    // C'EST ICI QUE ÇA CHANGE : On réduit la zone utile
    int axisHeight = graph_rect.h - padding - (padding / 2) - arrow_size - 5;
    int axisWidth  = graph_rect.w - padding - (padding / 2) - arrow_size - 5;

    // --- 2. GRADUATIONS AXE Y (Vertical) ---
    int nbStepsY = 5; // On veut 5 graduations (0%, 25%, 50%, 75%, 100%)

    for (int i = 0; i <= nbStepsY; i++) {
        // Calcul de la valeur à afficher (ex: si Max=100, on affiche 0, 20, 40...)
        float value = maxVal * ((float)i / nbStepsY);

        // Position en pixels
        int yPos = originY - (int)((value / maxVal) * axisHeight);

        // A. Dessiner le petit trait (Tick)
        SDL_RenderDrawLine(renderer, originX - 5, yPos, originX, yPos);

        // B. Dessiner le texte (seulement si ce n'est pas 0 pour ne pas surcharger l'angle)
        if (i > 0) {
            std::string txt = std::to_string((int)value); // Cast en int pour éviter "12.000000"
            SDL_Surface* surf = TTF_RenderText_Blended(label_font, txt.c_str(), textColor);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                // On aligne le texte à gauche du trait
                SDL_Rect r = { originX - 10 - surf->w, yPos - surf->h / 2, surf->w, surf->h };
                SDL_RenderCopy(renderer, tex, NULL, &r);
                SDL_FreeSurface(surf); SDL_DestroyTexture(tex);
            }
        }
    }

    // --- 3. GRADUATIONS AXE X (Horizontal) ---
    int nbStepsX = 5; // On veut 5 repères temporels

    for (int i = 0; i <= nbStepsX; i++) {
        // On calcule quel index du tableau correspond à cette étape
        // Ex: si 100 points de données, i=0 -> index 0, i=5 -> index 100
        float percentage = (float)i / nbStepsX;

        // Position en pixels
        int xPos = originX + (int)(percentage * axisWidth);

        // A. Dessiner le petit trait
        SDL_RenderDrawLine(renderer, xPos, originY, xPos, originY + 5);

        // B. Dessiner le texte (Numéro de l'échantillon ou Temps)
        // On affiche l'index approximatif (ex: tour 0, tour 20, tour 40...)
        int valueDisplay = (int)(percentage * (maxPoints - 1));

        std::string txt = std::to_string(valueDisplay);
        SDL_Surface* surf = TTF_RenderText_Blended(label_font, txt.c_str(), textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            // On centre le texte sous le trait
            SDL_Rect r = { xPos - surf->w / 2, originY + 8, surf->w, surf->h };
            SDL_RenderCopy(renderer, tex, NULL, &r);
            SDL_FreeSurface(surf); SDL_DestroyTexture(tex);
        }
    }
}

void Graph::drawCurves() {
    // Sécurité : s'il n'y a pas de données, on ne fait rien
    if (series.empty()) return;

    // 1. TROUVER L'ÉCHELLE (MAX Y)
    // On cherche la valeur maximale parmi toutes les séries pour caler l'échelle
    float maxVal = 0;
    for (const auto& s : series) {
        for (float v : s.data) {
            if (v > maxVal) maxVal = v;
        }
    }
    // Évite la division par zéro si toutes les valeurs sont à 0
    if (maxVal == 0) maxVal = 1.0f;

    int startX = graph_rect.x + padding;
    int startY = graph_rect.y + graph_rect.h - padding;

    // On réduit la zone de dessin
    int usefulWidth  = graph_rect.w - padding - (padding / 2) - arrow_size - 5;
    int usefulHeight = graph_rect.h - padding - (padding / 2) - arrow_size - 5;

    // 3. DESSINER CHAQUE SÉRIE
    for (const auto& s : series) {
        if (s.data.size() < 2) continue;

        SDL_SetRenderDrawColor(renderer, s.color.r, s.color.g, s.color.b, s.color.a);

        // Le pas horizontal est calculé sur la largeur RÉDUITE
        float stepX = (float)usefulWidth / (float)(s.data.size() - 1);

        for (size_t i = 0; i < s.data.size() - 1; i++) {
            int x1 = startX + (int)(i * stepX);
            int y1 = startY - (int)((s.data[i] / maxVal) * usefulHeight); // Hauteur RÉDUITE

            int x2 = startX + (int)((i + 1) * stepX);
            int y2 = startY - (int)((s.data[i+1] / maxVal) * usefulHeight); // Hauteur RÉDUITE

            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }
}

void Graph::drawAxes() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int origin_x = graph_rect.x + padding;
    int origin_y = graph_rect.y + graph_rect.h - padding;
    int end_x = graph_rect.x + graph_rect.w - padding/2;
    int end_y = graph_rect.y + padding/2;

    SDL_RenderDrawLine(renderer, origin_x, origin_y, end_x, origin_y); //Axe X
    SDL_RenderDrawLine(renderer, origin_x, origin_y, origin_x, end_y); //Axe Y

    // Flèche au bout de l'axe X
    SDL_RenderDrawLine(renderer, end_x, origin_y, end_x - arrow_size, origin_y - arrow_size / 2);
    SDL_RenderDrawLine(renderer, end_x, origin_y, end_x - arrow_size, origin_y + arrow_size / 2);

    // Flèche au bout de l'axe Y
    SDL_RenderDrawLine(renderer, origin_x, end_y, origin_x - arrow_size / 2, end_y + arrow_size);
    SDL_RenderDrawLine(renderer, origin_x, end_y, origin_x + arrow_size / 2, end_y + arrow_size);
}

void Graph::drawLabels() {
    int origin_x = graph_rect.x + padding;
    int origin_y = graph_rect.y + graph_rect.h - padding;
    int end_x = graph_rect.x + graph_rect.w - padding;
    int end_y = graph_rect.y + padding;

    SDL_Surface* surface = TTF_RenderUTF8_Blended(label_font, x_label.c_str(), {0, 0, 0});
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {
            end_x -arrow_size,
            origin_y - surface->h - arrow_size,
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
    surface = TTF_RenderUTF8_Blended(label_font, y_label.c_str(), {0, 0, 0});
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {
            origin_x + arrow_size,
            end_y - surface->h - arrow_size,
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void Graph::drawLegend() {
    if (series.empty()) return;

    // Position de départ : Coin Haut-Droit (moins le padding)
    int startX = graph_rect.x + graph_rect.w - 5;
    int startY = graph_rect.y + 5;

    // On parcourt chaque série pour afficher son nom
    for (const auto& s : series) {
        // 1. Créer le texte avec LA COULEUR de la série
        SDL_Surface* surface = TTF_RenderUTF8_Blended(label_font, s.name.c_str(), s.color);

        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            // 2. Calculer la position
            // On aligne à DROITE : donc x = point_depart - largeur_texte
            SDL_Rect rect = {
                startX - surface->w,
                startY,
                surface->w,
                surface->h
            };

            // 3. Afficher et nettoyer
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            // 4. Descendre pour le prochain texte (hauteur du texte + 5px de marge)
            startY += surface->h + 5;
        }
    }
}