#pragma once

#include "memory.h"
#include "processor.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "cartridge.h"

class device
{
private:
	memory m_memory{};
	processor m_processor{};
	graphics m_graphics{};
	input m_input{};
	
	cartridge m_cartridge;

public:
	device(const cartridge& cartridge);
};
