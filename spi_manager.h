#ifndef SPI_MANAGER_H_
#define SPI_MANAGER_H_


#include <string>
#include <vector>
#include <linux/spi/spidev.h>


const std::string SPI_DEVICES[] = {"/dev/spidev0.0", "/dev/spidev0.1"};
typedef std::vector<std::vector<uint8_t>> data_pack_t;

class SPIManager {
	public:
		SPIManager();
		~SPIManager();
		data_pack_t do_transfer(int device, const data_pack_t& data);
	private:
	int fds[2];
};
#endif
