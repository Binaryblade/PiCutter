#include "gpio_manager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const std::unordered_map<IOIndex, GPIODescription> GPIOManager::GPIO_TABLE = 
	{
		{ IOIndex::P1_7,
		 {	IOIndex::P1_7,
		 	4,
			1,
			7,
			"P1_4"
		 }
		},

		{ IOIndex::P1_11,
		 {	IOIndex::P1_11,
		  	17,
			1,
			11,
			"P1_11"
		 }
		},

		{ IOIndex::P1_12,
		  { 	IOIndex::P1_12,
		  	18,
			1,
			12,
			"P1_12"
		  }
		},
		
		{ IOIndex::P1_15,
		 {	IOIndex::P1_15,
		  	22,
			1,
			15,
			"P1_15"
		 }
		},

		{ IOIndex::P1_16,
		  { 	IOIndex::P1_16,
		  	23,
			1,
			16,
			"P1_16"
		  }
		},
		
		{ IOIndex::P1_18,
		 {	IOIndex::P1_18,
		  	24,
			1,
			18,
			"P1_18"
		 }
		},

		{ IOIndex::P1_22,
		  { 	IOIndex::P1_22,
		  	25,
			1,
			22,
			"P1_22"
		  }
		},
		
		{ IOIndex::P1_13,
		 {	IOIndex::P1_13,
		  	27,
			1,
			13,
			"P1_13"
		 }
		},

		{ IOIndex::P5_5,
		  { 	IOIndex::P5_5,
		  	30,
			5,
			5,
			"P5_5"
		  }
		},
		
		{ IOIndex::P5_6,
		 {	IOIndex::P5_6,
		  	31,
			5,
			6,
			"P5_6"
		 }
		}
};

const std::string GPIO_PATH = "/sys/class/gpio/";
const std::string EXPORT_PATH = GPIO_PATH + "export";
const std::string UNEXPORT_PATH = GPIO_PATH + "unexport";

GPIOManager::GPIOManager() {
	for(auto i=GPIO_TABLE.begin();i != GPIO_TABLE.end(); ++i) {
		export_gpio(i->second);
		setup_gpio(i->second);
	}
}

GPIOManager::~GPIOManager() {
	for(auto i=GPIO_TABLE.begin();i != GPIO_TABLE.end(); ++i) {
		unexport_gpio(i->second);
	}
}

GPIOChangedSig& GPIOManager::gpio_changed() {
	return changed_signal;
}

void GPIOManager::export_gpio(const GPIODescription& gpio) {
	std::ofstream export_file(EXPORT_PATH);
	export_file << static_cast<uint16_t>(gpio.gpio_id) << std::endl;
}

void GPIOManager::unexport_gpio(const GPIODescription& gpio) {
	std::ofstream unexport_file(UNEXPORT_PATH);
	unexport_file << static_cast<uint16_t>(gpio.gpio_id) << std::endl;
}

void GPIOManager::setup_gpio(const GPIODescription& gpio) {
	GPIOFiles fd_obj;
	std::stringstream ss;
	ss << GPIO_PATH;
	ss << "gpio";
	ss << static_cast<uint16_t>(gpio.gpio_id) << "/";
	std::string directory = ss.str();
	
	fd_obj.value_fd = open((directory + "value").c_str(),O_RDWR);
	fd_obj.edge_fd = open((directory + "edge").c_str(), O_WRONLY);
	fd_obj.direction_fd = open((directory + "direction").c_str(), O_WRONLY);
	fd_obj.active_low_fd = open((directory + "active_low").c_str(), O_WRONLY);
	
	if(write(fd_obj.direction_fd, "in", sizeof("in")) != sizeof("in")) {
		std::cout << "write failed" << std::endl;
	}

	if(write(fd_obj.edge_fd, "both", sizeof("both")) != sizeof("both")) {
		std::cout << "write failed" << std::cout;
	}
	auto mem_function = sigc::mem_fun(*this, &GPIOManager::io_changed);
	auto callback = sigc::bind<1>(mem_function, gpio.id);
	Glib::signal_io().connect(callback, fd_obj.value_fd, Glib::IO_PRI | Glib::IO_ERR);
	fd_table.insert(std::make_pair(gpio.id, fd_obj));
}


bool GPIOManager::io_changed(Glib::IOCondition, IOIndex index) {
	lseek(fd_table[index].value_fd,0,SEEK_SET);
	char read_value[2];
	if(!read(fd_table[index].value_fd,read_value,1)) {
	 std::cout << "what the fuck" << std::endl;
	}
	GPIOState temp;
	if(read_value[0] != '1' && read_value[0] != '0') {
		std::cout << "well damn" << std::endl;
	}
	temp.value = (read_value[0] == '1');
	temp.direction = false;
	temp.active_low = false;
	changed_signal.emit(GPIO_TABLE.find(index)->second, temp);
	return true;
};
