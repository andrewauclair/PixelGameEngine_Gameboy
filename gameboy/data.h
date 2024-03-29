#pragma once

#include <string_view>

// cartridge types
constexpr char cart_type_dmg = 0x00;
constexpr unsigned char cart_type_gbc = 0x80;
constexpr unsigned char cart_type_gbc_only = 0xC0;

// interrupt addresses
constexpr char int_addr_vblank = 0x40;
constexpr char int_addr_lcd = 0x48;
constexpr char int_addr_timer = 0x50;
constexpr char int_addr_serial = 0x58;
constexpr char int_addr_joypad = 0x60;

// I/O addresses

// timers
constexpr unsigned short io_div = 0xFF04;
constexpr unsigned short io_tima = 0xFF05;
constexpr unsigned short io_tma = 0xFF06;
constexpr unsigned short io_tac = 0xFF07;

// joypad
constexpr unsigned short io_joypad = 0xFF00;

// sound
constexpr unsigned short io_NR10 = 0xFF10;
constexpr unsigned short io_NR11 = 0xFF11;
constexpr unsigned short io_NR12 = 0xFF12;
constexpr unsigned short io_NR13 = 0xFF13;
constexpr unsigned short io_NR14 = 0xFF14;
constexpr unsigned short io_NR21 = 0xFF16;
constexpr unsigned short io_NR22 = 0xFF17;
constexpr unsigned short io_NR23 = 0xFF18;
constexpr unsigned short io_NR24 = 0xFF19;
constexpr unsigned short io_NR30 = 0xFF1A;
constexpr unsigned short io_NR31 = 0xFF1B;
constexpr unsigned short io_NR32 = 0xFF1C;
constexpr unsigned short io_NR33 = 0xFF1D;
constexpr unsigned short io_NR34 = 0xFF1E;
constexpr unsigned short io_NR41 = 0xFF20;
constexpr unsigned short io_NR42 = 0xFF21;
constexpr unsigned short io_NR43 = 0xFF22;
constexpr unsigned short io_NR44 = 0xFF23;
constexpr unsigned short io_NR50 = 0xFF24;
constexpr unsigned short io_NR51 = 0xFF25;
constexpr unsigned short io_NR52 = 0xFF26;

// lcd
constexpr unsigned short io_lcd_lcdc = 0xFF40;
constexpr unsigned short io_lcd_stat = 0xFF41;
constexpr unsigned short io_lcd_scy = 0xFF42;
constexpr unsigned short io_lcd_scx = 0xFF43;
constexpr unsigned short io_lcd_ly = 0xFF44;
constexpr unsigned short io_lcd_lyc = 0xFF45;
constexpr unsigned short io_lcd_dma = 0xFF46;
constexpr unsigned short io_lcd_bgp = 0xFF47;
constexpr unsigned short io_lcd_obp0 = 0xFF48;
constexpr unsigned short io_lcd_obp1 = 0xFF49;
constexpr unsigned short io_lcd_wy = 0xFF4A;
constexpr unsigned short io_lcd_wx = 0xFF4B;
constexpr unsigned short io_lcd_vbank = 0xFF4F;
constexpr unsigned short io_lcd_bcps = 0xFF68;
constexpr unsigned short io_lcd_bcpd = 0xFF69;
constexpr unsigned short io_lcd_ocps = 0xFF6A;
constexpr unsigned short io_lcd_ocpd = 0xFF6B;

// interrupts
constexpr unsigned short io_int_flag = 0xFF0F;
constexpr unsigned short io_int_enable = 0xFFFF;

// Masks
constexpr char mask_int_vblank = (char)0x1;
constexpr char mask_int_lcd = (char)0x2;
constexpr char mask_int_timer = (char)0x4;
constexpr char mask_int_serial = (char)0x8;
constexpr char mask_int_joypad = (char)0x10;

constexpr char mask_lcdc_lcd_status = (char)0x80;
constexpr char mask_lcdc_win_map = (char)0x40;
constexpr char mask_lcdc_win_status = (char)0x20;
constexpr char mask_lcdc_tileset = (char)0x10;
constexpr char mask_lcdc_bg_map = (char)0x8;
constexpr char mask_lcdc_obj_size = (char)0x4;
constexpr char mask_lcdc_obj_status = (char)0x2;
constexpr char mask_lcdc_bg_status = (char)0x1;

