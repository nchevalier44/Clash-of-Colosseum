#include "Graphics.h"
#include <iostream>

int main() {
    Graphics graphics;

    bool running = true;

    while (running) {
        graphics.update(&running);
        SDL_Delay(16);
    }

    return 0;
}