#include "application.h"

#include <fstream>

std::vector<uint8_t> read_boot_rom(const std::string_view boot_bin)
{
	std::ifstream file{ boot_bin.data(), std::ios::in | std::ios::binary | std::ios::ate};

	std::vector<char> data{};

	if (file.is_open())
	{
		std::cout << "Opened boot rom for read" << std::endl;

		const auto size{ file.tellg() };

		data.resize(size);

		file.seekg(0, std::ios::beg);
		file.read(data.data(), data.size());
	}
	else
	{
		std::cout << "Failed to open boot rom for read" << std::endl;
	}

	return std::vector<uint8_t>{ data.begin(), data.end() };
}

application::application(const std::string_view boot_bin, const std::string_view cart)
{
	std::vector<uint8_t> boot_rom{ read_boot_rom(boot_bin) };

	sAppName = "Gameboy Emulator";
}

bool application::OnUserCreate()
{
	return true;
}

bool application::OnUserUpdate(float elapsed_time)
{
	// called once per frame
	for (int x = 0; x < ScreenWidth(); x++)
		for (int y = 0; y < ScreenHeight(); y++)
			Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
	return true;
}
