#include "cartridge.h"

cartridge::cartridge(const std::vector<uint8_t>& rom, unsigned int memory_bank_controller, unsigned int ram_banks, unsigned int ram_bank_size)
	: m_memory_bank_controller{ memory_bank_controller },
	m_rom{ rom }
{
	m_ram.resize(ram_banks);

	for (std::vector<uint8_t>& bank : m_ram)
	{
		bank.resize(ram_bank_size);
	}
}

uint8_t cartridge::read_byte(uint16_t address)
{
	if (address >= m_rom.size())
	{
		return 0;
	}

	if (m_memory_bank_controller == 0)
	{
		return m_rom[address];
	}
	
	switch (address & 0xf000)
	{
	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
	{
		switch (m_memory_bank_controller)
		{
		case 1:
		case 2:
		case 3:
			return m_rom[address + ((m_current_bank_ROM - 1) * 0x4000)];
		case 5:
			return m_rom[address + ((m_current_bank_ROM - 1) * 0x4000)];
		}
		break;
	}
	case 0xa000:
	case 0xb000:
	{
		if (m_current_bank_RAM < m_ram.size())
		{
			return m_ram[m_current_bank_RAM][address & 0x1fff];
		}
		break;
	}
	}

	return m_rom[address];
}

void cartridge::write_byte(uint16_t address, uint8_t value)
{
	switch (address & 0xf000)
	{
	case 0x0000:
	case 0x1000:
	{
		switch (m_memory_bank_controller)
		{
		case 1:
		case 3:
		case 5:
		{
			m_ram_enabled = (value & 0xf) == 0xa;
			break;
		}
		case 2:
		{
			m_ram_enabled = (address & 0x100) == 0 && (value & 0xf) == 0xa;
			break;
		}
		}
		break;
	}
	case 0x2000:
	case 0x3000:
	{
		switch (m_memory_bank_controller)
		{
		case 1:
		{
			if (value == 0)
			{
				value++;
			}

			value &= 0x1f;

			m_current_bank_ROM &= 0x60;
			m_current_bank_ROM |= value;

			break;
		}
		case 2:
		{
			m_current_bank_ROM = value & 0xf;

			break;
		}
		case 3:
		{
			if (value == 0)
			{
				value++;
			}

			value &= 0x7f;

			m_current_bank_ROM = value;

			break;
		}
		case 5:
		{
			switch (address & 0xf000)
			{
			case 0x2000:
			{
				m_current_bank_ROM &= 0x100;
				m_current_bank_ROM |= value & 0xff;

				break;
			}
			case 0x3000:
			{
				m_current_bank_ROM &= 0xff;
				m_current_bank_ROM |= (value & 0x1) << 8;

				break;
			}
			break;
			}
			break;
		}
		}
	}
	case 0x4000:
	case 0x5000:
	{
		switch (m_memory_bank_controller)
		{
		case 1:
		{
			if (m_memory_model == 1)
			{
				m_current_bank_RAM = 0;

				value &= 0x3;
				value <<= 0x5;

				// 20h, 40h and 60h aren't accessible
				if ((m_current_bank_ROM & 0x1f) == 0)
				{
					value++;
				}

				m_current_bank_ROM &= 0x1f;
				m_current_bank_ROM |= value;
			}
			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			if (value <= 0x3)
			{
				m_current_bank_RAM = value;
			}
			else
			{
				// this is rtc stuff
			}
			break;
		}
		case 5:
		{
			break;
		}
		}
		break;
	}
	case 0x6000:
	case 0x7000:
	{
		switch (m_memory_bank_controller)
		{
		case 1:
		{
			value &= 0x1;
			m_memory_model = value;

			if (m_memory_model == 0)
			{
				m_current_bank_RAM = 0;
			}
			else
			{
				m_current_bank_ROM &= 0x1F;
			}

			break;
		}
		}
		break;
	}
	case 0xa000:
	case 0xb000:
	{
		if (m_ram_enabled && m_current_bank_RAM < m_ram.size())
		{
			m_ram[m_current_bank_RAM][address & 0x1fff] = value;
		}

		break;
	}
	}
}