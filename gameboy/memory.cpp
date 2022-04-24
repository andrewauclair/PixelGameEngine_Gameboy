#include "memory.h"
#include "data.h"

memory::memory()
{
	constexpr int ram_size{ 65536 };
	m_memory.resize(ram_size);
}

void memory::boot(const std::vector<uint8_t> boot_rom, std::shared_ptr<cartridge> cartridge)
{
	m_booting = true;

	m_boot_rom = boot_rom;

	m_cartridge = cartridge;

	std::copy(boot_rom.begin(), boot_rom.end(), m_memory.begin());

	m_memory[0xFF00] = (char)0xCF;
	m_memory[0xFF40] = (char)0x00;
	m_memory[0xFF41] = (char)0x84;
	m_memory[0xFF47] = (char)0xFC;
	m_memory[0xFF48] = (char)0xFF;
	m_memory[0xFF49] = (char)0xFF;
	m_memory[0xFF02] = (char)0x7E;
	m_memory[0xFF04] = (char)0x00;
	m_memory[0xFF07] = (char)0xF8;
	m_memory[0xFF0F] = (char)0xE1;
	m_memory[0xFF10] = (char)0x80;
	m_memory[0xFF11] = (char)0xBF;
	m_memory[0xFF12] = (char)0xF0;
	m_memory[0xFF13] = (char)0xFF;
	m_memory[0xFF14] = (char)0xBF;
	m_memory[0xFF15] = (char)0xFF;
	m_memory[0xFF16] = (char)0x3F;
	m_memory[0xFF17] = (char)0x00;
	m_memory[0xFF18] = (char)0xFF;
	m_memory[0xFF19] = (char)0xBF;
	m_memory[0xFF1A] = (char)0x7F;
	m_memory[0xFF1B] = (char)0xFF;
	m_memory[0xFF1C] = (char)0x9F;
	m_memory[0xFF1D] = (char)0xFF;
	m_memory[0xFF1E] = (char)0xBF;
	m_memory[0xFF1F] = (char)0xFF;
	m_memory[0xFF20] = (char)0xFF;
	m_memory[0xFF21] = (char)0x00;
	m_memory[0xFF22] = (char)0x00;
	m_memory[0xFF23] = (char)0xBF;
	m_memory[0xFF24] = (char)0x77;
	m_memory[0xFF25] = (char)0xF3;
	m_memory[0xFF26] = (char)0xFB;

	// gameboy color registers
	m_memory[0xFF4D] = (char)0xFF;
	m_memory[0xFF4F] = (char)0xFF;
	m_memory[0xFF68] = (char)0xFF;
	m_memory[0xFF69] = (char)0xFF;
	m_memory[0xFF6A] = (char)0xFF;
	m_memory[0xFF6B] = (char)0xFF;
	m_memory[0xFF70] = (char)0xFF;
}

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
	if (address == 0xff4d)
	{
		return m_memory[address];
	}

	if (address <= m_boot_rom.size() && !m_booting)
	{
		return m_cartridge->read_byte(address);
	}
	else if (address > 0xff && address < 0x200)
	{
		return m_cartridge->read_byte(address);
	}
	else if (address > m_boot_rom.size() && address < 0x8000)
	{
		return m_cartridge->read_byte(address);
	}
	else if (address >= 0x8000 && address < 0xa000)
	{
		//return m_graphics->read_byte(address);
	}
	else if (address >= 0xa000 && address <= 0xbfff)
	{
		return m_cartridge->read_byte(address);
	}
	else if (address == 0xFF00)
	{
		//m_pJoypad->updateValues();
		//return m_pMemory[0xFF00];
	}
	else if (address >= 0xFF10 && address <= 0xFF26)
	{
		//return m_pSound->readByte(addr);
	}
	else if (address >= 0xFF47 && address < 0xFF50)
	{
		//return m_pGPU->readRegister(addr);
	}
	else if (address == 0xFF4F)
	{
		//return m_pGPU->readRegister(addr);
	}
	else if (address >= 0xFF68 && address <= 0xFF6B)
	{
		//return m_pGPU->readRegister(addr);
	}
	else if (address >= 0xD000 && address <= 0xDFFF)
	{
		//return m_cgbRamBanks[m_currBankRAM][addr - 0xD000];
	}

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
