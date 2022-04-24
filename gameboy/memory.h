#pragma once

#include "cartridge.h"

#include <vector>
#include <memory>

class memory
{
private:
	std::vector<uint8_t> m_memory{};
	std::vector<uint8_t> m_boot_rom{};

	std::shared_ptr<cartridge> m_cartridge{};

	bool m_booting{};

public:
	memory();

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
