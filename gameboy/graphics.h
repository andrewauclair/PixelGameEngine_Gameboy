#pragma once

#include <memory>

#include "olcPixelGameEngine.h"

#include "memory.h"

constexpr int SCREEN_WIDTH{ 144 };
constexpr int SCREEN_HEIGHT{ 160 };

class graphics
{
private:
	olc::Pixel m_screen[SCREEN_WIDTH][SCREEN_HEIGHT]{};

	std::array<uint8_t, 8192> m_bank_0{};

	std::shared_ptr<memory> m_memory{};
	std::shared_ptr<olc::PixelGameEngine> m_renderer{};

	uint8_t m_line{};
	uint8_t m_mode{};
	unsigned short m_cycles{};

	bool m_frame_complete{};
	bool m_screen_clear{ true };

public:
	graphics(std::shared_ptr<memory> memory, std::shared_ptr<olc::PixelGameEngine> renderer);

	bool is_frame_complete() const;
	void start_new_frame();

	void update(int clock_cycles);

	void turn_off_LCD() {} // TODO implement this

	unsigned char read_byte(uint16_t address);
	void write_byte(uint16_t address, uint8_t value);

	unsigned char read_register(uint16_t reg);
	void write_register(uint16_t reg, uint8_t value);

	void render();
	void render_map();
	void render_tiles();

private:
	void render_scanline();

	void render_background();
	void render_sprites();

	void render_window();

	void render_background_tile(unsigned short x_tile, unsigned short y_tile,
		short x_pos, short y_pos, unsigned short line);

	void render_window_tile(unsigned short xtile, unsigned short ytile,
		short x_pos, short y_pos, unsigned short line);

	void render_sprite(short x_pos, short y_pos, unsigned char tile, unsigned char attr);

	void render_tile_scanline(short x_pos, short y_pos,
		unsigned short line, unsigned short addr, unsigned short palette_addr);

	void render_tile_scanline(short xpos, short ypos,
		unsigned short line, unsigned short addr, unsigned short palette_addr,
		unsigned short addr_bank, unsigned short palette_type,
		bool flip_X, bool flip_Y, bool check_BG, bool transparent);
};
