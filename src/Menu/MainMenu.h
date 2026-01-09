#ifndef CLASH_OF_COLOSSEUM_MAINMENU_H
#define CLASH_OF_COLOSSEUM_MAINMENU_H
#include "Button.h"
#include <map>
#include "../Graphics.h"
#include "Config.h"

class MainMenu {
public:
    MainMenu(SDL_Window* window, SDL_Renderer* renderer, Graphics* graphics, std::map<std::string, std::string>* parameters, bool* keep_playing);
    ~MainMenu();
    void draw();

    void drawBackground();

    void handleEvent();
    void initGame(Menu* menu);
private:
    Button* play_button = nullptr;
    Button* settings_button = nullptr;
    Button* exit_button = nullptr;

    SDL_Texture* background = nullptr;
    Mix_Music* menuMusic = nullptr;
    TTF_Font* font = nullptr;
    TTF_Font* title_font = nullptr;
    bool* running = nullptr;
    SDL_Renderer* renderer = nullptr;
    Graphics* graphics = nullptr;
    SDL_Window* window = nullptr;
    std::map<std::string, std::string>* parameters = nullptr;

    std::vector<Button*> buttons;
    bool * keep_playing = nullptr;
};


#endif //CLASH_OF_COLOSSEUM_MAINMENU_H