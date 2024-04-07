#include <iostream>
#include <SDL.h>
#include "chip8.hpp"
#include "display.hpp"

#include <chrono>
#include <thread>

const int SCALE = 10;
const int WINDOW_WIDTH = SCREEN_WIDTH * SCALE;
const int WINDOW_HEIGHT = SCREEN_HEIGHT * SCALE;

chip8 chip;

void printKeyState() {
	printf("%c %c %c %c\n",(chip.key[0x1] == true ? 'o' : 'x')
				 	      ,(chip.key[0x2] == true ? 'o' : 'x')
					      ,(chip.key[0x3] == true ? 'o' : 'x')
				   	      ,(chip.key[0xC] == true ? 'o' : 'x'));

}

bool handleEvents() {
	static SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN: {
				uint8_t key;
				switch (event.key.keysym.sym) {
			        case SDLK_1:
			            key = 0x1;
			            break;
			        case SDLK_2:
			            key = 0x2;
			            break;
			        case SDLK_3:
			            key = 0x3;
			            break;
			        case SDLK_4:
			            key = 0xC;
			            break;
			        case SDLK_q:
			            key = 0x4;
			            break;
			        case SDLK_w:
			            key = 0x5;
			            break;
			        case SDLK_e:
			            key = 0x6;
			            break;
			        case SDLK_r:
			            key = 0xD;
			            break;
			        case SDLK_a:
			            key = 0x7;
			            break;
			        case SDLK_s:
			            key = 0x8;
			            break;
			        case SDLK_d:
			            key = 0x9;
			            break;
			        case SDLK_f:
			            key = 0xE;
			            break;
			        case SDLK_z:
			            key = 0xA;
			            break;
			        case SDLK_x:
			            key = 0x0;
			            break;
			        case SDLK_c:
			            key = 0xB;
			            break;
			        case SDLK_v:
			            key = 0xF;
			            break;
			        default:
			        	continue;
			    }			    
			    // Set the corresponding value in chip.key to true using the calculated key
			   chip.key[key] = true;
			    
			    if (chip.waitingForKey) {
			    	chip.waitKey = key;
			    	chip.gotKey = true;
			    }
			    printKeyState();
			    }
			    break;
			case SDL_KEYUP:
		        switch (event.key.keysym.sym) {
		            case SDLK_1:
		                chip.key[0x1] = false;
		                break;
		            case SDLK_2:
		                chip.key[0x2] = false;
		                break;
		            case SDLK_3:
		                chip.key[0x3] = false;
		                break;
		            case SDLK_4:
		                chip.key[0xC] = false;
		                break;
		            case SDLK_q:
		                chip.key[0x4] = false;
		                break;
		            case SDLK_w:
		                chip.key[0x5] = false;
		                break;
		            case SDLK_e:
		                chip.key[0x6] = false;
		                break;
		            case SDLK_r:
		                chip.key[0xD] = false;
		                break;
		            case SDLK_a:
		                chip.key[0x7] = false;
		                break;
		            case SDLK_s:
		                chip.key[0x8] = false;
		                break;
		            case SDLK_d:
		                chip.key[0x9] = false;
		                break;
		            case SDLK_f:
		                chip.key[0xE] = false;
		                break;
		            case SDLK_z:
		                chip.key[0xA] = false;
		                break;
		            case SDLK_x:
		                chip.key[0x0] = false;
		                break;
		            case SDLK_c:
		                chip.key[0xB] = false;
		                break;
		            case SDLK_v:
		                chip.key[0xF] = false;
		                break;
			        default:
			        	continue;
		        }
		        break;
		    }
	}
	return true;
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

	SDL_Window* window = initializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Renderer* renderer = initializeRenderer(window);

    chip.initialize();
    chip.loadRom(argv[1]);

    while (true) { 
    // handle running speed with sdl
    // this will allow for easy timers
    	if (!handleEvents())
    		break;

		chip.emulateCycle();
		if(chip.drawFlag) {
			updateDisplay(renderer, chip.gfx, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_WIDTH);
			chip.drawFlag = false;
		}

		updateDisplay(renderer, chip.gfx, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_WIDTH);

		// implement some sort of pause for timing
		// std::chrono::milliseconds duration(10);
    	// std::this_thread::sleep_for(duration);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
