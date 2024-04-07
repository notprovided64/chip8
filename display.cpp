#include "display.hpp"

SDL_Window* initializeWindow(int width, int height){
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_Quit();
        return NULL;
    }

    return window;
}


SDL_Renderer* initializeRenderer(SDL_Window* window){
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

	return renderer;
}

void updateDisplay(SDL_Renderer* renderer, bool* pixels, int width, int height, int displayWidth) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

	// change to be based on current running environment
    int squareSize = displayWidth / width;

    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int x = col * squareSize;
            int y = row * squareSize;
            SDL_Rect rect = { x, y, squareSize, squareSize };

            if (pixels[row * width + col]) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_RenderPresent(renderer);
}
