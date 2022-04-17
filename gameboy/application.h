#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class application : public olc::PixelGameEngine
{
public:
	application();

	bool OnUserCreate() override;

	bool OnUserUpdate(float elapsed_time) override;
};
