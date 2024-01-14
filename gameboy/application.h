#pragma once

#include "olcPixelGameEngine.h"
#include "device.h"

#include <optional>
#include <filesystem>

class application : public olc::PixelGameEngine
{
private:
	std::vector<uint8_t> m_boot_rom;
	std::unique_ptr<device> m_device{};
	

	std::optional<cartridge> m_game_cartridge{};

	std::vector<std::filesystem::path> m_carts;
	std::size_t m_current_cart = 0;

public:
	application(const std::filesystem::path& boot_bin, const std::filesystem::path& carts);

	bool OnUserCreate() override;

	bool OnUserUpdate(float elapsed_time) override;
};
