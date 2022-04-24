#pragma once

#include "olcPixelGameEngine.h"

#include "device.h"

class application : public olc::PixelGameEngine
{
private:
	std::unique_ptr<device> m_device{};

public:
	application(const std::string_view boot_bin, const std::string_view cart);

	bool OnUserCreate() override;

	bool OnUserUpdate(float elapsed_time) override;
};
