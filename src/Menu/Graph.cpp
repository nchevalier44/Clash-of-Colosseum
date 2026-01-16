#include "Graph.h"
#include "GameMenu.h"

#include <iostream>
#include <ostream>
#include <sstream>
#include <iomanip>

Graph::Graph(int x, int y, int w, int h, SDL_Renderer* renderer) : renderer(renderer) {
    label_font = TTF_OpenFont("../assets/arial.ttf", 14);
    // Empêche de créer un graphique trop petit
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
    // Fond blanc
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &graph_rect);

    // Contour noir
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &graph_rect);

    // Ordre d'affichage (calques)
    drawGraduations();
    drawCurves();
    drawLabels();
    drawAxes();
    drawLegend();
}

void Graph::drawGraduations() {
    if (series.empty()) return;

    SDL_Color textColor = {0, 0, 0, 255};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gris pour les traits de grille

    // Recherche du Maximum Global pour l'échelle verticale
    float maxVal = 0;
    size_t maxPoints = 0;
    for (const auto& s : series) {
        for (float v : s.data) if (v > maxVal) maxVal = v;
        if (s.data.size() > maxPoints) maxPoints = s.data.size();
    }
    if (maxVal == 0) maxVal = 1;
    if (maxPoints < 2) maxPoints = 2; // Évite division par zero si une seule génération

    // On définit la zone utile de dessin (pour ne pas écrire sur les bords ou les flèches)
    int originX = graph_rect.x + padding;
    int originY = graph_rect.y + graph_rect.h - padding;
    int axisHeight = graph_rect.h - padding - (padding / 2) - arrow_size - 5;
    int axisWidth  = graph_rect.w - padding - (padding / 2) - arrow_size - 5;

    //Graduations Y
    int nbStepsY = 5; // On veut 5 lignes horizontales

    for (int i = 0; i <= nbStepsY; i++) {
        float value = maxVal * ((float)i / nbStepsY);
        int yPos = originY - (int)((value / maxVal) * axisHeight);
        SDL_RenderDrawLine(renderer, originX - 5, yPos, originX, yPos);

        if (i > 0) {
            // Utilisation de stringstream pour afficher proprement les nombres à virgule
            std::string txt;

            if (maxVal < 2.0f) {
                txt = roundingFloatToString(value, 2); // ex: 0.25 (Précis)
            }
            else if (maxVal < 10.0f) {
                txt = roundingFloatToString(value, 1); // ex: 2.5 (Moyen)
            }
            else {
                txt = roundingFloatToString(value, 0); // ex: 150 (Entier)
            }

            // Rendu du texte à gauche de l'axe
            SDL_Surface* surf = TTF_RenderText_Blended(label_font, txt.c_str(), textColor);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect r = { originX - 10 - surf->w, yPos - surf->h / 2, surf->w, surf->h };
                SDL_RenderCopy(renderer, tex, NULL, &r);
                SDL_FreeSurface(surf); SDL_DestroyTexture(tex);
            }
        }
    }


    //Graduations axe X
    // On veut afficher environ 5 à 10 graduations max pour ne pas surcharger
    int targetSteps = 10;

    // Calcul du "step" (saut) : si on a 100 points et qu'on veut 10 labels, on affiche 1 point sur 10.
    int step = 1;
    if (maxPoints > targetSteps) {
        step = (maxPoints + targetSteps - 1) / targetSteps; // Division entière arrondie au supérieur
    }

    // On parcourt les générations par bonds de 'step'
    for (size_t i = 0; i < maxPoints; i += step) {

        // Position relative (0.0 à 1.0) sur l'axe X
        float percentage = (float)i / (float)(maxPoints - 1);
        // Position absolue en pixels
        int xPos = originX + (int)(percentage * axisWidth);

        // Dessiner le petit trait vertical
        SDL_RenderDrawLine(renderer, xPos, originY, xPos, originY + 5);
        // Dessiner le texte (Numéro de génération)
        std::string txt = std::to_string(i+1); // +1 car on commence la génération à 1
        SDL_Surface* surf = TTF_RenderText_Blended(label_font, txt.c_str(), textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            // Centré sous le trait
            SDL_Rect r = { xPos - surf->w / 2, originY + 8, surf->w, surf->h };
            SDL_RenderCopy(renderer, tex, NULL, &r);
            SDL_FreeSurface(surf); SDL_DestroyTexture(tex);
        }
    }
}

void Graph::drawCurves() {
    if (series.empty()) return;

    // On cherche la valeur maximale parmi toutes les séries pour caler l'échelle
    float maxVal = 0;
    for (const auto& s : series) {
        for (float v : s.data) {
            if (v > maxVal) maxVal = v;
        }
    }
    // Évite la division par zéro si toutes les valeurs sont à 0
    if (maxVal == 0) maxVal = 1.0f;

    // Coordonnées de l'origine du graphe (Bas-Gauche)
    int startX = graph_rect.x + padding;
    int startY = graph_rect.y + graph_rect.h - padding;

    // Dimensions utiles pour le tracé
    int usefulWidth  = graph_rect.w - padding - (padding / 2) - arrow_size - 5;
    int usefulHeight = graph_rect.h - padding - (padding / 2) - arrow_size - 5;

    //On dessine chaque série/courbe
    for (const auto& s : series) {
        if (s.data.size() < 2) continue; // Pas de ligne avec 1 seul point

        SDL_SetRenderDrawColor(renderer, s.color.r, s.color.g, s.color.b, s.color.a);

        // Ecart horizontal entre deux points consécutifs
        float stepX = (float)usefulWidth / (float)(s.data.size() - 1);

        // On relie chaque point i au point i+1
        for (size_t i = 0; i < s.data.size() - 1; i++) {
            int x1 = startX + (int)(i * stepX);
            int y1 = startY - (int)((s.data[i] / maxVal) * usefulHeight);

            int x2 = startX + (int)((i + 1) * stepX);
            int y2 = startY - (int)((s.data[i+1] / maxVal) * usefulHeight);

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

    //Axe X
    SDL_Surface* surface = TTF_RenderUTF8_Blended(label_font, x_label.c_str(), {0, 0, 0});
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {
            end_x -arrow_size - surface->w/2,
            origin_y - surface->h - arrow_size,
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    //Axe Y
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

    // Position de départ : Coin Haut-Droit du graphique
    int startX = graph_rect.x + graph_rect.w - 5;
    int startY = graph_rect.y + 5;

    // On liste toutes les courbes présentes
    for (const auto& s : series) {
        // Rendu du texte avec la couleur spécifique de la série
        SDL_Surface* surface = TTF_RenderUTF8_Blended(label_font, s.name.c_str(), s.color);

        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {
                startX - surface->w,
                startY,
                surface->w,
                surface->h
            };

            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            // On décale vers le bas pour le prochain élément de la légende
            startY += surface->h + 5;
        }
    }
}

void Graph::clearSeries() {
    series.clear();
}