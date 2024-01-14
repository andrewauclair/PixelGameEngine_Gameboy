#pragma once

#include "memory.h"
#include "processor.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "cartridge.h"
#include "joypad.h"

class device
{
private:
	std::shared_ptr<memory> m_memory{};
	std::shared_ptr<processor> m_processor{};
	std::shared_ptr<graphics> m_graphics{};
	std::shared_ptr<joypad> m_joypad{};

	input m_input{};
	
	std::vector<uint8_t> m_boot_rom{};
	std::shared_ptr<cartridge> m_cartridge{};

	bool m_booted{};
	bool m_running{};
	
public:
	device(const std::vector<uint8_t>& boot_rom, const cartridge& cart, std::shared_ptr<olc::PixelGameEngine> renderer);

	bool boot();

	void execute();

	std::shared_ptr<graphics> graphics_module();
};
