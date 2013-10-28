#include "gpio_manager.h"
#include "spi_manager.h"
#include "main_loop.h"
#include <glibmm.h>
#include <iostream>


void pin_handler(GPIODescription gpio, GPIOState state) {
	std::string state_str = (state.value)?"HIGH":"LOW";
	std::cout << "GPIO: " << gpio.desc << " IN STATE " << state_str << std::endl;
}

int main(void) {
	data_pack_t data;
	std::vector<uint8_t> stuff;
	stuff.push_back(0x55);
	stuff.push_back('U');
	data.push_back(stuff);
//	GPIOManager gpios;
//	gpios.gpio_changed().connect(&pin_handler);
//	MainLoop::run();
	SPIManager spi_bus;
	data_pack_t returnvals = spi_bus.do_transfer(0,data);
	if(returnvals == data) 
		std::cout << "sweet" << std::endl;
	return 0;
}
