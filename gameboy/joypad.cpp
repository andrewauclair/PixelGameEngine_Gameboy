#include "joypad.h"

#include "memory.h"

joypad::joypad(std::shared_ptr<memory> memory)
	: m_memory{ memory }
{
}

void joypad::update()
{
	uint8_t reg{ m_memory->read_byte_from_raw_memory(0xff00) };
	uint8_t intr{ m_memory->read_byte_from_raw_memory(0xff0f) };
	reg |= 0xcf;

	bool buttonKeys = (reg & 0x20) != 0x20;
	bool dirKeys = (reg & 0x10) != 0x10;

	if (buttonKeys)
	{
		if (!m_key_states[0])
		{
			reg |= 0x8;
		}
		else
		{
			reg &= ~(0x8);
		}
		if (!m_key_states[1])
		{
			reg |= 0x4;
		}
		else
		{
			reg &= ~(0x4);
		}
		if (!m_key_states[2])
		{
			reg |= 0x2;
		}
		else
		{
			reg &= ~(0x2);
		}
		if (!m_key_states[3])
		{
			reg |= 0x1;
		}
		else
		{
			reg &= ~(0x1);
		}
		//reg = 0xef;
	}
	else if (dirKeys)
	{
		if (!m_key_states[4])
		{
			reg |= 0x8;
		}
		else
		{
			reg &= ~(0x8);
		}
		if (!m_key_states[5])
		{
			reg |= 0x4;
		}
		else
		{
			reg &= ~(0x4);
		}
		if (!m_key_states[6])
		{
			reg |= 0x2;
		}
		else
		{
			reg &= ~(0x2);
		}
		if (!m_key_states[7])
		{
			reg |= 0x1;
		}
		else
		{
			reg &= ~(0x1);
		}
		//reg &= ~(0x1);
	}
	//reg |= 0x30;
	//cout << "0xFF00: " << std::hex << (unsigned int)reg << endl;
	m_memory->write_byte_to_raw_memory(0xff00, reg);
}
