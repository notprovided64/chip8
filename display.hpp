#pragma once
#include <SDL.h>
#include "chip8.hpp"

SDL_Window* initializeWindow(int width, int height);

SDL_Renderer* initializeRenderer(SDL_Window* window);

void updateDisplay(SDL_Renderer* renderer, bool* pixels, int width, int height, int displayWidth);
