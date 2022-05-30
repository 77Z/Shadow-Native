#include <iostream>

// THIS IS SO COOL
//https://stackoverflow.com/a/11622727

int main() {
	extern uint8_t data[] asm("_binary_data_txt_start");
	extern uint8_t data_end[] asm("_binary_data_txt_end");

	std::cout << data << std::endl;

	return 0;
}
