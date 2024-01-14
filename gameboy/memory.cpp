#include <iostream>
#include <iomanip>

#include "memory.h"
#include "graphics.h"
#include "processor.h"
#include "data.h"

bool memory::wrote_to_tima = false;

memory::memory()
	: m_cgbRamBanks{}
{
	memory::wrote_to_tima = false;

	/*constexpr int ram_size{ 65536 };
	m_memory.resize(ram_size);*/
}

void memory::set_processor(std::shared_ptr<processor> processor)
{
	m_processor = processor;
}

void memory::set_graphics(std::shared_ptr<graphics> graphics)
{
	m_graphics = graphics;
}

void memory::set_joypad(std::shared_ptr<joypad> joypad)
{
	m_joypad = joypad;
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
	m_memory[0xFF03] = (char)0xff;
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
		return m_graphics->read_byte(address);
	}
	else if (address >= 0xa000 && address <= 0xbfff)
	{
		return m_cartridge->read_byte(address);
	}
	else if (address == 0xFF00)
	{
		m_joypad->update();
		return m_memory[0xFF00];
		//return 0;
	}
	else if (address >= 0xFF10 && address <= 0xFF26)
	{
		//return m_pSound->readByte(addr);
		//return m_memory[address];
		//return 0;
		return m_sound.read_byte(address);
	}
	else if (address >= 0xFF47 && address < 0xFF50)
	{
		return m_graphics->read_register(address);
	}
	else if (address == 0xFF4F)
	{
		return m_graphics->read_register(address);
	}
	else if (address >= 0xFF68 && address <= 0xFF6B)
	{
		return m_graphics->read_register(address);
	}
	else if (address >= 0xD000 && address <= 0xDFFF)
	{
		return m_cgbRamBanks[m_currBankRAM][address - 0xD000];
		//std::cout << "reading from cgb ram banks" << std::endl;
	}
	else
	{
		return m_memory[address];
	}
}

uint16_t memory::read_short(uint16_t address) const
{
	return (read_byte(address + 1) << 8) | (read_byte(address) & 0xff);
}