constexpr char mask_stat_coincidenceint = (char)0x40;
constexpr char mask_stat_oamint = (char)0x20;
constexpr char mask_stat_vblankint = (char)0x10;
constexpr char mask_stat_hblankint = (char)0x8;
constexpr char mask_stat_mode = (char)0x3;

constexpr char mask_tac_speed = (char)0x3;
constexpr char mask_tac_status = (char)0x4;

constexpr char mask_mapattr_palette = (char)0x7;

// clock counts
constexpr unsigned short clocks_gpu_hblank = 200;
constexpr unsigned short clocks_gpu_oam = 84;
constexpr unsigned short clocks_gpu_vram = 172;

enum device_type
{
	gameboy_dmg = 0,
	gameboy_cgb
};

struct cpu_registers
{
	union
	{
		struct
		{
			union
			{
				uint8_t f;
				struct
				{
					uint8_t : 4;
					uint8_t c_f : 1;
					uint8_t h_f : 1;
					uint8_t n_f : 1;
					uint8_t z_f : 1;
				};
			};
			uint8_t a;
		};
		uint16_t af;
	};

	union
	{
		struct
		{
			uint8_t c;
			uint8_t b;
		};
		uint16_t bc;
	};

	union
	{
		struct
		{
			uint8_t e;
			uint8_t d;
		};
		uint16_t de;
	};

	union
	{
		struct
		{
			uint8_t l;
			uint8_t h;
		};
		uint16_t hl;
	};

	uint16_t sp;
	uint16_t pc;
};

struct cpu_instruction
{
	uint8_t opcode;
	std::string_view instruction;
	unsigned int length;
	unsigned int clocks;
};

