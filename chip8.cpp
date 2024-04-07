#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include "chip8.hpp"

class Chip8Opcode {
	private:
		uint16_t m_op;
	
	public:
		Chip8Opcode(uint16_t op): m_op(op) {}

		uint16_t getOpcode() {
			return m_op;
		}
	
		uint8_t getOperation() const {
			return m_op >> 12;
		}
		uint16_t getNNN() const {
			return m_op & 0x0fff;
		}
		uint8_t getNN() const {
			return m_op & 0x00ff;
		}
		uint8_t getN() const {
			return m_op & 0x000f;
		}
		uint8_t getX() const {
			return (m_op & 0x0f00) >> 8;
		}
		uint8_t getY() const {
			return (m_op & 0x00f0) >> 4;
		}

		std::string dissasemble() const {
			return std::string();
		}
};

uint8_t chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8::initialize() {
	pc    = 0x200;  // Program counter starts at 0x200
	index = 0;      // Reset index register
	sp    = 0;      // Reset stack pointer

	// Clear display
	std::memset(gfx, false, sizeof(gfx));
	// write clear display cmd in display file 

	// Clear stack
	std::memset(stack, 0, sizeof(stack));

	// Clear registers V0-VF
	std::memset(V, 0, sizeof(V));

	// Clear memory
	std::memset(memory, 0, sizeof(memory));

	// Load fontset
	for(int i = 0; i < 80; ++i)
	  memory[i] = chip8_fontset[i];		

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	gotKey = false;
	waitingForKey = false;
	drawFlag = false;
}

