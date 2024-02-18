#include "joypad.h"

#include "memory.h"
#include "application.h"

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

void joypad::check_for_input(application& app)
{
	m_key_states[static_cast<int>(joypad_key::start)] = app.GetKey(olc::Key::Z).bPressed;
	m_key_states[static_cast<int>(joypad_key::select)] = app.GetKey(olc::Key::X).bPressed;
	m_key_states[static_cast<int>(joypad_key::a)] = app.GetKey(olc::Key::A).bPressed;
	m_key_states[static_cast<int>(joypad_key::b)] = app.GetKey(olc::Key::B).bPressed;
	m_key_states[static_cast<int>(joypad_key::down)] = app.GetKey(olc::Key::DOWN).bPressed;
	m_key_states[static_cast<int>(joypad_key::up)] = app.GetKey(olc::Key::UP).bPressed;
	m_key_states[static_cast<int>(joypad_key::left)] = app.GetKey(olc::Key::LEFT).bPressed;
	m_key_states[static_cast<int>(joypad_key::right)] = app.GetKey(olc::Key::RIGHT).bPressed;
}
