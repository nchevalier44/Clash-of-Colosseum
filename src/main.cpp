#include "Graphics.h"
#include <iostream>

int main() {
    Graphics graphics;

    bool running = true;

    while (running) {
        graphics.update(&running);
    }

    return 0;
}