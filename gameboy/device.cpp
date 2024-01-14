#include "device.h"

device::device(const std::vector<uint8_t>& boot_rom, const cartridge& cart, std::shared_ptr<olc::PixelGameEngine> renderer)
	: m_memory{ std::make_shared<memory>() },
	  m_boot_rom { boot_rom },
	  m_cartridge { std::make_shared<cartridge>(cart) }
{
	m_graphics = std::make_shared<graphics>(m_memory, renderer);
	m_processor = std::make_shared<processor>(m_memory, m_graphics);
	m_joypad = std::make_shared<joypad>(m_memory);
	
	m_memory->set_processor(m_processor);
	m_memory->set_graphics(m_graphics);
	m_memory->set_joypad(m_joypad);
}

bool device::boot()
{
	const auto passed_logo_check = [&]() -> bool {
		constexpr int logo_size{ 48 };

		constexpr uint16_t boot_memory_address{ 0xa8 };
		constexpr uint16_t cart_memory_address{ 0x104 };

		uint16_t boot_address{ boot_memory_address };
		uint16_t cart_address{ cart_memory_address };

		for (int i = 0; i < logo_size; i++, boot_address++, cart_address++)
		{
			if (m_memory->read_byte(boot_address) != m_cartridge->read_byte(cart_address))
			{
				return false;
			}
		}
		return true;
	};

	m_memory->boot(m_boot_rom, m_cartridge);

	m_booted = passed_logo_check();
	
	return m_booted;
}

void device::execute()
{
	if (m_booted)
	{
		m_processor->execute_instruction();
	}
}

std::shared_ptr<graphics> device::graphics_module()
{
	return m_graphics;
}