constexpr cpu_instruction g_gbInstructions[256] =
{
	{0x00, "NOP",             0, 4 },
	{0x01, "LD BC, $%04X",    2, 12 },
	{0x02, "LD (BC), A",      0, 8 },
	{0x03, "INC BC",          0, 8 },
	{0x04, "INC B",           0, 4 },
	{0x05, "DEC B",           0, 4 },
	{0x06, "LD B, $%02X",     1, 8 },
	{0x07, "RLC A",           0, 4 },
	{0x08, "LD ($%04X), SP",  2, 20 },
	{0x09, "ADD HL, BC",      0, 8 },
	{0x0A, "LD A, (BC)",      0, 8 },
	{0x0B, "DEC BC",          0, 8 },
	{0x0C, "INC C",           0, 4 },
	{0x0D, "DEC C",           0, 4 },
	{0x0E, "LD C, $%02X",     1, 8 },
	{0x0F, "RRC A",           0, 4 },
	{0x10, "STOP",            0, 4 },
	{0x11, "LD DE, $%04X",    2, 12 },
	{0x12, "LD (DE), A",      0, 8 },
	{0x13, "INC DE",          0, 8 },
	{0x14, "INC D",           0, 4 },
	{0x15, "DEC D",           0, 4 },
	{0x16, "LD D, $%02X",     1, 8 },
	{0x17, "RL A",            0, 4 },
	{0x18, "JR $%02X",        1, 12 },
	{0x19, "ADD HL, DE",      0, 8 },
	{0x1A, "LD A, (DE)",      0, 8 },
	{0x1B, "DEC DE",          0, 8 },
	{0x1C, "INC E",           0, 4 },
	{0x1D, "DEC E",           0, 4 },
	{0x1E, "LD E, $%02X",     1, 8 },
	{0x1F, "RR A",            0, 4 },
	{0x20, "JR NZ, $%02X",    1, 8 },
	{0x21, "LD HL, $%04X",    2, 12 },
	{0x22, "LDI (HL), A",     0, 8 },
	{0x23, "INC HL",          0, 8 },
	{0x24, "INC H",           0, 4 },
	{0x25, "DEC H",           0, 4 },
	{0x26, "LD H, $%02X",     1, 8 },
	{0x27, "DAA",             0, 4 },
	{0x28, "JR Z, $%02X",     1, 8 },
	{0x29, "ADD HL, HL",      0, 8 },
	{0x2A, "LDI A, (HL)",     0, 8 },
	{0x2B, "DEC HL",          0, 8 },
	{0x2C, "INC L",           0, 4 },
	{0x2D, "DEC L",           0, 4 },
	{0x2E, "LD L, $%02X",     1, 8 },
	{0x2F, "CPL",             0, 4 },
	{0x30, "JR NC, $%02X",    1, 8 },
	{0x31, "LD SP, $%04X",    2, 12 },
	{0x32, "LDD (HL), A",     0, 8 },
	{0x33, "INC SP",          0, 8 },
	{0x34, "INC (HL)",        0, 12 },
	{0x35, "DEC (HL)",        0, 12 },
	{0x36, "LD (HL), $%02X",  1, 12 },
	{0x37, "SCF",             0, 4 },
	{0x38, "JR C, $%02X",     1, 8 },
	{0x39, "ADD HL, SP",      0, 8 },
	{0x3A, "LDD A, (HL)",     0, 8 },
	{0x3B, "DEC SP",          0, 8 },
	{0x3C, "INC A",           0, 4 },
	{0x3D, "DEC A",           0, 4 },
	{0x3E, "LD A, $%02X",     1, 8 },
	{0x3F, "CCF",             0, 4 },
	{0x40, "LD B, B",         0, 4 },
	{0x41, "LD B, C",         0, 4 },
	{0x42, "LD B, D",         0, 4 },
	{0x43, "LD B, E",         0, 4 },
	{0x44, "LD B, H",         0, 4 },
	{0x45, "LD B, L",         0, 4 },
	{0x46, "LD B, (HL)", 0, 8 },
	{0x47, "LD B, A", 0, 4 },
	{0x48, "LD C, B", 0, 4 },
	{0x49, "LD C, C", 0, 4 },
	{0x4A, "LD C, D", 0, 4 },
	{0x4B, "LD C, E", 0, 4 },
	{0x4C, "LD C, H", 0, 4 },
	{0x4D, "LD C, L", 0, 4 },
	{0x4E, "LD C, (HL)", 0, 8 },
	{0x4F, "LD C, A", 0, 4 },
	{0x50, "LD D, B", 0, 4 },
	{0x51, "LD D, C", 0, 4 },
	{0x52, "LD D, D", 0, 4 },
	{0x53, "LD D, E", 0, 4 },
	{0x54, "LD D, H", 0, 4 },
	{0x55, "LD D, L", 0, 4 },
	{0x56, "LD D, (HL)", 0, 8 },
	{0x57, "LD D, A", 0, 4 },
	{0x58, "LD E, B", 0, 4 },
	{0x59, "LD E, C", 0, 4 },
	{0x5A, "LD E, D", 0, 4 },
	{0x5B, "LD E, E", 0, 4 },
	{0x5C, "LD E, H", 0, 4 },
	{0x5D, "LD E, L", 0, 4 },
	{0x5E, "LD E, (HL)", 0, 8 },
	{0x5F, "LD E, A", 0, 4 },
	{0x60, "LD H, B", 0, 4 },
	{0x61, "LD H, C", 0, 4 },
	{0x62, "LD H, D", 0, 4 },
	{0x63, "LD H, E", 0, 4 },
	{0x64, "LD H, H", 0, 4 },
	{0x65, "LD H, L", 0, 4 },
	{0x66, "LD H, (HL)", 0, 8 },
	{0x67, "LD H, A", 0, 4 },
	{0x68, "LD L, B", 0, 4 },
	{0x69, "LD L, C", 0, 4 },
	{0x6A, "LD L, D", 0, 4 },
	{0x6B, "LD L, E", 0, 4 },
	{0x6C, "LD L, H", 0, 4 },
	{0x6D, "LD L, L", 0, 4 },
	{0x6E, "LD L, (HL)", 0, 8 },
	{0x6F, "LD L, A", 0, 4 },
	{0x70, "LD (HL), B", 0, 8 },
	{0x71, "LD (HL), C", 0, 8 },
	{0x72, "LD (HL), D", 0, 8 },
	{0x73, "LD (HL), E", 0, 8 },
	{0x74, "LD (HL), H", 0, 8 },
	{0x75, "LD (HL), L", 0, 8 },
	{0x76, "HALT", 0, 4 },
	{0x77, "LD (HL), A", 0, 8 },
	{0x78, "LD A, B", 0, 4 },
	{0x79, "LD A, C", 0, 4 },
	{0x7A, "LD A, D", 0, 4 },
	{0x7B, "LD A, E", 0, 4 },
	{0x7C, "LD A, H", 0, 4 },
	{0x7D, "LD A, L", 0, 4 },
	{0x7E, "LD A, (HL)", 0, 8 },
	{0x7F, "LD A, A", 0, 4 },
	{0x80, "ADD A, B", 0, 4 },
	{0x81, "ADD A, C", 0, 4 },
	{0x82, "ADD A, D", 0, 4 },
	{0x83, "ADD A, E", 0, 4 },
	{0x84, "ADD A, H", 0, 4 },
	{0x85, "ADD A, L", 0, 4 },
	{0x86, "ADD A, (HL)", 0, 8 },
	{0x87, "ADD A, A", 0, 4 },
	{0x88, "ADC A, B", 0, 4 },
	{0x89, "ADC A, C", 0, 4 },
	{0x8A, "ADC A, D", 0, 4 },
	{0x8B, "ADC A, E", 0, 4 },
	{0x8C, "ADC A, H", 0, 4 },
	{0x8D, "ADC A, L", 0, 4 },
	{0x8E, "ADC A, (HL)", 0, 8 },
	{0x8F, "ADC A, A", 0, 4 },
	{0x90, "SUB A, B", 0, 4 },
	{0x91, "SUB A, C", 0, 4 },
	{0x92, "SUB A, D", 0, 4 },
	{0x93, "SUB A, E", 0, 4 },
	{0x94, "SUB A, H", 0, 4 },
	{0x95, "SUB A, L", 0, 4 },
	{0x96, "SUB A, (HL)", 0, 8 },
	{0x97, "SUB A, A", 0, 4 },
	{0x98, "SBC A, B", 0, 4 },
	{0x99, "SBC A, C", 0, 4 },
	{0x9A, "SBC A, D", 0, 4 },
	{0x9B, "SBC A, E", 0, 4 },
	{0x9C, "SBC A, H", 0, 4 },
	{0x9D, "SBC A, L", 0, 4 },
	{0x9E, "SBC A, (HL)", 0, 8 },
	{0x9F, "SBC A, A", 0, 4 },
	{0xA0, "AND B", 0, 4 },
	{0xA1, "AND C", 0, 4 },
	{0xA2, "AND D", 0, 4 },
	{0xA3, "AND E", 0, 4 },
	{0xA4, "AND H", 0, 4 },
	{0xA5, "AND L", 0, 4 },
	{0xA6, "AND (HL)", 0, 8 },
	{0xA7, "AND A", 0, 4 },
	{0xA8, "XOR B", 0, 4 },
	{0xA9, "XOR C", 0, 4 },
	{0xAA, "XOR D", 0, 4 },
	{0xAB, "XOR E", 0, 4 },
	{0xAC, "XOR H", 0, 4 },
	{0xAD, "XOR L", 0, 4 },
	{0xAE, "XOR (HL)", 0, 8 },
	{0xAF, "XOR A", 0, 4 },
	{0xB0, "OR B", 0, 4 },
	{0xB1, "OR C", 0, 4 },
	{0xB2, "OR D", 0, 4 },
	{0xB3, "OR E", 0, 4 },
	{0xB4, "OR H", 0, 4 },
	{0xB5, "OR L", 0, 4 },
	{0xB6, "OR (HL)", 0, 8 },
	{0xB7, "OR A", 0, 4 },
	{0xB8, "CP B", 0, 4 },
	{0xB9, "CP C", 0, 4 },
	{0xBA, "CP D", 0, 4 },
	{0xBB, "CP E", 0, 4 },
	{0xBC, "CP H", 0, 4 },
	{0xBD, "CP L", 0, 4 },
	{0xBE, "CP (HL)", 0, 8 },
	{0xBF, "CP A", 0, 4 },
	{0xC0, "RET NZ", 0, 8 },
	{0xC1, "POP BC", 0, 12 },
	{0xC2, "JP NZ, $%04X", 2, 12 },
	{0xC3, "JP $%04X", 2, 16 },
	{0xC4, "CALL NZ, $%04X", 2, 12 },
	{0xC5, "PUSH BC", 0, 16 },
	{0xC6, "ADD A, $%02X", 1, 8 },
	{0xC7, "RST 0", 0, 16 },
	{0xC8, "RET Z", 0, 8 },
	{0xC9, "RET", 0, 16 },
	{0xCA, "JP Z, $%04X", 2, 12 },
	{0xCB, "$%02X", 1, 4 },
	{0xCC, "CALL Z, $%04X", 2, 12 },
	{0xCD, "CALL $%04X", 2, 24 },
	{0xCE, "ADC A, $%02X", 1, 8 },
	{0xCF, "RST 8", 0, 16 },
	{0xD0, "RET NC", 0, 8 },
	{0xD1, "POP DE", 0, 12 },
	{0xD2, "JP NC, $%04X", 2, 12 },
	{0xD3, "XX", 0, 0 },
	{0xD4, "CALL NC, $%04X", 2, 12 },
	{0xD5, "PUSH DE", 0, 16 },
	{0xD6, "SUB A, $%02X", 1, 8 },
	{0xD7, "RST 10", 0, 16 },
	{0xD8, "RET C", 0, 8 },
	{0xD9, "RETI", 0, 16 },
	{0xDA, "JP C, $%04X", 1, 12 },
	{0xDB, "XX", 0, 0 },
	{0xDC, "CALL C, $%04X", 2, 12 },
	{0xDD, "XX", 0, 0 },
	{0xDE, "SBC A, $%02X", 1, 8 },
	{0xDF, "RST 18", 0, 16 },
	{0xE0, "LDH ($%02X), A", 1, 12 },
	{0xE1, "POP HL", 0, 12 },
	{0xE2, "LDH (C), A", 0, 8 },
	{0xE3, "XX", 0, 0 },
	{0xE4, "XX", 0, 0 },
	{0xE5, "PUSH HL", 0, 16 },
	{0xE6, "AND $%02X", 1, 8 },
	{0xE7, "RST 20", 0, 16 },
	{0xE8, "ADD SP, $%02X", 1, 16 },
	{0xE9, "JP (HL)", 0, 4 },
	{0xEA, "LD ($%04X), A", 2, 16 },
	{0xEB, "XX", 0, 0 },
	{0xEC, "XX", 0, 0 },
	{0xED, "XX", 0, 0 },
	{0xEE, "XOR $%02X", 1, 8 },
	{0xEF, "RST 28", 0, 16 },
	{0xF0, "LDH A, ($%02X)", 1, 12 },
	{0xF1, "POP AF", 0, 12 },
	{0xF2, "LD A, (C)", 0, 8 },
	{0xF3, "DI", 0, 4 },
	{0xF4, "XX", 0, 0 },
	{0xF5, "PUSH AF", 0, 16 },
	{0xF6, "OR $%02X", 1, 8 },
	{0xF7, "RST 30", 0, 16 },
	{0xF8, "LDHL SP, $%02X", 1, 12 },
	{0xF9, "LD SP, HL", 0, 8 },
	{0xFA, "LD A, ($%04X)", 2, 16 },
	{0xFB, "EI", 0, 4 },
	{0xFC, "XX", 0, 0 },
	{0xFD, "XX", 0, 0 },
	{0xFE, "CP $%02X", 1, 8 },
	{0xFF, "RST 38", 0, 16 },
};

