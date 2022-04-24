#pragma once

#include "data.h"

#include <memory>

class memory;
class graphics;

class processor
{
public:
	static bool s_debugRegisters;
	static bool s_breakOnCALL;
	static bool s_breakOnRET;
	static bool s_breakOnJR;
	static bool s_stepOver;

private:
	std::shared_ptr<memory> m_memory{};
	std::shared_ptr<graphics> m_graphics{};

	cpu_registers m_registers{};

	unsigned int m_cpu_clocks{};

	bool m_ime{};
	bool m_ima{};
	bool m_halt{};
	bool m_stop{};
	bool m_speed_switch{};

	bool m_double_speed_mode{};

	unsigned short m_clocks_timer{};
	unsigned short m_timer{};

public:
	processor(std::shared_ptr<memory> memory, std::shared_ptr<graphics> graphics);

	bool execute_instruction();

private:
	bool execute_opcode(unsigned char opcode);

	void execute_cycle();

	void log_registers(cpu_registers& regs);

	void update_timers(unsigned int clocks);
	void check_interrupts();

	bool execute_extension_opcode(); // 0xCB

	void load_8bit(unsigned char& reg);
	void load_8bit(unsigned char& reg, unsigned char val);
	void load_8bit_mem(unsigned char& reg, unsigned short mem);

	void load_16bit(unsigned short& reg);
	void load_16bit(unsigned short& reg, unsigned short val);

	void dec_16bit(unsigned short& val);
	void inc_16bit(unsigned short& val);

	void add_8bit(unsigned char& reg, unsigned char val, bool immediate, bool carry);
	void add_8bit_mem(unsigned char& reg, unsigned short mem, bool carry);

	void sub_8bit(unsigned char& reg, unsigned char val, bool immediate, bool carry);
	void sub_8bit_mem(unsigned char& reg, unsigned short mem, bool carry);

	void and_8bit(unsigned char& reg, unsigned char val, bool immediate);
	void and_8bit_mem(unsigned char& reg, unsigned short mem);

	void or_8bit(unsigned char& reg, unsigned char val, bool immediate);
	void or_8bit_mem(unsigned char& reg, unsigned short mem);

	void xor_8bit(unsigned char& reg, unsigned char val, bool immediate);
	void xor_8bit_mem(unsigned char& reg, unsigned short mem);

	void cp_8bit(unsigned char& reg, unsigned char val, bool immediate);
	void cp_8bit_mem(unsigned char& reg, unsigned short mem);

	void inc_8bit(unsigned char& reg);
	void inc_8bit_mem(unsigned short& mem);
	void dec_8bit(unsigned char& reg);
	void dec_8bit_mem(unsigned short& mem);

	void add_16bit(unsigned short& reg, unsigned short val);

	void jump_8bit_flag(int flag, bool on);
	void jump_8bit();
	void jump_16bit_flag(int flag, bool on);
	void jump_16bit(unsigned short addr, bool immediate);

	void call_16bit();
	void call_flag(int flag, bool on);

	void ret();
	void reti();
	void ret_flag(int flag, bool on);

	void push_stack(unsigned short val);
	void pop_stack(unsigned short& reg);

	void rst(unsigned char val);

	void di();
	void ei();

	void reset_bit(unsigned char& reg, int bit);
	void reset_bit_mem(unsigned short addr, int bit);

	void set_bit(unsigned char& reg, int bit);
	void set_bit_mem(unsigned short addr, int bit);

	void test_bit(unsigned char& reg, int bit);
	void test_bit_mem(unsigned short addr, int bit);

	void daa();

	void rotate_left(unsigned char& reg, bool carryBit7);
	void rotate_left_mem(unsigned short addr, bool carryBit7);

	void rotate_right(unsigned char& reg, bool carryBit7);
	void rotate_right_mem(unsigned short addr, bool carryBit7);

	void shift_right(unsigned char& reg, bool keepMSB);
	void shift_right_mem(unsigned short addr, bool keepMSB);

	void shift_left(unsigned char& reg);
	void shift_left_mem(unsigned short addr);

	void swap(unsigned char& reg);
	void swap_mem(unsigned short addr);
};
