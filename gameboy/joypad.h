#pragma once

#include <memory>

class memory;
class application;

enum class joypad_key
{
	start = 0, // bit 3
	select,    // bit 2
	b,         // bit 1
	a,         // bit 0
	down,      // bit 3
	up,        // bit 2
	left,      // bit 1
	right      // bit 0
};

class joypad
{
private:
	bool m_key_states[8]{};

	std::shared_ptr<memory> m_memory{};

public:
	joypad(std::shared_ptr<memory> memory);

	void update();

	void check_for_input(application& app);
};
