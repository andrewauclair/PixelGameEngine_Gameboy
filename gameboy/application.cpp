#include "application.h"

#include <fstream>
#include <optional>

std::vector<uint8_t> read_boot_rom(const std::string_view boot_bin)
{
	std::ifstream file{ boot_bin.data(), std::ios::in | std::ios::binary | std::ios::ate };

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

std::optional<cartridge> load_cartridge(const std::string_view cartridge_path)
{
	std::ifstream file{ cartridge_path.data(), std::ios::in | std::ios::binary | std::ios::ate };

	if (file.is_open())
	{
		std::cout << "Opened game cartridge for read" << std::endl;

		std::vector<char> rom{};
		unsigned int memory_bank_controller{};
		unsigned int ram_banks{};
		unsigned int ram_bank_size{};

		rom.resize(file.tellg());

		file.seekg(0, std::ios::beg);
		file.read(rom.data(), rom.size());

		// MBC
		switch (rom[0x147])
		{
		case 0x01:
		case 0x02:
		case 0x03:
		{
			memory_bank_controller = 1;

			break;
		}
		case 0x05:
		case 0x06:
		{
			memory_bank_controller = 3;

			break;
		}
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		{
			memory_bank_controller = 3;

			break;
		}
		case 0x19:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		{
			memory_bank_controller = 5;

			break;
		}
		default:
		{
			memory_bank_controller = 0;

			break;
		}
		}

		// Ram Banks
		switch (rom[0x149])
		{
		case 1:
		{
			ram_banks = 1;
			ram_bank_size = 2048;

			break;
		}
		case 2:
		{
			ram_banks = 1;
			ram_bank_size = 8192;

			break;
		}
		case 3:
		{
			ram_banks = 4;
			ram_bank_size = 8192;

			break;
		}
		case 4:
		{
			ram_banks = 16;
			ram_bank_size = 8192;

			break;
		}
		default:
		{
			ram_banks = 0;
			ram_bank_size = 0;

			break;
		}
		}

		return cartridge{ std::vector<uint8_t>{ rom.begin(), rom.end() }, memory_bank_controller, ram_banks, ram_bank_size};
	}
	else
	{
		std::cout << "Failed to open game cartridge for read" << std::endl;
	}

	return {};
}

application::application(const std::string_view boot_bin, const std::string_view cart)
{
	sAppName = "Gameboy Emulator";
	
	m_boot_rom = read_boot_rom(boot_bin);

	m_game_cartridge = load_cartridge(cart);
}

bool application::OnUserCreate()
{
	if (m_game_cartridge.has_value())
	{
		m_device = std::make_unique<device>(m_boot_rom, m_game_cartridge.value(), shared_from_this());

		if (m_device->boot())
		{
			std::cout << "Successfully booted the game cartridge" << std::endl;
		}
		else
		{
			std::cout << "Failed to boot the game cartridge" << std::endl;
		}
	}

	return true;
}

bool application::OnUserUpdate(float elapsed_time)
{
	m_device->graphics_module()->start_new_frame();

	while (!m_device->graphics_module()->is_frame_complete())
	{
		m_device->execute();
	}

	return true;
}
