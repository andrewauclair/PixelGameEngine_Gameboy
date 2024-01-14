#include "sound.h"
#include "data.h"

uint8_t sound::read_byte(uint16_t address) const
{
	//if (m_soundOn)
	{
		switch (address)
		{
		case io_NR10:
		{
			return m_NR10;
		}break;
		case io_NR11:
		{
			return m_NR11;
		}break;
		case io_NR12:
		{
			return m_NR12;
		}break;
		case io_NR13:
		{
			return m_NR13;
		}break;
		case io_NR14:
		{
			return m_NR14;
		}break;
		case io_NR41:
		{
			return m_NR41;
		}break;
		case io_NR52:
		{
			return m_NR52;
		}break;
		};
	}

	if (address >= 0xFF30 && address <= 0xFF3F)
	{
		return m_waveRAM[address & 0xFF30];
	}
	return 0;
}

void sound::write_byte(uint16_t address, uint8_t value)
{
	if (m_soundOn)
	{
		switch (address)
		{
		case io_NR10:
		{
			m_NR10 = value;
		}break;
		case io_NR11:
		{
			m_NR11 = value;
		}break;
		case io_NR12:
		{
			m_NR12 = value;
		}break;
		case io_NR14:
		{
			m_NR14 = value;
		}break;
		case io_NR41:
		{
			m_NR41 = value;
		}break;
		case io_NR52:
		{
			m_NR52 &= 0x7F;
			m_NR52 |= (value & 0x80);
			m_soundOn = (m_NR52 & 0x80) != 0;
			if (!m_soundOn)
			{
				clear_registers();
			}
		}break;
		};
	}
	else if (!m_soundOn && address == io_NR52)
	{
		m_NR52 &= 0x7F;
		m_NR52 |= (value & 0x80);
		m_soundOn = (m_NR52 & 0x80) != 0;
	}

	if (address >= 0xFF30 && address <= 0xFF3F)
	{
		m_waveRAM[address & 0xFF30] = value;
	}
}

void sound::clear_registers()
{
	m_NR10 = 0;
	m_NR11 = 0;
	m_NR12 = 0;
	m_NR13 = 0;
	m_NR14 = 0;
	m_NR21 = 0;
	m_NR22 = 0;
	m_NR23 = 0;
	m_NR24 = 0;
	m_NR30 = 0;
	m_NR31 = 0;
	m_NR32 = 0;
	m_NR33 = 0;
	m_NR34 = 0;
	m_NR41 = 0;
	m_NR42 = 0;
	m_NR43 = 0;
	m_NR44 = 0;
	m_NR50 = 0;
	m_NR51 = 0;
	m_NR52 = 0;
}
