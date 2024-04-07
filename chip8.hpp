#pragma once
#include <stdint.h>
#include <cstdio>
#include <assert.h>

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;

class chip8 {
	private:
	    uint8_t memory[4096];
	    uint8_t V[16];
	    uint16_t index;
	    uint16_t pc;
	    uint8_t delay_timer;
	    uint8_t sound_timer;
	    uint16_t stack[16];
	    uint8_t sp;

	    void setReg(uint8_t v, uint8_t value) {
			assert(v < 16);
	    	V[v] = value;
	    }
	    uint8_t getReg(uint8_t v) const {
	    	assert(v < 16);
	    	return V[v];
	    }
	    void setIndex(uint16_t v) {
	    	index = v;
	    }
	    void setPC(uint16_t value) {
	    	pc = value;
	    }
	    void advancePC(){
	    	setPC(pc + 2);
	    }
		void push(uint16_t value) {
		    if (sp >= 16) {
		        printf("Stack Overflow\n");
		    } else {
		        stack[sp++] = value;
		    }
		}
		uint16_t pop() {
		    if (sp <= 0) {
		        printf("Stack Underflow\n");
		        return -1;
		    } else {
		        return stack[--sp];
		    }
		}
	    
	    bool getPixel(uint8_t x, uint8_t y) const {
	    	return gfx[x + (64 * y)];
	    }
	    void setPixel(uint8_t x, uint8_t y, bool v) {
	    	gfx[x + (64 * y)] = v;
	    }
	    bool flipPixel(uint8_t x, uint8_t y) {
	    	bool pixel = getPixel(x,y);

	    	setPixel(x,y, !pixel);
	    	
			// if pixel was on before being flipped
			// return true to indicate collision
	    	return pixel ? true : false;
	    }

	public:
		bool gfx[64 * 32];
	    bool key[16];
		bool drawFlag;
		
		bool waitingForKey;
		bool gotKey;
		uint8_t waitKey;
		
		void initialize();
		void loadRom(char *name);
		void emulateCycle();
};
