#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include <string>
#include <unordered_map>
#include <stdint.h>
#include <glibmm.h>
#include <sigc++/signal.h>
#include <sigc++/trackable.h>

enum class IOIndex {
	P1_7,
	P1_11,
	P1_12,
	P1_15,
	P1_16,
	P1_18,
	P1_22,
	P1_13,
	P5_5,
	P5_6
};

namespace std {
	template<> struct hash<IOIndex> {
		size_t operator()(const IOIndex &Foo) const {
			return static_cast<size_t>(Foo);
		}
	};
};

struct GPIODescription {
	IOIndex id;
	uint8_t gpio_id;
	uint8_t port_num;
	uint8_t pin_num;
	std::string desc;
};

struct GPIOFiles {
	int value_fd;
	int direction_fd;
	int active_low_fd;
	int edge_fd;
};

struct GPIOState {
	bool value;
	bool direction;
	bool active_low;
};

typedef sigc::signal<void, GPIODescription, GPIOState> GPIOChangedSig;

class GPIOManager : public sigc::trackable {
	public:
		GPIOManager();
		~GPIOManager();
		GPIOChangedSig& gpio_changed();
	private:
		static const std::unordered_map<IOIndex, GPIODescription> GPIO_TABLE;
		std::unordered_map<IOIndex, GPIOFiles> fd_table; 
		GPIOChangedSig changed_signal;
		void export_gpio(const GPIODescription& );
		void unexport_gpio(const GPIODescription&);
		void setup_gpio(const GPIODescription &);
		bool io_changed(Glib::IOCondition, IOIndex);
		
};
#endif

