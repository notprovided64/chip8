#include <iostream>

int main() {

	uint8_t number = 153;
	
	uint8_t hundreds = number / 100;
	uint8_t tens = number / 10 % 10;
	uint8_t ones = number % 10;	

	printf("%d\n", hundreds);
	printf("%d\n", tens);
	printf("%d\n", ones);
    return 0;
}
