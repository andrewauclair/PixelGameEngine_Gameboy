#pragma once

#include <vector>

class memory
{
private:
	std::vector<uint8_t> m_memory{};

public:
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
