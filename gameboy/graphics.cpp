#include "graphics.h"
#include "data.h"

graphics::graphics(std::shared_ptr<memory> memory, std::shared_ptr<olc::PixelGameEngine> renderer)
	: m_memory{ memory },
	  m_renderer{ renderer }
{
}

bool graphics::is_frame_complete() const
{
	return m_frame_complete;
}

void graphics::start_new_frame()
{
	m_frame_complete = false;
}

void graphics::update(int clock_cycles)
{
	m_line = m_memory->read_byte_from_raw_memory(io_lcd_ly);
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);
	unsigned char lcdstat = m_memory->read_byte_from_raw_memory(io_lcd_stat);

	if ((lcdc & mask_lcdc_lcd_status) == 0)
	{
		if (m_screen_clear)
		{
			return;
		}
		m_screen_clear = true;

		memset(m_screen, 0, sizeof(m_screen));
		//memset(m_pPixels, 0, sizeof(m_pPixels));

		m_mode = 0;
		m_cycles = 0;
		lcdstat &= 0xFC;
		m_line = 0;

		m_memory->write_byte_to_raw_memory(io_lcd_stat, lcdstat);
		m_memory->write_byte_to_raw_memory(io_lcd_ly, m_line);
		// lcd is off so we shouldn't do anything
		return;
	}
	m_screen_clear = false;
	m_cycles += clock_cycles;

	switch (m_mode)
	{
	case 0: // H-Blank
	{
		if (m_cycles >= clocks_gpu_hblank)
		{
			m_cycles -= clocks_gpu_hblank;

			m_line++;


			if (m_line >= 144)
			{
				/*if (s_breakOnVBlank)
				{
					std::cout << "v-blank, waiting for user..." << std::endl;
					gbDevice::s_waitForInput = true;
				}*/
				m_mode = 1;
				if (lcdstat & mask_stat_vblankint)
				{
					// request lcd interrupt
					m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) | mask_int_lcd);
				}
				
				// request v-blank interrupt
				m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) | mask_int_vblank);

				//float pixels[(160 * 144) * 3];
				unsigned char palette = m_memory->read_byte_from_raw_memory(io_lcd_bgp);
				float colors[4] = { 255.0f, 192.0f, 96.0f, 0.0f };

				//olc::Sprite sprite(160, 144);

				// move m_screen to m_pixels
				for (int y = 0; y < 144; ++y)
				{
					for (int x = 0; x < 160; ++x)
					{
						//float val = colors[m_screen[y][x]];

						//int yp = 143 - y;
						//int pos = ((yp * 160) + x) * 3;
						/*pixels[pos] = colors[m_screen[y][x].red] / 255.0f;
						pixels[pos + 1] = colors[m_screen[y][x].green] / 255.0f;
						pixels[pos + 2] = colors[m_screen[y][x].blue] / 255.0f;*/

						//m_pPixelGameEngine->Draw(x, y, m_screen[y][x]);

						m_renderer->Draw(x, y, olc::Pixel(colors[m_screen[y][x].r], colors[m_screen[y][x].g], colors[m_screen[y][x].b]));
						//sprite.SetPixel(x, y, olc::Pixel(colors[m_screen[y][x].red], colors[m_screen[y][x].green], colors[m_screen[y][x].blue]));
					}
				}

				//m_pPixelGameEngine->DrawSprite(0, 0, &sprite);

				m_frame_complete = true;

				//memcpy(m_pScreen->pixels, pixels, sizeof(pixels));

				//glutPostRedisplay();
			}
			else
			{
				m_mode = 2;

				if (lcdstat & mask_stat_oamint)
				{
					m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) | mask_int_lcd);
				}
			}
		}
	}break;
	case 1: // V-Blank
	{

		if (m_cycles >= (clocks_gpu_hblank + clocks_gpu_oam + clocks_gpu_vram))
		{
			m_cycles -= (clocks_gpu_hblank + clocks_gpu_oam + clocks_gpu_vram);

			m_line++;

			if (m_line >= 153)
			{
				m_mode = 2;
				m_line = 0;
				if (lcdstat & mask_stat_oamint)
				{
					m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) | mask_int_lcd);

				}
			}
		}
	}break;
	case 2: // scan line OAM
	{
		if (m_cycles >= clocks_gpu_oam)
		{
			m_cycles -= clocks_gpu_oam;
			m_mode = 3;
			if (lcdstat & 0x8)
			{
				//m_pRawMemory[io_int_flag] |= 0x2;
			}
		}
	}break;
	case 3: // scan line VRAM
	{
		if (m_cycles >= clocks_gpu_vram)
		{
			/*if (s_breakOnHBlank)
			{
				std::cout << "h-blank, waiting for user..." << std::endl;
				device::s_waitForInput = true;
			}*/
			m_cycles -= clocks_gpu_vram;
			m_mode = 0;
			if (lcdstat & mask_stat_hblankint)
			{
				m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) | mask_int_lcd);
			}
			render_scanline();
		}
	}break;
	}

	lcdstat &= 0xFC; // remove current mode
	lcdstat |= m_mode; // set new mode
	
	m_memory->write_byte_to_raw_memory(io_lcd_ly, m_line);

	if (m_memory->read_byte_from_raw_memory(io_lcd_ly) == m_memory->read_byte_from_raw_memory(io_lcd_lyc) && (lcdstat & mask_stat_coincidenceint))
	{
		m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) | 0x2);

	}

	m_memory->write_byte_to_raw_memory(io_lcd_stat, lcdstat);
}


