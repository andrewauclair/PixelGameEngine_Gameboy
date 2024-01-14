#pragma once

#include "cartridge.h"
#include "sound.h"
#include "joypad.h"
#include "processor.h"

#include <vector>
#include <memory>
#include <array>

class graphics;

class memory
{
private:
	sound m_sound{}; // TODO move this eventually
	std::shared_ptr<joypad> m_joypad{};

	unsigned char m_cgbRamBanks[8][4096]{};

	int m_currBankRAM = 1;
	std::array<uint8_t, 65536> m_memory{};
	std::vector<uint8_t> m_boot_rom{};

	std::shared_ptr<cartridge> m_cartridge{};
	std::shared_ptr<graphics> m_graphics{};
	std::shared_ptr<processor> m_processor{};

	int m_current_bank_RAM{};
	
	bool m_booting{};
	//bool m_ram_bank_enabled{};
	
public:
	static bool wrote_to_tima;

	memory();

	void set_processor(std::shared_ptr<processor> processor);

	void set_graphics(std::shared_ptr<graphics> graphics);

	void set_joypad(std::shared_ptr<joypad> joypad);

	void boot(const std::vector<uint8_t> boot_rom, std::shared_ptr<cartridge> cartridge);

	uint8_t read_byte_from_raw_memory(uint16_t address) const;
	uint16_t read_short_from_raw_memory(uint16_t address) const;

	uint8_t read_byte(uint16_t address) const;
	uint16_t read_short(uint16_t address) const;

	void write_byte(uint16_t address, uint8_t value);
	void write_short(uint16_t address, uint16_t value);

	void write_byte_to_raw_memory(uint16_t address, uint8_t value);
	void write_short_to_raw_memory(uint16_t address, uint16_t value);

	void request_interrupt(uint8_t interrupt);
	void clear_interrupt(uint8_t interrupt);
};
