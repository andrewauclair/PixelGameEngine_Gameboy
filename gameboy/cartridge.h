#pragma once

#include <vector>

class cartridge
{
private:
	std::vector<std::vector<uint8_t>> m_ram{};
	std::vector<uint8_t> m_rom{};

	unsigned int m_current_bank_ROM{};
	unsigned int m_current_bank_RAM{};

	unsigned int m_memory_bank_controller{};
	unsigned int m_ram_banks{};
	unsigned int m_ram_bank_size{};

	unsigned int m_rom_size{};

	unsigned char m_memory_model{};

	bool m_ram_enabled{};

public:
	cartridge(const std::vector<uint8_t>& rom, unsigned int memory_bank_controller, unsigned int ram_banks, unsigned int ram_bank_size);

	uint8_t read_byte(uint16_t address);
	void write_byte(uint16_t address, uint8_t value);
};