void graphics::render_background()
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);

	bool lcdEnabled = (lcdc & 0x80) != 0;
	bool bgEnabled = (lcdc & 0x1) != 0;

	//if (lcdEnabled && bgEnabled)
	{
		unsigned short startMap = (lcdc & 0x8) ? 0x9C00 : 0x9800;
		unsigned short startTiles = (lcdc & 0x10) ? 0x8000 : 0x8800;
		unsigned char tileSet = (lcdc & 0x10) ? 1 : 2;

		unsigned char scrollY = m_memory->read_byte_from_raw_memory(io_lcd_scy);
		unsigned char scrollX = m_memory->read_byte_from_raw_memory(io_lcd_scx);

		int screenTileY = m_line / 8;

		for (int screenTileX = 0; screenTileX < 168 / 8; ++screenTileX)
		{
			int mapX = (screenTileX * 8) + scrollX;
			mapX %= 256; // map is 256 pixels wide, wrap around if we need to

			int mapY = m_line + scrollY;
			mapY %= 256; // map is 256 pixels tall

			int mapTileX = mapX / 8;
			int mapTileY = mapY / 8;

			int tileX = scrollX - ((scrollX / 8) * 8);
			int tileY = scrollY - ((scrollY / 8) * 8);
			//int tileLine = mapY - ((scrollY / 8) * 8);
			int tileLine = mapY - (mapTileY * 8);
			tileLine %= 8;

			unsigned short mapAddr = startMap;
			//			//mapAddr += (tileY * 32) + tileX;
			mapAddr += (mapTileY * 32) + mapTileX;
			//			
			unsigned short tileAddr;
			if (tileSet == 1)
			{
				unsigned char tile = m_memory->read_byte_from_raw_memory(mapAddr);
				tileAddr = startTiles + (tile << 4);
			}
			else
			{
				char tile = m_memory->read_byte_from_raw_memory(mapAddr);
				tileAddr = startTiles + ((tile + 128) << 4);
			}

			render_background_tile(mapTileX, mapTileY, (screenTileX * 8) - tileX, m_line - tileLine, tileLine);
			//renderTileScanline((screenTileX * 8) - tileX, m_line - tileLine, tileLine, tileAddr, false, false, false);
		}
	}
}

void graphics::render_sprites()
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);

	bool lcdEnabled = (lcdc & 0x80) == 0x80;

	// only draw the sprites if they are enabled
	//if ((m_pRawMemory[0xFF40] & 0x2) == 0x2)
	{
		unsigned char palette0 = m_memory->read_byte_from_raw_memory(io_lcd_obp0);
		unsigned short rawColors0[4];
		rawColors0[3] = (palette0 & 0xC0) >> 6;
		rawColors0[2] = (palette0 & 0x30) >> 4;
		rawColors0[1] = (palette0 & 0x0C) >> 2;
		rawColors0[0] = (palette0 & 0x03);

		unsigned char palette1 = m_memory->read_byte_from_raw_memory(io_lcd_obp1);
		unsigned short rawColors1[4];
		rawColors1[3] = (palette1 & 0xC0) >> 6;
		rawColors1[2] = (palette1 & 0x30) >> 4;
		rawColors1[1] = (palette1 & 0x0C) >> 2;
		rawColors1[0] = (palette1 & 0x03);

		unsigned short spriteTable = 0x8000;
		unsigned short spriteAttr = 0xFE00;

		unsigned char objWidth = 8;
		unsigned char objHeight = (lcdc & 0x4) ? 16 : 8;

		spriteAttr = 0xFE00;
		for (int i = 0; i < 40; ++i, spriteAttr += 4)
		{
			short ypos = m_memory->read_byte_from_raw_memory(spriteAttr) & 0xFF;// -objHeight;
			short xpos = m_memory->read_byte_from_raw_memory(spriteAttr + 1) & 0xFF;// -8;
			unsigned char tile = m_memory->read_byte_from_raw_memory(spriteAttr + 2);
			unsigned char attr = m_memory->read_byte_from_raw_memory(spriteAttr + 3);

			render_sprite(xpos, ypos, tile, attr);
			continue;
			if (ypos == 0 || ypos >= 160)
			{
				//continue;
			}
			if (xpos == 0 || xpos >= 168)
			{
				//continue;
			}
			ypos -= 16;
			xpos -= 8;
			bool behindBG = (attr & 0x80) != 0;
			bool flipY = (attr & 0x40) != 0;
			bool flipX = (attr & 0x20) != 0;

			// check if this sprite rectangle is in this scanline
			if (m_line >= ypos && m_line < ypos + objHeight)
			{
				unsigned char y = m_line - ypos;// -objHeight;
												// draw the current line of this sprite
				unsigned short tileAddr = spriteTable | (tile << 4);
				if (flipY)
				{
					tileAddr += ((objHeight - 1 - y) * 2);
				}
				else
				{
					tileAddr += (y * 2); // get current line
				}

				unsigned short tileData = (m_memory->read_byte_from_raw_memory(tileAddr + 1) << 8) | (m_memory->read_byte_from_raw_memory(tileAddr) & 0x00FF);

				for (int x = 0; x < 8; ++x)
				{
					unsigned char bit = 1 << (7 - x);
					unsigned char color = ((tileData & (bit << 8)) >> (14 - x)) | ((tileData & bit) >> (7 - x));

					if (color == 0)// && !behindBG)
					{
						continue;
					}

					int drawX = xpos + x;

					if (flipX)
					{
						drawX = xpos + (7 - x);
					}
					if (drawX >= 0 && drawX < 160)
					{
						if (attr & 0x10)
						{
							//m_screen[m_line][drawX] = rawColors1[color];
						}
						else
						{
							//m_screen[m_line][drawX] = rawColors0[color];
						}
					}
				}
			}
		}
	}
}


