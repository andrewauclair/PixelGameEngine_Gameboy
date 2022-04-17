#include "application.h"

application::application()
{
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