void chip8::loadRom(char *name) {
    std::ifstream file(name, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (file.read((char *)buffer.data(), buffer.size())) {
        for(int i = 0; i < size; ++i)
			memory[i + 0x200] = buffer[i];
    }    
}

void chip8::emulateCycle()
{
	// Fetch Opcode
	Chip8Opcode opcode(memory[pc] << 8 | memory[pc + 1]);
	
	// Decode Opcode
	bool updatePC = true;

	//printf("%04X\n", opcode.getOpcode());
	
	switch (opcode.getOperation()) {
		case 0x0: {
				uint8_t nn = opcode.getNN();
				switch (nn) {
					case 0xE0: {
							for (int i = 0; i < (64*32); i++) {
								gfx[i] = false;
							}
						}
						break;
					case 0xEE:
						setPC(pop());
						break;
				}
			}
			break;	
		case 0x1: {
				uint16_t nnn = opcode.getNNN();

				setPC(nnn);
				updatePC = false;
			}
			break;	
		case 0x2: {
				uint16_t nnn = opcode.getNNN();
				
				push(pc);
				setPC(nnn);
				updatePC = false;
			}
			break;	
		case 0x3: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				if (getReg(x) == nn)
					advancePC();
			}
			break;	
		case 0x4: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				if (getReg(x) != nn)
					advancePC();
			}
			break;	
		case 0x5: {
				uint8_t x = opcode.getX();
				uint8_t y = opcode.getY();

				if (getReg(x) == getReg(y))
					advancePC();
			}
			break;	
		case 0x6: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				setReg(x, nn);
			}
			break;	
		case 0x7: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				setReg(x, getReg(x) + nn);
			}
			break;	
		case 0x8: {
				uint8_t x = opcode.getX();
				uint8_t y = opcode.getY();
				uint8_t n = opcode.getN();

				switch (n) {
					case 0x0:
						setReg(x, getReg(y));
						break;
					case 0x1:
						setReg(x, getReg(x) | getReg(y));
						break;
					case 0x2:
						setReg(x, getReg(x) & getReg(y));
						break;
					case 0x3:
						setReg(x, getReg(x) ^ getReg(y));	
						break;
					case 0x4: {
							uint8_t sum = getReg(x) + getReg(y);
							bool carry = sum < getReg(x);
							
							setReg(x, sum);
							setReg(0xF, carry ? 1 : 0);

						}
						break;
					case 0x5: {
							uint8_t dif = getReg(x) - getReg(y);
							bool borrow = getReg(y) > getReg(x);
							
							setReg(x, dif);
							setReg(0xF, borrow ? 0 : 1);
						}
						break;
					case 0x6:
						setReg(0xF, getReg(x) & 1);
						setReg(x, getReg(x) >> 1);
						break;
					case 0x7: {
							uint8_t dif = getReg(y) - getReg(x);
							bool borrow = getReg(x) > getReg(y);
							
							setReg(x, dif);
							setReg(0xF, borrow ? 0 : 1);
						}
						break;
					case 0xE:
						setReg(0xF, (getReg(x) >> 7) & 1);
						setReg(x, getReg(x) << 1);

						break;
				}
			}
			break;	
		case 0x9: {
				uint8_t x = opcode.getX();
				uint8_t y = opcode.getY();

				if (getReg(x) != getReg(y))
					advancePC();
			}
			break;	
		case 0xA: {
				setIndex(opcode.getNNN());
			}
			break;	
		case 0xB: {
			uint16_t nnn = opcode.getNNN();

			setPC(V[0] + nnn);
			updatePC = false;
			}
			break;	
		case 0xC: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				uint8_t randValue = rand() % 256;

				setReg(x, randValue & nn);
			}	
			break;	
		case 0xD: {
				// TODO make sure this works
				// EDIT seems to work in most cases
				uint8_t x = getReg(opcode.getX());
				uint8_t y = getReg(opcode.getY());
				uint8_t n = opcode.getN();

				//printf("%d\n", y);

				bool collision = false;

				for (int h = 0; h < n; h++) {
					// loads row of bitcoded sprite from the address at index
					// offset according to the current row
					uint8_t spriteRow = memory[index + h];
					
					for (int w = 0; w < 8; w++) {

						// gets the bit specified by the current p
						bool pixelInSprite = (spriteRow >> (7 - w)) & 1;
					
						if(pixelInSprite) {
							if (flipPixel(x+w, y+h))
								collision = true;
						}
					}
				} 				
				setReg(0xF, collision ? 1 : 0);
				drawFlag = true;
			}
			break;	
		case 0xE: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				//make sure key value is between 0-F
				assert(getReg(x) < 0x10);
				
				switch (nn) {
					case 0x9E:
						if (key[getReg(x)]) {
							advancePC();
						}
						break;
					case 0xA1:
						if (!key[getReg(x)]) {
							advancePC();
						}
						break;
				}
			}
			break;	
		case 0xF: {
				uint8_t x = opcode.getX();
				uint8_t nn = opcode.getNN();

				switch (nn) {
					case 0x07: {
						setReg(x, delay_timer);	
						}
						break;
					case 0x0A: {
							if(gotKey) {
				            	setReg(x, waitKey);
				            	
				                gotKey = false;
				                waitingForKey = false;
						    } else {
						        waitingForKey = true;
						        updatePC = false;
						    }
						}
						break;
					case 0x15:
						delay_timer = getReg(x);	
						break;
					case 0x18:
						sound_timer = getReg(x);
						break;
					case 0x1E:
						setIndex(index + getReg(x));
						break;
					case 0x29: {
						//set I to the hex sprite for the lowest nibble in vX						
							uint8_t key = 0x0F & getReg(x);
							
							setIndex(0 + key*5);
						}
						break;
					case 0x33: {
							// bcd
							int xValue = getReg(x); // 154
							uint8_t hundreds = xValue / 100;
							uint8_t tens = xValue / 10 % 10;
							uint8_t ones = xValue % 10;	

							memory[index] = hundreds;
							memory[index+1] = tens;
							memory[index+2] = ones;
						}
						break;
					case 0x55: {
						// Stores from V0 to VX (including VX) in memory
						// starting at address I.
						
						// The offset from I is increased by 1 for each value written
						// but I itself is left unmodified
						for (int i = 0; i <= x; i++)
							memory[index + i] = getReg(i);	
						}
						break;
					case 0x65: {
						// Fills from V0 to VX (including VX) with 
						// values from memory starting at address I
						for (int i = 0; i <= x; i++)
							setReg(i, memory[index + i]);
						}
						break;
				}
			}
			break;
	}
	if (updatePC) {
		advancePC();
	}

	// Update timers
}