void graphics::render_window()
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);

	bool lcdEnabled = (lcdc & 0x80) != 0;

	short winY = m_memory->read_byte_from_raw_memory(io_lcd_wy) & 0xFF;
	short winX = (m_memory->read_byte_from_raw_memory(io_lcd_wx) & 0xFF) - 7; // why -7?

													   // only draw the window if it is enabled
	if (m_line >= winY)
	{
		// starting map address
		unsigned short startWinMap = (lcdc & 0x40) ? 0x9C00 : 0x9800;
		unsigned short startTiles = /*(lcdc & 0x10) ? 0x8000 :*/ 0x8800;
		unsigned char tileSet = 2;// (lcdc & 0x10) ? 1 : 2;

		int screenTileY = m_line / 8;

		for (int screenTileX = 0; screenTileX < 168 / 8; ++screenTileX)
		{
			int mapX = screenTileX;// (screenTileX * 8);

			int mapY = m_line - winY;
			mapY /= 8;

			unsigned short mapAddr = startWinMap;
			//mapAddr += (screenTileY * 32) + screenTileX;
			mapAddr += (mapY * 32) + screenTileX;

			char tile = m_memory->read_byte_from_raw_memory(mapAddr);
			unsigned short tileAddr = startTiles + ((tile + 128) << 4);

			render_window_tile(mapX, mapY, (screenTileX * 8) + (winX % 8), (screenTileY * 8) + (winY % 8), m_line % 8);
			//renderTileScanline((screenTileX * 8) + (winX % 8), (screenTileY * 8) + (winY % 8), m_line % 8, tileAddr, false, false, false);
			//renderTileScanline((screenTileX * 8) + winX, (screenTileY * 8), m_line % 8, tileAddr, false, false, false);
		}
	}
}

void graphics::render_scanline()
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);

	bool lcdEnabled = (lcdc & 0x80) != 0;
	bool bgEnabled = (lcdc & 0x1) != 0;
	bool spritesEnabled = (lcdc & 0x2) != 0;
	bool windowEnabled = (lcdc & mask_lcdc_win_status) != 0;

	if (lcdEnabled)
	{
		if (bgEnabled)
		{
			render_background();
		}
		if (windowEnabled)
		{
			render_window();
		}
		if (spritesEnabled)
		{
			render_sprites();
		}
	}
}

void graphics::render_background_tile(unsigned short xtile, unsigned short ytile,
	short xpos, short ypos, unsigned short line)
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);
	unsigned short startMap = (lcdc & 0x8) ? 0x9C00 : 0x9800;
	unsigned short startTiles = (lcdc & 0x10) ? 0x8000 : 0x8800;
	unsigned char tileSet = (lcdc & 0x10) ? 1 : 2;

	unsigned short mapAddr = startMap + (ytile * 32) + xtile;

	unsigned short tileAddr;
	if (tileSet == 1)
	{
		unsigned char tile = m_memory->read_byte_from_raw_memory(mapAddr);
		tileAddr = startTiles + (tile << 4);
	}
	else
	{
		char tile = m_memory->read_byte_from_raw_memory(mapAddr);
		tileAddr = startTiles + ((tile + 128) << 4);
	}

	render_tile_scanline(xpos, ypos, line, tileAddr, io_lcd_bgp);
}

