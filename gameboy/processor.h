#pragma once

#include "data.h"

class memory;
class graphics;

class processor
{
private:
	memory* m_memory{};
	graphics* m_graphics{};

	cpu_registers m_registers{};

	unsigned int m_cpu_clocks{};

public:
	bool execute_instruction();
};
