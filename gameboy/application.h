#pragma once

#include "olcPixelGameEngine.h"
#include "device.h"

#include <optional>

class application : public olc::PixelGameEngine
{
private:
	std::vector<uint8_t> m_boot_rom{};
	std::unique_ptr<device> m_device{};
	

	std::optional<cartridge> m_game_cartridge{};

public:
	application(const std::string_view boot_bin, const std::string_view cart);

	bool OnUserCreate() override;

	bool OnUserUpdate(float elapsed_time) override;
};