void graphics::render_window_tile(unsigned short xtile, unsigned short ytile,
	short xpos, short ypos, unsigned short line)
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);
	unsigned short startWinMap = (lcdc & 0x40) ? 0x9C00 : 0x9800;

	unsigned short mapAddr = startWinMap + (ytile * 32) + xtile;

	char tile = m_memory->read_byte_from_raw_memory(mapAddr);
	unsigned short tileAddr = 0x8800 + ((tile + 128) << 4);

	render_tile_scanline(xpos, ypos, line, tileAddr, io_lcd_bgp);
}

void graphics::render_sprite(short xpos, short ypos, unsigned char tile, unsigned char attr)
{
	unsigned char lcdc = m_memory->read_byte_from_raw_memory(io_lcd_lcdc);

	// check if the sprite is off screen
	unsigned short spriteTable = 0x8000;

	unsigned char objHeight = (lcdc & 0x4) ? 16 : 8;

	ypos -= 16;
	xpos -= 8;

	bool behindBG = (attr & 0x80) != 0;
	bool flipY = (attr & 0x40) != 0;
	bool flipX = (attr & 0x20) != 0;

	if (m_line >= ypos && m_line < ypos + objHeight)
	{
		unsigned char y = m_line - ypos;

		unsigned short tileAddr = spriteTable | (tile << 4);

		render_tile_scanline(xpos, ypos, y, tileAddr, (attr & 0x10) ? io_lcd_obp1 : io_lcd_obp0, 0, 0, flipX, flipY, behindBG, true);
	}
}

void graphics::render_tile_scanline(short xpos, short ypos,
	unsigned short line, unsigned short addr, unsigned short paletteAddr)
{
	const short y = line - ypos;
	addr += line * 2;

	const unsigned char upper_data = m_memory->read_byte_from_raw_memory(addr + 1);
	const unsigned char lower_data = m_memory->read_byte_from_raw_memory(addr);

	const unsigned char palette = m_memory->read_byte_from_raw_memory(paletteAddr);

	const unsigned short rawColors[4]{ (palette & 0x03), (palette & 0x0C) >> 2, (palette & 0x30) >> 4, (palette & 0xC0) >> 6 };

	for (int x = 0; x < 8; ++x)
	{
		const unsigned char bit = 1 << (7 - x);
		const unsigned char upper_bit = upper_data & bit;
		const unsigned char lower_bit = lower_data & bit;

		const unsigned char color = (upper_bit ? 0x2 : 0) | (lower_bit ? 1 : 0);

		const int screenX = xpos + x;

		if (screenX >= 0 && screenX < 160 && ypos + line >= 0 && ypos + line < 144)
		{
			m_screen[ypos + line][screenX].r = rawColors[color];
			m_screen[ypos + line][screenX].g = rawColors[color];
			m_screen[ypos + line][screenX].b = rawColors[color];
		}
	}
}

void graphics::render_tile_scanline(short xpos, short ypos,
	unsigned short line, unsigned short addr, unsigned short paletteAddr,
	unsigned short addrBank, unsigned short paletteType,
	bool flipX, bool flipY, bool checkBG, bool transparent)
{
	if (flipY)
	{
		short y = line - ypos;
		addr += (8 - line) * 2;
	}
	else
	{
		short y = line - ypos;
		addr += line * 2;
	}

	const unsigned char upper_data = m_memory->read_byte_from_raw_memory(addr + 1);
	const unsigned char lower_data = m_memory->read_byte_from_raw_memory(addr);

	const unsigned char palette = m_memory->read_byte_from_raw_memory(paletteAddr);

	const unsigned short rawColors[4]{ (palette & 0x03), (palette & 0x0C) >> 2, (palette & 0x30) >> 4, (palette & 0xC0) >> 6 };

	for (int x = 0; x < 8; ++x)
	{
		const unsigned char bit = 1 << (7 - x);
		const unsigned char upper_bit = upper_data & bit;
		const unsigned char lower_bit = lower_data & bit;

		const unsigned char color = (upper_bit ? 0x2 : 0) | (lower_bit ? 1 : 0);

		if (color == 0 && transparent)
		{
			continue;
		}

		const int screenX = flipX ? xpos + (7 - x) : xpos + x;

		if (screenX >= 0 && screenX < 160 && ypos + line >= 0 && ypos + line < 144)
		{
			m_screen[ypos + line][screenX].r = rawColors[color];
			m_screen[ypos + line][screenX].g = rawColors[color];
			m_screen[ypos + line][screenX].b = rawColors[color];
		}
	}
}