constexpr cpu_instruction g_gbExtInstructions[256] =
{
	{0x00, "RLC B", 0, 8 },
	{0x01, "RLC C", 0, 8 },
	{0x02, "RLC D", 0, 8 },
	{0x03, "RLC E", 0, 8 },
	{0x04, "RLC H", 0, 8 },
	{0x05, "RLC L", 0, 8 },
	{0x06, "RLC (HL)", 0, 16 },
	{0x07, "RLC A", 0, 8 },
	{0x08, "RRC B", 0, 8 },
	{0x09, "RRC C", 0, 8 },
	{0x0A, "RRC D", 0, 8 },
	{0x0B, "RRC E", 0, 8 },
	{0x0C, "RRC H", 0, 8 },
	{0x0D, "RRC L", 0, 8 },
	{0x0E, "RRC (HL)", 0, 16 },
	{0x0F, "RRC A", 0, 8 },
	{0x10, "RL B", 0, 8 },
	{0x11, "RL C", 0, 8 },
	{0x12, "RL D", 0, 8 },
	{0x13, "RL E", 0, 8 },
	{0x14, "RL H", 0, 8 },
	{0x15, "RL L", 0, 8 },
	{0x16, "RL (HL)", 0, 16 },
	{0x17, "RL A", 0, 8 },
	{0x18, "RR B", 0, 8 },
	{0x19, "RR C", 0, 8 },
	{0x1A, "RR D", 0, 8 },
	{0x1B, "RR E", 0, 8 },
	{0x1C, "RR H", 0, 8 },
	{0x1D, "RR L", 0, 8 },
	{0x1E, "RR (HL)", 0, 16 },
	{0x1F, "RR A", 0, 8 },
	{0x20, "SLA B", 0, 8 },
	{0x21, "SLA C", 0, 8 },
	{0x22, "SLA D", 0, 8 },
	{0x23, "SLA E", 0, 8 },
	{0x24, "SLA H", 0, 8 },
	{0x25, "SLA L", 0, 8 },
	{0x26, "SLA (HL)", 0, 16 },
	{0x27, "SLA A", 0, 8 },
	{0x28, "SRA B", 0, 8 },
	{0x29, "SRA C", 0, 8 },
	{0x2A, "SRA D", 0, 8 },
	{0x2B, "SRA E", 0, 8 },
	{0x2C, "SRA H", 0, 8 },
	{0x2D, "SRA L", 0, 8 },
	{0x2E, "SRA (HL)", 0, 16 },
	{0x2F, "SRA A", 0, 8 },
	{0x30, "SWAP B", 0, 8 },
	{0x31, "SWAP C", 0, 8 },
	{0x32, "SWAP D", 0, 8 },
	{0x33, "SWAP E", 0, 8 },
	{0x34, "SWAP H", 0, 8 },
	{0x35, "SWAP L", 0, 8 },
	{0x36, "SWAP (HL)", 0, 16 },
	{0x37, "SWAP A", 0, 8 },
	{0x38, "SRL B", 0, 8 },
	{0x39, "SRL C", 0, 8 },
	{0x3A, "SRL D", 0, 8 },
	{0x3B, "SRL E", 0, 8 },
	{0x3C, "SRL H", 0, 8 },
	{0x3D, "SRL L", 0, 8 },
	{0x3E, "SRL (HL)", 0, 16 },
	{0x3F, "SRL A", 0, 8 },
	{0x40, "BIT 0, B", 0, 8 },
	{0x41, "BIT 0, C", 0, 8 },
	{0x42, "BIT 0, D", 0, 8 },
	{0x43, "BIT 0, E", 0, 8 },
	{0x44, "BIT 0, H", 0, 8 },
	{0x45, "BIT 0, L", 0, 8 },
	{0x46, "BIT 0, (HL)", 0, 12 },
	{0x47, "BIT 0, A", 0, 8 },
	{0x48, "BIT 1, B", 0, 8 },
	{0x49, "BIT 1, C", 0, 8 },
	{0x4A, "BIT 1, D", 0, 8 },
	{0x4B, "BIT 1, E", 0, 8 },
	{0x4C, "BIT 1, H", 0, 8 },
	{0x4D, "BIT 1, L", 0, 8 },
	{0x4E, "BIT 1, (HL)", 0, 12 },
	{0x4F, "BIT 1, A", 0, 8 },
	{0x50, "BIT 2, B", 0, 8 },
	{0x51, "BIT 2, C", 0, 8 },
	{0x52, "BIT 2, D", 0, 8 },
	{0x53, "BIT 2, E", 0, 8 },
	{0x54, "BIT 2, H", 0, 8 },
	{0x55, "BIT 2, L", 0, 8 },
	{0x56, "BIT 2, (HL)", 0, 12 },
	{0x57, "BIT 2, A", 0, 8 },
	{0x58, "BIT 3, B", 0, 8 },
	{0x59, "BIT 3, C", 0, 8 },
	{0x5A, "BIT 3, D", 0, 8 },
	{0x5B, "BIT 3, E", 0, 8 },
	{0x5C, "BIT 3, H", 0, 8 },
	{0x5D, "BIT 3, L", 0, 8 },
	{0x5E, "BIT 3, (HL)", 0, 12 },
	{0x5F, "BIT 3, A", 0, 8 },
	{0x60, "BIT 4, B", 0, 8 },
	{0x61, "BIT 4, C", 0, 8 },
	{0x62, "BIT 4, D", 0, 8 },
	{0x63, "BIT 4, E", 0, 8 },
	{0x64, "BIT 4, H", 0, 8 },
	{0x65, "BIT 4, L", 0, 8 },
	{0x66, "BIT 4, (HL)", 0, 12 },
	{0x67, "BIT 4, A", 0, 8 },
	{0x68, "BIT 5, B", 0, 8 },
	{0x69, "BIT 5, C", 0, 8 },
	{0x6A, "BIT 5, D", 0, 8 },
	{0x6B, "BIT 5, E", 0, 8 },
	{0x6C, "BIT 5, H", 0, 8 },
	{0x6D, "BIT 5, L", 0, 8 },
	{0x6E, "BIT 5, (HL)", 0, 12 },
	{0x6F, "BIT 5, A", 0, 8 },
	{0x70, "BIT 6, B", 0, 8 },
	{0x71, "BIT 6, C", 0, 8 },
	{0x72, "BIT 6, D", 0, 8 },
	{0x73, "BIT 6, E", 0, 8 },
	{0x74, "BIT 6, H", 0, 8 },
	{0x75, "BIT 6, L", 0, 8 },
	{0x76, "BIT 6, (HL)", 0, 12 },
	{0x77, "BIT 6, A", 0, 8 },
	{0x78, "BIT 7, B", 0, 8 },
	{0x79, "BIT 7, C", 0, 8 },
	{0x7A, "BIT 7, D", 0, 8 },
	{0x7B, "BIT 7, E", 0, 8 },
	{0x7C, "BIT 7, H", 0, 8 },
	{0x7D, "BIT 7, L", 0, 8 },
	{0x7E, "BIT 7, (HL)", 0, 12 },
	{0x7F, "BIT 7, A", 0, 8 },
	{0x80, "RES 0, B", 0, 8 },
	{0x81, "RES 0, C", 0, 8 },
	{0x82, "RES 0, D", 0, 8 },
	{0x83, "RES 0, E", 0, 8 },
	{0x84, "RES 0, H", 0, 8 },
	{0x85, "RES 0, L", 0, 8 },
	{0x86, "RES 0, (HL)", 0, 16 },
	{0x87, "RES 0, A", 0, 8 },
	{0x88, "RES 1, B", 0, 8 },
	{0x89, "RES 1, C", 0, 8 },
	{0x8A, "RES 1, D", 0, 8 },
	{0x8B, "RES 1, E", 0, 8 },
	{0x8C, "RES 1, H", 0, 8 },
	{0x8D, "RES 1, L", 0, 8 },
	{0x8E, "RES 1, (HL)", 0, 16 },
	{0x8F, "RES 1, A", 0, 8 },
	{0x90, "RES 2, B", 0, 8 },
	{0x91, "RES 2, C", 0, 8 },
	{0x92, "RES 2, D", 0, 8 },
	{0x93, "RES 2, E", 0, 8 },
	{0x94, "RES 2, H", 0, 8 },
	{0x95, "RES 2, L", 0, 8 },
	{0x96, "RES 2, (HL)", 0, 16 },
	{0x97, "RES 2, A", 0, 8 },
	{0x98, "RES 3, B", 0, 8 },
	{0x99, "RES 3, C", 0, 8 },
	{0x9A, "RES 3, D", 0, 8 },
	{0x9B, "RES 3, E", 0, 8 },
	{0x9C, "RES 3, H", 0, 8 },
	{0x9D, "RES 3, L", 0, 8 },
	{0x9E, "RES 3, (HL)", 0, 16 },
	{0x9F, "RES 3, A", 0, 8 },
	{0xA0, "RES 4, B", 0, 8 },
	{0xA1, "RES 4, C", 0, 8 },
	{0xA2, "RES 4, D", 0, 8 },
	{0xA3, "RES 4, E", 0, 8 },
	{0xA4, "RES 4, H", 0, 8 },
	{0xA5, "RES 4, L", 0, 8 },
	{0xA6, "RES 4, (HL)", 0, 16 },
	{0xA7, "RES 4, A", 0, 8 },
	{0xA8, "RES 5, B", 0, 8 },
	{0xA9, "RES 5, C", 0, 8 },
	{0xAA, "RES 5, D", 0, 8 },
	{0xAB, "RES 5, E", 0, 8 },
	{0xAC, "RES 5, H", 0, 8 },
	{0xAD, "RES 5, L", 0, 8 },
	{0xAE, "RES 5, (HL)", 0, 16 },
	{0xAF, "RES 5, A", 0, 8 },
	{0xB0, "RES 6, B", 0, 8 },
	{0xB1, "RES 6, C", 0, 8 },
	{0xB2, "RES 6, D", 0, 8 },
	{0xB3, "RES 6, E", 0, 8 },
	{0xB4, "RES 6, H", 0, 8 },
	{0xB5, "RES 6, L", 0, 8 },
	{0xB6, "RES 6, (HL)", 0, 16 },
	{0xB7, "RES 6, A", 0, 8 },
	{0xB8, "RES 7, B", 0, 8 },
	{0xB9, "RES 7, C", 0, 8 },
	{0xBA, "RES 7, D", 0, 8 },
	{0xBB, "RES 7, E", 0, 8 },
	{0xBC, "RES 7, H", 0, 8 },
	{0xBD, "RES 7, L", 0, 8 },
	{0xBE, "RES 7, (HL)", 0, 16 },
	{0xBF, "RES 7, A", 0, 8 },
	{0xC0, "SET 0, B", 0, 8 },
	{0xC1, "SET 0, C", 0, 8 },
	{0xC2, "SET 0, D", 0, 8 },
	{0xC3, "SET 0, E", 0, 8 },
	{0xC4, "SET 0, H", 0, 8 },
	{0xC5, "SET 0, L", 0, 8 },
	{0xC6, "SET 0, (HL)", 0, 16 },
	{0xC7, "SET 0, A", 0, 8 },
	{0xC8, "SET 1, B", 0, 8 },
	{0xC9, "SET 1, C", 0, 8 },
	{0xCA, "SET 1, D", 0, 8 },
	{0xCB, "SET 1, E", 0, 8 },
	{0xCC, "SET 1, H", 0, 8 },
	{0xCD, "SET 1, L", 0, 8 },
	{0xCE, "SET 1, (HL)", 0, 16 },
	{0xCF, "SET 1, A", 0, 8 },
	{0xD0, "SET 2, B", 0, 8 },
	{0xD1, "SET 2, C", 0, 8 },
	{0xD2, "SET 2, D", 0, 8 },
	{0xD3, "SET 2, E", 0, 8 },
	{0xD4, "SET 2, H", 0, 8 },
	{0xD5, "SET 2, L", 0, 8 },
	{0xD6, "SET 2, (HL)", 0, 16 },
	{0xD7, "SET 2, A", 0, 8 },
	{0xD8, "SET 3, B", 0, 8 },
	{0xD9, "SET 3, C", 0, 8 },
	{0xDA, "SET 3, D", 0, 8 },
	{0xDB, "SET 3, E", 0, 8 },
	{0xDC, "SET 3, H", 0, 8 },
	{0xDD, "SET 3, L", 0, 8 },
	{0xDE, "SET 3, (HL)", 0, 16 },
	{0xDF, "SET 3, A", 0, 8 },
	{0xE0, "SET 4, B", 0, 8 },
	{0xE1, "SET 4, C", 0, 8 },
	{0xE2, "SET 4, D", 0, 8 },
	{0xE3, "SET 4, E", 0, 8 },
	{0xE4, "SET 4, H", 0, 8 },
	{0xE5, "SET 4, L", 0, 8 },
	{0xE6, "SET 4, (HL)", 0, 16 },
	{0xE7, "SET 4, A", 0, 8 },
	{0xE8, "SET 5, B", 0, 8 },
	{0xE9, "SET 5, C", 0, 8 },
	{0xEA, "SET 5, D", 0, 8 },
	{0xEB, "SET 5, E", 0, 8 },
	{0xEC, "SET 5, H", 0, 8 },
	{0xED, "SET 5, L", 0, 8 },
	{0xEE, "SET 5, (HL)", 0, 16 },
	{0xEF, "SET 5, A", 0, 8 },
	{0xF0, "SET 6, B", 0, 8 },
	{0xF1, "SET 6, C", 0, 8 },
	{0xF2, "SET 6, D", 0, 8 },
	{0xF3, "SET 6, E", 0, 8 },
	{0xF4, "SET 6, H", 0, 8 },
	{0xF5, "SET 6, L", 0, 8 },
	{0xF6, "SET 6, (HL)", 0, 16 },
	{0xF7, "SET 6, A", 0, 8 },
	{0xF8, "SET 7, B", 0, 8 },
	{0xF9, "SET 7, C", 0, 8 },
	{0xFA, "SET 7, D", 0, 8 },
	{0xFB, "SET 7, E", 0, 8 },
	{0xFC, "SET 7, H", 0, 8 },
	{0xFD, "SET 7, L", 0, 8 },
	{0xFE, "SET 7, (HL)", 0, 16 },
	{0xFF, "SET 7, A", 0, 8 },
};