void memory::write_byte(uint16_t address, uint8_t value)
{
	// writing to cartridge
	if (address < 0x8000)
	{
		m_cartridge->write_byte(address, value);
		return;
	}

	if (address == 0xFF00)
	{
		//gbCPU::s_debugRegisters = true;
		// need to only set the 0x30 values of val 00110000 into addr
		//m_pMemory[addr] = (m_pMemory[addr] & 0xCF) | (val & 0x30);
		m_memory[0xFF00] = value & 0x30;
		// joypad to update values
		//m_pJoypad->updateValues();
		m_joypad->update();
	}
	else if (address >= 0x8000 && address < 0xA000)
	{
		m_graphics->write_byte(address, value);
	}
	else if ((address >= 0xA000) && (address <= 0xBFFF))
	{
		m_cartridge->write_byte(address, value);
	}
	else if (address >= 0xE000 && address <= 0xFDFF)
	{
		m_memory[address] = value;
		m_memory[address - 0x2000] = value;
	}
	else if (address >= 0xFEA0 && address <= 0xFEFF)
	{
		// restricted memory
	}
	else if (address == 0xFF04) // DIV
	{
		const bool bit_3_high = processor::m_clocks_timer & 0x8;
		const bool bit_5_high = processor::m_clocks_timer & 0x20;
		const bool bit_7_high = processor::m_clocks_timer & 0x80;
		const bool bit_9_high = processor::m_clocks_timer & 0x200;

		processor::m_clocks_timer = 0;
		
		m_memory[address] = 0;

		processor::m_timer = processor::m_timer_cycles_required;
		bool trigger = false;
		
		if (processor::m_timer_cycles_required == 16 && bit_3_high)
		{
			trigger = true;
		}
		else if (processor::m_timer_cycles_required == 64 && bit_5_high)
		{
			trigger = true;
		}
		else if (processor::m_timer_cycles_required == 256 && bit_7_high)
		{
			trigger = true;
		}
		else if (processor::m_timer_cycles_required == 1024 && bit_9_high)
		{
			trigger = true;
		}

		if (trigger)
		{
			uint8_t tima = read_byte_from_raw_memory(0xff05);
			uint8_t tma = read_byte_from_raw_memory(0xff06);
			uint8_t tmc = read_byte_from_raw_memory(0xff07);

			if (tmc & 0x4) // clock enabled
			{
				if (tima == 0xFF)
				{
					memory::wrote_to_tima = false;

					write_byte_to_raw_memory(0xff05, 0);
					m_processor->execute_cycle();
					m_processor->execute_cycle();
					m_processor->execute_cycle();
					m_processor->execute_cycle();

					if (!memory::wrote_to_tima)
					{
						write_byte_to_raw_memory(0xff05, tma);
						request_interrupt(mask_int_timer);
					}
					memory::wrote_to_tima = false;
				}
				else
				{
					write_byte_to_raw_memory(0xff05, tima + 1);
				}
			}
		}
	}
	else if (address == 0xFF05) // TIMA
	{
		//gbCPU::m_timer = 0;
		m_memory[address] = value;
		memory::wrote_to_tima = true;
	}
	else if (address == 0xff06) // TMA
	{
		m_memory[address] = value;
	}
	else if (address == 0xFF07) // TAC
	{
		//uint8_t cur = m_memory[address];
		//if (value & 0x4 && (cur & 0x4) == 0)
		//{
		//	//cout << "start timer" << endl;
		//	processor::m_timer = 0;
		//}

		//m_memory[address] = value;

		uint8_t current_frequency{ static_cast<uint8_t>(read_byte(0xFF07) & 0x3) };

		m_memory[address] = value & 0x7;

		uint8_t new_frequency{ static_cast<uint8_t>(read_byte(0xFF07) & 0x3) };

		if (value & 0x4)
		{
			switch (new_frequency)
			{
			case 0: processor::m_timer = 1024; break;
			case 1: processor::m_timer = 16; break;
			case 2: processor::m_timer = 64; break;
			case 3: processor::m_timer = 256; break;
			}

			if (current_frequency != new_frequency)
			{
				processor::m_timer_cycles_required = processor::m_timer;
			}
		}

	}
	else if (address >= 0xFF10 && address <= 0xFF26)
	{
		//m_pSound->writeByte(address, value);
		m_sound.write_byte(address, value);
	}
	else if (address == 0xFF40)
	{
		// clear the screen if the lcd is being turned off
		if ((m_memory[address] & 80) && ((value & 0x80) == 0))
		{
			//cout << "turn off lcd" << endl;
			m_graphics->turn_off_LCD();
		}
		m_memory[address] = value;
	}
	else if (address == 0xFF41)
	{
		m_memory[address] = value & 0xF8;
	}
	else if (address == 0xFF44)
	{
		m_memory[address] = 0;
	}
	else if (address == 0xFF46)
	{
		uint16_t fetchAddr = value << 8; // val = addr / 100h
		//cout << "DMA transfer addr: " << std::hex << fetchAddr << endl;
		for (int i = 0; i < 160; ++i)
		{
			m_memory[0xFE00 + i] = read_byte(fetchAddr + i);//m_pMemory[(fetchAddr + i)];
		}
	}
	else if (address >= 0xFF47 && address <= 0xFF49)
	{
		m_graphics->write_register(address, value);
	}
	//else if (addr >= 0xFF4C && addr <= 0xFF7F)
	//{
	//}
	else if (address == 0xFF4D)
	{
		//std::cout << "speed switch $FF4D " << std::hex << (unsigned short)value << std::endl;
		m_memory[address] |= value & 0x7F;
	}
	else if (address == 0xFF4F)
	{
		m_graphics->write_register(address, value);
	}
	else if (address == 0xFF50)
	{
		// replace the boot rom with the cart
		std::cout << "switch to cart" << std::endl;
		m_booting = false;

		m_processor->boot();

		//m_memory[0xff04] = 0;

		//std::cout << "current CGB palettes" << std::endl;
		//m_pGPU->dump();
	}

	/// \todo FF51, FF52, FF53, FF54, FF55



	else if (address >= 0xFF51 && address <= 0xFF55)
	{
		//std::cout << "Writing to " << std::hex << (unsigned short)address << std::endl;
	}
	else if (address >= 0xFF68 && address <= 0xFF6B)
	{
		//cout << "write to gpu register " << hex << addr << endl;
		m_graphics->write_register(address, value);
	}
	else if (address == 0xFF70)
	{
	
		/*int old = m_currBankRAM;
		m_currBankRAM = value & 0x7;
		if (m_currBankRAM == 0)
		{
			m_currBankRAM++;
		}
		m_memory[0xFF70] = m_currBankRAM & 0x7;
		if (old != m_currBankRAM)
		{
			for (int i = 0; i < 4096; ++i)
			{
				m_memory[0xD000 + i] = m_cgbRamBanks[m_currBankRAM][i];
			}
		}*/
	}
	else if (address >= 0xD000 && address <= 0xDFFF)
	{
		m_cgbRamBanks[m_currBankRAM][address - 0xD000] = value;
		//std::cout << "writing to cgb ram banks" << std::endl;
		//m_memory[address] = value;
		

	}
	else if (address >= 0xFF4C && address <= 0xFF7F)
	{
		// TODO what is this memory range?
	}
	else if (address == io_lcd_scx)
	{
		//cout << "new scroll x: " << hex << (unsigned short)val << endl;
		m_memory[address] = value;
	}
	else if (address == io_lcd_scy)
	{
		//cout << "new scroll y: " << hex << (unsigned short)val << endl;
		m_memory[address] = value;
	}
	else
	{
		m_memory[address] = value;
	}
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
