#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spi_manager.h"

SPIManager::SPIManager() { 
	for(int i=0;i<2;++i) {
		fds[i] = open(SPI_DEVICES[i].c_str(), O_RDWR);
	}
}

SPIManager::~SPIManager() {
	for(int i=0; i<2; ++i) {
		close(fds[i]);
	}
}

data_pack_t SPIManager::do_transfer(int device, const data_pack_t& data) {
	if(device > 1 || device < 0) {
		//some some error flagging
		throw std::out_of_range("Device index out of range");;
	}

	data_pack_t ret_values;
	for(auto i=data.begin();i!= data.end();++i) {
		std::vector<uint8_t> temp;
		temp.resize(i->size());
		ret_values.push_back(temp);
	}
	spi_ioc_transfer* transfers;
	transfers = new spi_ioc_transfer[data.size()];
	memset(transfers,0,sizeof(struct spi_ioc_transfer)*data.size());
	for(int index=0;index<data.size();++index) {
		transfers[index].tx_buf = (unsigned long) data[index].data();
		transfers[index].rx_buf = (unsigned long) ret_values[index].data();
		transfers[index].len = data[index].size();
	}
	int status = ioctl(fds[device], SPI_IOC_MESSAGE(data.size()), transfers);
	if(status < 0) {
		int error_code = errno;
		std::stringstream error_stream;
		error_stream << "IOCTL ERROR CODE: ";
		error_stream << error_code << " - ";
		error_stream << strerror(error_code);
		throw std::runtime_error(error_stream.str());
	}
	return ret_values;
}
