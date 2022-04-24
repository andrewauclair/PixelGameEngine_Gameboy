#include "memory.h"
#include "data.h"

uint8_t memory::read_byte_from_raw_memory(uint16_t address) const
{
	return m_memory[address];
}

uint16_t memory::read_short_from_raw_memory(uint16_t address) const
{
	return (read_byte_from_raw_memory(address + 1) << 8) | (read_byte_from_raw_memory(address) & 0xff);
}

uint8_t memory::read_byte(uint16_t address) const
{
	return m_memory[address];
}

uint16_t memory::read_short(uint16_t address) const
{
	return (read_byte(address + 1) << 8) | (read_byte(address) & 0xff);
}

void memory::write_byte(uint16_t address, uint8_t value)
{
	m_memory[address] = value;
}

void memory::write_short(uint16_t address, uint16_t value)
{
	write_byte(address + 1, (value & 0xff00) >> 8);
	write_byte(address, value & 0xff);
}

void memory::write_byte_to_raw_memory(uint16_t address, uint8_t value)
{
	m_memory[address] = value;
}

void memory::write_short_to_raw_memory(uint16_t address, uint16_t value)
{
	write_byte_to_raw_memory(address + 1, (value & 0xff00) >> 8);
	write_byte_to_raw_memory(address, value & 0xff);
}

void memory::request_interrupt(uint8_t interrupt)
{
	m_memory[io_int_flag] |= interrupt;
}

void memory::clear_interrupt(uint8_t interrupt)
{
	m_memory[io_int_flag] &= ~(interrupt);
}
