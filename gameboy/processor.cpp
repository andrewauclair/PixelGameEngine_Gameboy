#include "processor.h"
#include "memory.h"
#include "graphics.h"

#include <iostream>
#include <iomanip>

bool processor::s_debugRegisters = false;
bool processor::s_breakOnCALL = false;
bool processor::s_breakOnRET = false;
bool processor::s_breakOnJR = false;
bool processor::s_stepOver = false;

processor::processor(std::shared_ptr<memory> memory, std::shared_ptr<graphics> graphics)
	: m_memory{ memory },
	  m_graphics{ graphics }
{
}

bool processor::execute_instruction()
{
	bool ret = true;
	unsigned short opcode = 0;
	static unsigned int lastClockCycles = m_cpu_clocks;
	lastClockCycles = m_cpu_clocks;

	if (m_registers.pc == 0x1a40)// || m_registers.pc == 0xc2d5 || m_registers.pc == 0xc2e1)
	{
		//s_debugRegisters = true;
	}

	check_interrupts();

	if (!m_halt)
	{
		opcode = m_memory->read_byte(m_registers.pc++);

		execute_cycle();

		const cpu_instruction& instruction = g_gbInstructions[opcode];

		unsigned short oneArg = m_memory->read_byte(m_registers.pc);
		unsigned short twoArg = m_memory->read_short(m_registers.pc);

		ret = execute_opcode(opcode);

		if (ret)
		{
			/*if (s_debugRegisters)
			{
				if (opcode == 0xCB)
				{
					std::cout << "Executing ext opcode 0xCB 0x" << oneArg << " " << g_gbExtInstructions[oneArg].instruction << std::endl;
				}
				else
				{
					std::cout << "Executing opcode 0x" << std::hex << opcode << " ";

					char buffer[50];
					if (instruction.length == 1)
					{
						sprintf_s(buffer, instruction.instruction, oneArg);
					}
					else if (instruction.length == 2)
					{
						sprintf_s(buffer, instruction.instruction, twoArg);
					}
					else
					{
						sprintf_s(buffer, "%s", instruction.instruction);
					}

					std::cout << buffer << std::endl;
				}
			}*/
		}
	}
	else
	{
		execute_cycle();
	}

	int clockChange = m_cpu_clocks - lastClockCycles;

	if (s_debugRegisters)
	{
		log_registers(m_registers);
	}

	return ret;
}

void processor::execute_cycle()
{
	m_cpu_clocks += 4;

	m_graphics->update(4);
	update_timers(4);
}

bool processor::execute_opcode(unsigned char opcode)
{
	const cpu_instruction& instr = g_gbInstructions[opcode];
	bool retval = true;

	switch (opcode)
	{
	case 0x00: break;
	case 0x01: load_16bit(m_registers.bc); break;
	case 0x02:
	{
		m_memory->write_byte(m_registers.bc, m_registers.a);
		execute_cycle();

		break;
	}
	case 0x03: inc_16bit(m_registers.bc); break;
	case 0x04: inc_8bit(m_registers.b); break;
	case 0x05: dec_8bit(m_registers.b); break;
	case 0x06: load_8bit(m_registers.b); break;
	case 0x07: rotate_left(m_registers.a, true); m_registers.z_f = 0; break;
	case 0x08:
	{
		unsigned char n1 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		unsigned char n2 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		unsigned short addr = (n2 << 8) | n1;
		m_memory->write_byte(addr, m_registers.sp & 0x00FF);
		execute_cycle();
		m_memory->write_byte(addr + 1, (m_registers.sp & 0xFF00) >> 8);
		execute_cycle();

		break;
	}
	case 0x09: add_16bit(m_registers.hl, m_registers.bc); break;
	case 0x0A: load_8bit_mem(m_registers.a, m_registers.bc); break;
	case 0x0B: dec_16bit(m_registers.bc); break;
	case 0x0C: inc_8bit(m_registers.c); break;
	case 0x0D: dec_8bit(m_registers.c); break;
	case 0x0E: load_8bit(m_registers.c); break;
	case 0x0F: rotate_right(m_registers.a, true); m_registers.z_f = 0; break;
	case 0x10:
	{
		// todo check if we're the correct platform for this
		m_stop = true;
		
		uint8_t speed{ m_memory->read_byte_from_raw_memory(0xff4d) };

		std::cout << "before speed " << std::hex << static_cast<unsigned short>(speed) << std::endl;

		// we're looking to switch speeds
		if (speed & 0x1)
		{
			m_double_speed_mode = !m_double_speed_mode;
			speed &= 0x7E;
			speed |= (m_double_speed_mode) ? 0x80 : 0;
			m_stop = false;
		}

		std::cout << "after speed " << std::hex << (unsigned short)speed << std::endl;

		m_memory->write_byte_to_raw_memory(0xff4d, speed);

		break;
	}
	case 0x11: load_16bit(m_registers.de); break;
	case 0x12: m_memory->write_byte(m_registers.de, m_registers.a); execute_cycle(); break;
	case 0x13: inc_16bit(m_registers.de); break;
	case 0x14: inc_8bit(m_registers.d); break;
	case 0x15: dec_8bit(m_registers.d); break;
	case 0x16: load_8bit(m_registers.d); break;
	case 0x17: rotate_left(m_registers.a, false); m_registers.z_f = 0; break;
	case 0x18: jump_8bit(); break;
	case 0x19: add_16bit(m_registers.hl, m_registers.de); break;
	case 0x1A: load_8bit_mem(m_registers.a, m_registers.de); break;
	case 0x1B: dec_16bit(m_registers.de); break;
	case 0x1C: inc_8bit(m_registers.e); break;
	case 0x1D: dec_8bit(m_registers.e); break;
	case 0x1E: load_8bit(m_registers.e); break;
	case 0x1F: rotate_right(m_registers.a, false); m_registers.z_f = 0; break;
	case 0x20: jump_8bit_flag(0x80, false); break;
	case 0x21: load_16bit(m_registers.hl); break;
	case 0x22: m_memory->write_byte(m_registers.hl, m_registers.a); execute_cycle(); m_registers.hl++; break;
	case 0x23: inc_16bit(m_registers.hl); break;
	case 0x24: inc_8bit(m_registers.h); break;
	case 0x25: dec_8bit(m_registers.h); break;
	case 0x26: load_8bit(m_registers.h); break;
	case 0x27: daa(); break;
	case 0x28: jump_8bit_flag(0x80, true); break;
	case 0x29: add_16bit(m_registers.hl, m_registers.hl); break;
	case 0x2A: load_8bit_mem(m_registers.a, m_registers.hl); m_registers.hl++; break;
	case 0x2B: dec_16bit(m_registers.hl); break;
	case 0x2C: inc_8bit(m_registers.l); break;
	case 0x2D: dec_8bit(m_registers.l); break;
	case 0x2E: load_8bit(m_registers.l); break;
	case 0x2F: m_registers.a = ~(m_registers.a); m_registers.n_f = 1; m_registers.h_f = 1; break;
	case 0x30: jump_8bit_flag(0x10, false); break;
	case 0x31: load_16bit(m_registers.sp); break;
	case 0x32: m_memory->write_byte(m_registers.hl, m_registers.a); execute_cycle(); m_registers.hl--; break;
	case 0x33: inc_16bit(m_registers.sp); break;
	case 0x34: inc_8bit_mem(m_registers.hl); break;
	case 0x35: dec_8bit_mem(m_registers.hl); break;
	case 0x36:
	{
		unsigned char n = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		m_memory->write_byte(m_registers.hl, n);
		execute_cycle();
	}break;
	case 0x37: m_registers.n_f = 0; m_registers.h_f = 0; m_registers.c_f = 1; break;
	case 0x38: jump_8bit_flag(0x10, true); break;
	case 0x39: add_16bit(m_registers.hl, m_registers.sp); break;
	case 0x3A: load_8bit_mem(m_registers.a, m_registers.hl); m_registers.hl--; break;
	case 0x3B: dec_16bit(m_registers.sp); break;
	case 0x3C: inc_8bit(m_registers.a); break;
	case 0x3D: dec_8bit(m_registers.a); break;
	case 0x3E: load_8bit(m_registers.a); break;
	case 0x3F: m_registers.n_f = 0; m_registers.h_f = 0; m_registers.c_f = (m_registers.c_f == 0) ? 1 : 0; break;
	case 0x40: load_8bit(m_registers.b, m_registers.b); break;
	case 0x41: load_8bit(m_registers.b, m_registers.c); break;
	case 0x42: load_8bit(m_registers.b, m_registers.d); break;
	case 0x43: load_8bit(m_registers.b, m_registers.e); break;
	case 0x44: load_8bit(m_registers.b, m_registers.h); break;
	case 0x45: load_8bit(m_registers.b, m_registers.l); break;
	case 0x46: load_8bit_mem(m_registers.b, m_registers.hl); break;
	case 0x47: load_8bit(m_registers.b, m_registers.a); break;
	case 0x48: load_8bit(m_registers.c, m_registers.b); break;
	case 0x49: load_8bit(m_registers.c, m_registers.c); break;
	case 0x4A: load_8bit(m_registers.c, m_registers.d); break;
	case 0x4B: load_8bit(m_registers.c, m_registers.e); break;
	case 0x4C: load_8bit(m_registers.c, m_registers.h); break;
	case 0x4D: load_8bit(m_registers.c, m_registers.l); break;
	case 0x4E: load_8bit_mem(m_registers.c, m_registers.hl); break;
	case 0x4F: load_8bit(m_registers.c, m_registers.a); break;
	case 0x50: load_8bit(m_registers.d, m_registers.b); break;
	case 0x51: load_8bit(m_registers.d, m_registers.c); break;
	case 0x52: load_8bit(m_registers.d, m_registers.d); break;
	case 0x53: load_8bit(m_registers.d, m_registers.e); break;
	case 0x54: load_8bit(m_registers.d, m_registers.h); break;
	case 0x55: load_8bit(m_registers.d, m_registers.l); break;
	case 0x56: load_8bit_mem(m_registers.d, m_registers.hl); break;
	case 0x57: load_8bit(m_registers.d, m_registers.a); break;
	case 0x58: load_8bit(m_registers.e, m_registers.b); break;
	case 0x59: load_8bit(m_registers.e, m_registers.c); break;
	case 0x5A: load_8bit(m_registers.e, m_registers.d); break;
	case 0x5B: load_8bit(m_registers.e, m_registers.e); break;
	case 0x5C: load_8bit(m_registers.e, m_registers.h); break;
	case 0x5D: load_8bit(m_registers.e, m_registers.l); break;
	case 0x5E: load_8bit_mem(m_registers.e, m_registers.hl); break;
	case 0x5F: load_8bit(m_registers.e, m_registers.a); break;
	case 0x60: load_8bit(m_registers.h, m_registers.b); break;
	case 0x61: load_8bit(m_registers.h, m_registers.c); break;
	case 0x62: load_8bit(m_registers.h, m_registers.d); break;
	case 0x63: load_8bit(m_registers.h, m_registers.e); break;
	case 0x64: load_8bit(m_registers.h, m_registers.h); break;
	case 0x65: load_8bit(m_registers.h, m_registers.l); break;
	case 0x66: load_8bit_mem(m_registers.h, m_registers.hl); break;
	case 0x67: load_8bit(m_registers.h, m_registers.a); break;
	case 0x68: load_8bit(m_registers.l, m_registers.b); break;
	case 0x69: load_8bit(m_registers.l, m_registers.c); break;
	case 0x6A: load_8bit(m_registers.l, m_registers.d); break;
	case 0x6B: load_8bit(m_registers.l, m_registers.e); break;
	case 0x6C: load_8bit(m_registers.l, m_registers.h); break;
	case 0x6D: load_8bit(m_registers.l, m_registers.l); break;
	case 0x6E: load_8bit_mem(m_registers.l, m_registers.hl); break;
	case 0x6F: load_8bit(m_registers.l, m_registers.a); break;
	case 0x70: m_memory->write_byte(m_registers.hl, m_registers.b); execute_cycle(); break;
	case 0x71: m_memory->write_byte(m_registers.hl, m_registers.c); execute_cycle(); break;
	case 0x72: m_memory->write_byte(m_registers.hl, m_registers.d); execute_cycle(); break;
	case 0x73: m_memory->write_byte(m_registers.hl, m_registers.e); execute_cycle(); break;
	case 0x74: m_memory->write_byte(m_registers.hl, m_registers.h); execute_cycle(); break;
	case 0x75: m_memory->write_byte(m_registers.hl, m_registers.l); execute_cycle(); break;
	case 0x76: m_halt = true; break;
	case 0x77: m_memory->write_byte(m_registers.hl, m_registers.a); execute_cycle(); break;
	case 0x78: load_8bit(m_registers.a, m_registers.b); break;
	case 0x79: load_8bit(m_registers.a, m_registers.c); break;
	case 0x7A: load_8bit(m_registers.a, m_registers.d); break;
	case 0x7B: load_8bit(m_registers.a, m_registers.e); break;
	case 0x7C: load_8bit(m_registers.a, m_registers.h); break;
	case 0x7D: load_8bit(m_registers.a, m_registers.l); break;
	case 0x7E: load_8bit_mem(m_registers.a, m_registers.hl); break;
	case 0x7F: load_8bit(m_registers.a, m_registers.a); break;
	case 0x80: add_8bit(m_registers.a, m_registers.b, false, false); break;
	case 0x81: add_8bit(m_registers.a, m_registers.c, false, false); break;
	case 0x82: add_8bit(m_registers.a, m_registers.d, false, false); break;
	case 0x83: add_8bit(m_registers.a, m_registers.e, false, false); break;
	case 0x84: add_8bit(m_registers.a, m_registers.h, false, false); break;
	case 0x85: add_8bit(m_registers.a, m_registers.l, false, false); break;
	case 0x86: add_8bit_mem(m_registers.a, m_registers.hl, false); break;
	case 0x87: add_8bit(m_registers.a, m_registers.a, false, false); break;
	case 0x88: add_8bit(m_registers.a, m_registers.b, false, true); break;
	case 0x89: add_8bit(m_registers.a, m_registers.c, false, true); break;
	case 0x8A: add_8bit(m_registers.a, m_registers.d, false, true); break;
	case 0x8B: add_8bit(m_registers.a, m_registers.e, false, true); break;
	case 0x8C: add_8bit(m_registers.a, m_registers.h, false, true); break;
	case 0x8D: add_8bit(m_registers.a, m_registers.l, false, true); break;
	case 0x8E: add_8bit_mem(m_registers.a, m_registers.hl, true); break;
	case 0x8F: add_8bit(m_registers.a, m_registers.a, false, true); break;
	case 0x90: sub_8bit(m_registers.a, m_registers.b, false, false); break;
	case 0x91: sub_8bit(m_registers.a, m_registers.c, false, false); break;
	case 0x92: sub_8bit(m_registers.a, m_registers.d, false, false); break;
	case 0x93: sub_8bit(m_registers.a, m_registers.e, false, false); break;
	case 0x94: sub_8bit(m_registers.a, m_registers.h, false, false); break;
	case 0x95: sub_8bit(m_registers.a, m_registers.l, false, false); break;
	case 0x96: sub_8bit_mem(m_registers.a, m_registers.hl, false); break;
	case 0x97: sub_8bit(m_registers.a, m_registers.a, false, false); break;
	case 0x98: sub_8bit(m_registers.a, m_registers.b, false, true); break;
	case 0x99: sub_8bit(m_registers.a, m_registers.c, false, true); break;
	case 0x9A: sub_8bit(m_registers.a, m_registers.d, false, true); break;
	case 0x9B: sub_8bit(m_registers.a, m_registers.e, false, true); break;
	case 0x9C: sub_8bit(m_registers.a, m_registers.h, false, true); break;
	case 0x9D: sub_8bit(m_registers.a, m_registers.l, false, true); break;
	case 0x9E: sub_8bit_mem(m_registers.a, m_registers.hl, true); break;
	case 0x9F: sub_8bit(m_registers.a, m_registers.a, false, true); break;
	case 0xA0: and_8bit(m_registers.a, m_registers.b, false); break;
	case 0xA1: and_8bit(m_registers.a, m_registers.c, false); break;
	case 0xA2: and_8bit(m_registers.a, m_registers.d, false); break;
	case 0xA3: and_8bit(m_registers.a, m_registers.e, false); break;
	case 0xA4: and_8bit(m_registers.a, m_registers.h, false); break;
	case 0xA5: and_8bit(m_registers.a, m_registers.l, false); break;
	case 0xA6: and_8bit_mem(m_registers.a, m_registers.hl); break;
	case 0xA7: and_8bit(m_registers.a, m_registers.a, false); break;
	case 0xA8: xor_8bit(m_registers.a, m_registers.b, false); break;
	case 0xA9: xor_8bit(m_registers.a, m_registers.c, false); break;
	case 0xAA: xor_8bit(m_registers.a, m_registers.d, false); break;
	case 0xAB: xor_8bit(m_registers.a, m_registers.e, false); break;
	case 0xAC: xor_8bit(m_registers.a, m_registers.h, false); break;
	case 0xAD: xor_8bit(m_registers.a, m_registers.l, false); break;
	case 0xAE: xor_8bit_mem(m_registers.a, m_registers.hl); break;
	case 0xAF: xor_8bit(m_registers.a, m_registers.a, false); break;
	case 0xB0: or_8bit(m_registers.a, m_registers.b, false); break;
	case 0xB1: or_8bit(m_registers.a, m_registers.c, false); break;
	case 0xB2: or_8bit(m_registers.a, m_registers.d, false); break;
	case 0xB3: or_8bit(m_registers.a, m_registers.e, false); break;
	case 0xB4: or_8bit(m_registers.a, m_registers.h, false); break;
	case 0xB5: or_8bit(m_registers.a, m_registers.l, false); break;
	case 0xB6: or_8bit_mem(m_registers.a, m_registers.hl); break;
	case 0xB7: or_8bit(m_registers.a, m_registers.a, false); break;
	case 0xB8: cp_8bit(m_registers.a, m_registers.b, false); break;
	case 0xB9: cp_8bit(m_registers.a, m_registers.c, false); break;
	case 0xBA: cp_8bit(m_registers.a, m_registers.d, false); break;
	case 0xBB: cp_8bit(m_registers.a, m_registers.e, false); break;
	case 0xBC: cp_8bit(m_registers.a, m_registers.h, false); break;
	case 0xBD: cp_8bit(m_registers.a, m_registers.l, false); break;
	case 0xBE: cp_8bit_mem(m_registers.a, m_registers.hl); break;
	case 0xBF: cp_8bit(m_registers.a, m_registers.a, false); break;
	case 0xC0: ret_flag(0x80, false); break;
	case 0xC1: pop_stack(m_registers.bc); break;
	case 0xC2: jump_16bit_flag(0x80, false); break;
	case 0xC3: jump_16bit(0, true); break;
	case 0xC4: call_flag(0x80, false); break;
	case 0xC5: push_stack(m_registers.bc); execute_cycle(); break;
	case 0xC6: add_8bit(m_registers.a, 0, true, false); break;
	case 0xC7: rst(0x00); break;
	case 0xC8: ret_flag(0x80, true); break;
	case 0xC9: ret(); break;
	case 0xCA: jump_16bit_flag(0x80, true); break;
	case 0xCB: retval = execute_extension_opcode(); break;
	case 0xCC: call_flag(0x80, true); break;
	case 0xCD: call_16bit(); break;
	case 0xCE: add_8bit(m_registers.a, 0, true, true); break;
	case 0xCF: rst(0x08); break;
	case 0xD0: ret_flag(0x10, false); break;
	case 0xD1: pop_stack(m_registers.de); break;
	case 0xD2: jump_16bit_flag(0x10, false); break;
		// 0xD3: No instruction
	case 0xD4: call_flag(0x10, false); break;
	case 0xD5: push_stack(m_registers.de); execute_cycle(); break;
	case 0xD6: sub_8bit(m_registers.a, 0, true, false); break;
	case 0xD7: rst(0x10); break;
	case 0xD8: ret_flag(0x10, true); break;
	case 0xD9: reti(); break;
	case 0xDA: jump_16bit_flag(0x10, true); break;
		// 0xDB: No instruction
	case 0xDC: call_flag(0x10, true); break;
		// 0xDD: No instruction
	case 0xDE: sub_8bit(m_registers.a, 0, true, true); break;
	case 0xDF: rst(0x18); break;
	case 0xE0:
	{
		unsigned char n = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		m_memory->write_byte(0xFF00 | n, m_registers.a);
		execute_cycle();
	}break;
	case 0xE1: pop_stack(m_registers.hl); break;
	case 0xE2:
	{
		m_memory->write_byte(0xFF00 | m_registers.c, m_registers.a);
		execute_cycle();
	}break;
	// 0xE3: No instruction
	// 0xE4: No instruction
	case 0xE5: push_stack(m_registers.hl); execute_cycle(); break;
	case 0xE6: and_8bit(m_registers.a, 0, true); break;
	case 0xE7: rst(0x20); break;
	case 0xE8:
	{
		char n = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		unsigned short old = m_registers.sp;
		m_registers.sp += n;
		execute_cycle();
		m_registers.z_f = 0;
		m_registers.n_f = 0;
		m_registers.h_f = ((old & 0xF) + ((unsigned char)n & 0xF) > 0xF) ? 1 : 0;
		m_registers.c_f = ((old & 0xFF) + (unsigned char)n > 0xFF) ? 1 : 0;
		execute_cycle();
	}break;
	case 0xE9: m_registers.pc = m_registers.hl; break;
	case 0xEA:
	{
		unsigned char n1 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		unsigned char n2 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		m_memory->write_byte((n2 << 8) | n1, m_registers.a);
		execute_cycle();
	}break;
	// 0xEB: No instruction
	// 0xEC: No instruction
	// 0xED: No instruction
	case 0xEE: xor_8bit(m_registers.a, 0, true); break;
	case 0xEF: rst(0x28); break;
	case 0xF0:
	{
		unsigned char n = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		n = m_memory->read_byte(0xFF00 | n);
		execute_cycle();
		m_registers.a = n;
	}break;
	case 0xF1: pop_stack(m_registers.af); break;
	case 0xF2: m_registers.a = m_memory->read_byte(0xFF00 | m_registers.c); execute_cycle(); break;
	case 0xF3: di(); break;
		// 0xF4: No instruction
	case 0xF5: push_stack(m_registers.af); execute_cycle(); break;
	case 0xF6: or_8bit(m_registers.a, 0, true); break;
	case 0xF7: rst(0x30); break;
	case 0xF8:
	{
		char n = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		load_16bit(m_registers.hl, m_registers.sp + n);
		m_registers.z_f = 0;
		m_registers.n_f = 0;
		m_registers.h_f = ((m_registers.sp & 0xF) + ((unsigned char)n & 0xF) > 0xF) ? 1 : 0;
		m_registers.c_f = ((m_registers.sp & 0xFF) + (unsigned char)n > 0xFF) ? 1 : 0;
	}break;
	case 0xF9: load_16bit(m_registers.sp, m_registers.hl); break;
	case 0xFA:
	{
		unsigned char n1 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		unsigned char n2 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		load_8bit_mem(m_registers.a, (n2 << 8) | n1);
	}break;
	case 0xFB: ei(); break;
		// 0xFC: No instruction
		// 0xFD: No instruction
	case 0xFE: cp_8bit(m_registers.a, 0, true); break;
	case 0xFF: rst(0x38); break;
	default:
	{
		std::cout << "Unimplemented opcode 0x" << std::hex << (unsigned short)opcode << " ";

		char buffer[50];
		if (instr.length == 1)
		{
			std::cout << instr.instruction << (unsigned short)m_memory->read_byte(m_registers.pc);
		}
		else if (instr.length == 2)
		{
			std::cout << instr.instruction << m_memory->read_short(m_registers.pc);
		}
		else
		{
			std::cout << instr.instruction;
		}

		std::cout << buffer << std::endl;

		s_debugRegisters = true;
		retval = false;
	}break;
	}

	// force f to always be F0 like
	m_registers.f &= 0xF0;

	return retval;
}

void processor::log_registers(cpu_registers& regs)
{
	std::cout << std::hex << "		af=" << regs.af << " bc=" << regs.bc << " de=" << regs.de << std::endl;
	std::cout << std::hex << "		hl=" << regs.hl << " sp=" << regs.sp << " pc=" << regs.pc << std::endl;
}

void processor::update_timers(unsigned int clocks)
{
	uint8_t tima = m_memory->read_byte_from_raw_memory(0xff05);
	uint8_t tma = m_memory->read_byte_from_raw_memory(0xff06);
	uint8_t tac = m_memory->read_byte_from_raw_memory(0xff07);
	uint8_t intr = m_memory->read_byte_from_raw_memory(0xff0f);

	for (int i = 0; i < clocks; ++i)
	{
		m_clocks_timer++;
		m_memory->write_byte_to_raw_memory(0xff04, (m_clocks_timer & 0xff00) >> 8);
		m_timer++;
		unsigned short cycles = 0;

		if (tac & 0x4)
		{
			switch (tac & 0x3)
			{
			case 0: cycles = 1024; break;
			case 1: cycles = 16; break;
			case 2: cycles = 64; break;
			case 3: cycles = 256; break;
			};

			if (m_timer >= cycles)
			{
				m_timer -= cycles;
				if (tima == 0xFF)
				{
					tima = tma;
					m_memory->request_interrupt(mask_int_timer);
				}
				else
				{
					tima++;
				}
			}
		}
	}

	m_memory->write_byte_to_raw_memory(0xff05, tima);
}

void processor::check_interrupts()
{
	const unsigned char int_fired = m_memory->read_byte_from_raw_memory(io_int_enable) & m_memory->read_byte_from_raw_memory(io_int_flag);

	// if there is an interrupt to fire then we need to stop halting
	if (int_fired && m_halt)
	{
		m_halt = false;
	}

	if (m_ime)
	{
		if (int_fired & mask_int_vblank)
		{
			m_ime = false;
			m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) & ~mask_int_vblank);
			rst(int_addr_vblank);
		}
		else if (int_fired & mask_int_lcd)
		{
			m_ime = false;
			m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) & ~mask_int_lcd);
			rst(int_addr_lcd);
		}
		else if (int_fired & mask_int_timer)
		{
			m_ime = false;
			m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) & ~mask_int_timer);
			rst(int_addr_timer);
		}
		else if (int_fired & mask_int_serial)
		{
			m_ime = false;
			m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) & ~mask_int_serial);
			rst(int_addr_serial);
		}
		else if (int_fired & mask_int_joypad)
		{
			m_ime = false;
			m_memory->write_byte_to_raw_memory(io_int_flag, m_memory->read_byte_from_raw_memory(io_int_flag) & ~mask_int_joypad);
			rst(int_addr_joypad);
		}
	}
}

void processor::load_8bit(unsigned char& reg)
{
	load_8bit_mem(reg, m_registers.pc++);
}

void processor::load_8bit(unsigned char& reg, unsigned char val)
{
	reg = val;
}

void processor::load_8bit_mem(unsigned char& reg, unsigned short mem)
{
	reg = m_memory->read_byte(mem);
	execute_cycle();
}

void processor::load_16bit(unsigned short& reg)
{
	reg = (reg & 0xFF00) | m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	reg = (reg & 0x00FF) | (m_memory->read_byte(m_registers.pc++) << 8);
	execute_cycle();
}

void processor::load_16bit(unsigned short& reg, unsigned short val)
{
	reg = val;
	execute_cycle();
}

void processor::dec_16bit(unsigned short& val)
{
	val--;
	execute_cycle();
}

void processor::inc_16bit(unsigned short& val)
{
	val++;
	execute_cycle();
}

void processor::add_8bit(unsigned char& reg, unsigned char val, bool immediate, bool carry)
{
	unsigned char old = reg;
	unsigned char c = m_registers.c_f;
	if (immediate)
	{
		val = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
	}
	reg += val;
	if (carry) reg += c;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	if (carry)
	{
		m_registers.h_f = ((old & 0xF) + (val & 0xF) + c) > 0xF ? 1 : 0;
		m_registers.c_f = (old + val + c > 0xFF) ? 1 : 0;
	}
	else
	{
		m_registers.h_f = ((old & 0xF) + (val & 0xF)) > 0xF ? 1 : 0;
		m_registers.c_f = (old + val > 0xFF) ? 1 : 0;
	}
}

void processor::add_8bit_mem(unsigned char& reg, unsigned short mem, bool carry)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	add_8bit(reg, n, false, carry);
}

void processor::sub_8bit(unsigned char& reg, unsigned char val, bool immediate, bool carry)
{
	unsigned char old = reg;
	unsigned char c = m_registers.c_f;
	if (immediate)
	{
		val = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
	}
	reg -= val;
	if (carry) reg -= c;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 1;
	if (carry)
	{
		m_registers.h_f = ((old & 0xF) - (val & 0xF) - c) < 0 ? 1 : 0;
		m_registers.c_f = (old < (val + c)) ? 1 : 0;
	}
	else
	{
		m_registers.h_f = ((old & 0xF) - (val & 0xF)) < 0 ? 1 : 0;
		m_registers.c_f = (old < val) ? 1 : 0;
	}
}

void processor::sub_8bit_mem(unsigned char& reg, unsigned short mem, bool carry)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	sub_8bit(reg, n, false, carry);
}

void processor::and_8bit(unsigned char& reg, unsigned char val, bool immediate)
{
	unsigned char old = reg;
	if (immediate)
	{
		val = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
	}
	reg &= val;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 1;
	m_registers.c_f = 0;
}

void processor::and_8bit_mem(unsigned char& reg, unsigned short mem)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	and_8bit(reg, n, false);
}

void processor::or_8bit(unsigned char& reg, unsigned char val, bool immediate)
{
	unsigned char old = reg;
	if (immediate)
	{
		val = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
	}
	reg |= val;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
	m_registers.c_f = 0;
}

void processor::or_8bit_mem(unsigned char& reg, unsigned short mem)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	or_8bit(reg, n, false);
}

void processor::xor_8bit(unsigned char& reg, unsigned char val, bool immediate)
{
	unsigned char old = reg;
	if (immediate)
	{
		val = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
	}
	reg ^= val;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
	m_registers.c_f = 0;
}

void processor::xor_8bit_mem(unsigned char& reg, unsigned short mem)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	xor_8bit(reg, n, false);
}

void processor::cp_8bit(unsigned char& reg, unsigned char val, bool immediate)
{
	if (immediate)
	{
		val = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
	}
	m_registers.z_f = (reg == val) ? 1 : 0;
	m_registers.n_f = 1;
	m_registers.c_f = (reg < val) ? 1 : 0;
	m_registers.h_f = ((reg & 0xF) - (val & 0xF)) < 0 ? 1 : 0;
}

void processor::cp_8bit_mem(unsigned char& reg, unsigned short mem)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	cp_8bit(reg, n, false);
}

void processor::inc_8bit(unsigned char& reg)
{
	unsigned char old = reg++;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = ((old & 0x0F) == 0x0F) ? 1 : 0;
}

void processor::inc_8bit_mem(unsigned short& mem)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	inc_8bit(n);
	m_memory->write_byte(mem, n);
	execute_cycle();
}

void processor::dec_8bit(unsigned char& reg)
{
	unsigned char old = reg--;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 1;
	m_registers.h_f = ((old & 0xF) == 0) ? 1 : 0;
}

void processor::dec_8bit_mem(unsigned short& mem)
{
	unsigned char n = m_memory->read_byte(mem);
	execute_cycle();
	dec_8bit(n);
	m_memory->write_byte(mem, n);
	execute_cycle();
}

void processor::add_16bit(unsigned short& reg, unsigned short val)
{
	unsigned short old = reg;
	reg += val;
	m_registers.n_f = 0;
	m_registers.h_f = ((old & 0xFFF) + (val & 0xFFF)) > 0xFFF ? 1 : 0;// old & 0xFFF, val & 0xFFF > 0x1000
																	  //m_registers.h_f = (((old & 0xFF00) & 0xF) + ((val >> 8) & 0xF)) ? 1 : 0;
	m_registers.c_f = (old + val > 0xFFFF) ? 1 : 0;
	execute_cycle();
}

void processor::jump_8bit_flag(int flag, bool on)
{
	char n = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	bool jump = ((m_registers.f & flag) == flag) == on;
	m_registers.pc = jump ? m_registers.pc + n : m_registers.pc;
	if (jump) execute_cycle();
	if (!jump && s_breakOnJR) s_debugRegisters = true;
}

void processor::jump_8bit()
{
	char n = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	m_registers.pc += n;
	execute_cycle();
}

void processor::jump_16bit_flag(int flag, bool on)
{
	unsigned char n1 = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	unsigned char n2 = m_memory->read_byte(m_registers.pc++);
	execute_cycle();

	bool jump = ((m_registers.f & flag) == flag) == on;

	if (jump)
	{
		m_registers.pc = (n2 << 8) | n1;
		execute_cycle();
	}
	else if (s_breakOnJR)
	{
		s_debugRegisters = true;
	}
}

void processor::jump_16bit(unsigned short addr, bool immediate)
{
	if (immediate)
	{
		unsigned char n1 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		unsigned char n2 = m_memory->read_byte(m_registers.pc++);
		execute_cycle();
		addr = (n2 << 8) | n1;
	}
	m_registers.pc = addr;
	execute_cycle();
}

void processor::call_16bit()
{
	if (s_breakOnCALL)
	{
		s_debugRegisters = true;
	}
	unsigned char n1 = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	unsigned char n2 = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	push_stack(m_registers.pc);
	m_registers.pc = (n2 << 8) | n1;
	execute_cycle();
}

void processor::call_flag(int flag, bool on)
{
	if (s_breakOnCALL)
	{
		s_debugRegisters = true;
	}
	unsigned char n1 = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	unsigned char n2 = m_memory->read_byte(m_registers.pc++);
	execute_cycle();
	if (((m_registers.f & flag) == flag) == on)
	{
		push_stack(m_registers.pc);
		m_registers.pc = (n2 << 8) | n1;
		execute_cycle();
	}
}

void processor::ret()
{
	if (s_breakOnRET)
	{
		s_debugRegisters = true;
	}
	unsigned short nn;
	pop_stack(nn);
	m_registers.pc = nn;
	execute_cycle();
}

void processor::reti()
{
	m_ime = true;
	m_halt = false;
	ret();
}

void processor::ret_flag(int flag, bool on)
{
	if (((m_registers.f & flag) == flag) == on)
	{
		ret();
	}
	execute_cycle();
}

void processor::push_stack(unsigned short val)
{
	m_memory->write_byte(--m_registers.sp, (val & 0xFF00) >> 8);
	execute_cycle();
	m_memory->write_byte(--m_registers.sp, val & 0x00FF);
	execute_cycle();
}

void processor::pop_stack(unsigned short& reg)
{
	reg = (reg & 0xFF00) | m_memory->read_byte(m_registers.sp++);
	execute_cycle();
	reg = (reg & 0x00FF) | (m_memory->read_byte(m_registers.sp++) << 8);
	execute_cycle();
}

void processor::rst(unsigned char val)
{
	push_stack(m_registers.pc);
	m_registers.pc = val;
	execute_cycle();
}

void processor::di()
{
	m_ime = false;
}

void processor::ei()
{
	m_ime = true;
}

void processor::reset_bit(unsigned char& reg, int bit)
{
	reg &= ~(1 << bit);
}

void processor::reset_bit_mem(unsigned short addr, int bit)
{
	unsigned char n = m_memory->read_byte(addr);
	execute_cycle();
	reset_bit(n, bit);
	m_memory->write_byte(addr, n);
	execute_cycle();
}

void processor::set_bit(unsigned char& reg, int bit)
{
	unsigned char val = (1 << bit);
	reg |= (1 << bit);
}

void processor::set_bit_mem(unsigned short addr, int bit)
{
	unsigned char n = m_memory->read_byte(addr);
	execute_cycle();
	set_bit(n, bit);
	m_memory->write_byte(addr, n);
	execute_cycle();
}

void processor::test_bit(unsigned char& reg, int bit)
{
	m_registers.z_f = (reg & (1 << bit)) == 0 ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 1;
}

void processor::test_bit_mem(unsigned short addr, int bit)
{
	unsigned char n = m_memory->read_byte(addr);
	execute_cycle();
	test_bit(n, bit);
}

void processor::daa()
{
	bool carry = false;
	if (m_registers.n_f == 0)
	{
		if (m_registers.c_f == 1 || m_registers.a > 0x99)
		{
			m_registers.a += 0x60;
			carry = true;
		}
		if (m_registers.h_f == 1 || (m_registers.a & 0xF) > 0x9)
		{
			m_registers.a += 0x06;
		}
	}
	else if (m_registers.c_f == 1)
	{
		carry = true;
		if (m_registers.h_f == 1)
		{
			m_registers.a += 0x9A;
		}
		else
		{
			m_registers.a += 0xA0;
		}
	}
	else if (m_registers.h_f == 1)
	{
		m_registers.a += 0xFA;
	}

	m_registers.z_f = (m_registers.a == 0) ? 1 : 0;
	m_registers.c_f = (carry) ? 1 : 0;
	m_registers.h_f = 0;
}

bool processor::execute_extension_opcode()
{
	unsigned char n = m_memory->read_byte(m_registers.pc++);
	execute_cycle();

	const cpu_instruction& instr = g_gbExtInstructions[n];
	bool ret = true;

	switch (n)
	{
	case 0x00: rotate_left(m_registers.b, true); break;
	case 0x01: rotate_left(m_registers.c, true); break;
	case 0x02: rotate_left(m_registers.d, true); break;
	case 0x03: rotate_left(m_registers.e, true); break;
	case 0x04: rotate_left(m_registers.h, true); break;
	case 0x05: rotate_left(m_registers.l, true); break;
	case 0x06: rotate_left_mem(m_registers.hl, true); break;
	case 0x07: rotate_left(m_registers.a, true); break;
	case 0x08: rotate_right(m_registers.b, true); break;
	case 0x09: rotate_right(m_registers.c, true); break;
	case 0x0A: rotate_right(m_registers.d, true); break;
	case 0x0B: rotate_right(m_registers.e, true); break;
	case 0x0C: rotate_right(m_registers.h, true); break;
	case 0x0D: rotate_right(m_registers.l, true); break;
	case 0x0E: rotate_right_mem(m_registers.hl, true); break;
	case 0x0F: rotate_right(m_registers.a, true); break;
	case 0x10: rotate_left(m_registers.b, false); break;
	case 0x11: rotate_left(m_registers.c, false); break;
	case 0x12: rotate_left(m_registers.d, false); break;
	case 0x13: rotate_left(m_registers.e, false); break;
	case 0x14: rotate_left(m_registers.h, false); break;
	case 0x15: rotate_left(m_registers.l, false); break;
	case 0x16: rotate_left_mem(m_registers.hl, false); break;
	case 0x17: rotate_left(m_registers.a, false); break;
	case 0x18: rotate_right(m_registers.b, false); break;
	case 0x19: rotate_right(m_registers.c, false); break;
	case 0x1A: rotate_right(m_registers.d, false); break;
	case 0x1B: rotate_right(m_registers.e, false); break;
	case 0x1C: rotate_right(m_registers.h, false); break;
	case 0x1D: rotate_right(m_registers.l, false); break;
	case 0x1E: rotate_right_mem(m_registers.hl, false); break;
	case 0x1F: rotate_right(m_registers.a, false); break;
	case 0x20: shift_left(m_registers.b); break;
	case 0x21: shift_left(m_registers.c); break;
	case 0x22: shift_left(m_registers.d); break;
	case 0x23: shift_left(m_registers.e); break;
	case 0x24: shift_left(m_registers.h); break;
	case 0x25: shift_left(m_registers.l); break;
	case 0x26: shift_left_mem(m_registers.hl); break;
	case 0x27: shift_left(m_registers.a); break;
	case 0x28: shift_right(m_registers.b, true); break;
	case 0x29: shift_right(m_registers.c, true); break;
	case 0x2A: shift_right(m_registers.d, true); break;
	case 0x2B: shift_right(m_registers.e, true); break;
	case 0x2C: shift_right(m_registers.h, true); break;
	case 0x2D: shift_right(m_registers.l, true); break;
	case 0x2E: shift_right_mem(m_registers.hl, true); break;
	case 0x2F: shift_right(m_registers.a, true); break;
	case 0x30: swap(m_registers.b); break;
	case 0x31: swap(m_registers.c); break;
	case 0x32: swap(m_registers.d); break;
	case 0x33: swap(m_registers.e); break;
	case 0x34: swap(m_registers.h); break;
	case 0x35: swap(m_registers.l); break;
	case 0x36: swap_mem(m_registers.hl); break;
	case 0x37: swap(m_registers.a); break;
	case 0x38: shift_right(m_registers.b, false); break;
	case 0x39: shift_right(m_registers.c, false); break;
	case 0x3A: shift_right(m_registers.d, false); break;
	case 0x3B: shift_right(m_registers.e, false); break;
	case 0x3C: shift_right(m_registers.h, false); break;
	case 0x3D: shift_right(m_registers.l, false); break;
	case 0x3E: shift_right_mem(m_registers.hl, false); break;
	case 0x3F: shift_right(m_registers.a, false); break;
	case 0x40: test_bit(m_registers.b, 0); break;
	case 0x41: test_bit(m_registers.c, 0); break;
	case 0x42: test_bit(m_registers.d, 0); break;
	case 0x43: test_bit(m_registers.e, 0); break;
	case 0x44: test_bit(m_registers.h, 0); break;
	case 0x45: test_bit(m_registers.l, 0); break;
	case 0x46: test_bit_mem(m_registers.hl, 0); break;
	case 0x47: test_bit(m_registers.a, 0); break;
	case 0x48: test_bit(m_registers.b, 1); break;
	case 0x49: test_bit(m_registers.c, 1); break;
	case 0x4A: test_bit(m_registers.d, 1); break;
	case 0x4B: test_bit(m_registers.e, 1); break;
	case 0x4C: test_bit(m_registers.h, 1); break;
	case 0x4D: test_bit(m_registers.l, 1); break;
	case 0x4E: test_bit_mem(m_registers.hl, 1); break;
	case 0x4F: test_bit(m_registers.a, 1); break;
	case 0x50: test_bit(m_registers.b, 2); break;
	case 0x51: test_bit(m_registers.c, 2); break;
	case 0x52: test_bit(m_registers.d, 2); break;
	case 0x53: test_bit(m_registers.e, 2); break;
	case 0x54: test_bit(m_registers.h, 2); break;
	case 0x55: test_bit(m_registers.l, 2); break;
	case 0x56: test_bit_mem(m_registers.hl, 2); break;
	case 0x57: test_bit(m_registers.a, 2); break;
	case 0x58: test_bit(m_registers.b, 3); break;
	case 0x59: test_bit(m_registers.c, 3); break;
	case 0x5A: test_bit(m_registers.d, 3); break;
	case 0x5B: test_bit(m_registers.e, 3); break;
	case 0x5C: test_bit(m_registers.h, 3); break;
	case 0x5D: test_bit(m_registers.l, 3); break;
	case 0x5E: test_bit_mem(m_registers.hl, 3); break;
	case 0x5F: test_bit(m_registers.a, 3); break;
	case 0x60: test_bit(m_registers.b, 4); break;
	case 0x61: test_bit(m_registers.c, 4); break;
	case 0x62: test_bit(m_registers.d, 4); break;
	case 0x63: test_bit(m_registers.e, 4); break;
	case 0x64: test_bit(m_registers.h, 4); break;
	case 0x65: test_bit(m_registers.l, 4); break;
	case 0x66: test_bit_mem(m_registers.hl, 4); break;
	case 0x67: test_bit(m_registers.a, 4); break;
	case 0x68: test_bit(m_registers.b, 5); break;
	case 0x69: test_bit(m_registers.c, 5); break;
	case 0x6A: test_bit(m_registers.d, 5); break;
	case 0x6B: test_bit(m_registers.e, 5); break;
	case 0x6C: test_bit(m_registers.h, 5); break;
	case 0x6D: test_bit(m_registers.l, 5); break;
	case 0x6E: test_bit_mem(m_registers.hl, 5); break;
	case 0x6F: test_bit(m_registers.a, 5); break;
	case 0x70: test_bit(m_registers.b, 6); break;
	case 0x71: test_bit(m_registers.c, 6); break;
	case 0x72: test_bit(m_registers.d, 6); break;
	case 0x73: test_bit(m_registers.e, 6); break;
	case 0x74: test_bit(m_registers.h, 6); break;
	case 0x75: test_bit(m_registers.l, 6); break;
	case 0x76: test_bit_mem(m_registers.hl, 6); break;
	case 0x77: test_bit(m_registers.a, 6); break;
	case 0x78: test_bit(m_registers.b, 7); break;
	case 0x79: test_bit(m_registers.c, 7); break;
	case 0x7A: test_bit(m_registers.d, 7); break;
	case 0x7B: test_bit(m_registers.e, 7); break;
	case 0x7C: test_bit(m_registers.h, 7); break;
	case 0x7D: test_bit(m_registers.l, 7); break;
	case 0x7E: test_bit_mem(m_registers.hl, 7); break;
	case 0x7F: test_bit(m_registers.a, 7); break;
	case 0x80: reset_bit(m_registers.b, 0); break;
	case 0x81: reset_bit(m_registers.c, 0); break;
	case 0x82: reset_bit(m_registers.d, 0); break;
	case 0x83: reset_bit(m_registers.e, 0); break;
	case 0x84: reset_bit(m_registers.h, 0); break;
	case 0x85: reset_bit(m_registers.l, 0); break;
	case 0x86: reset_bit_mem(m_registers.hl, 0); break;
	case 0x87: reset_bit(m_registers.a, 0); break;
	case 0x88: reset_bit(m_registers.b, 1); break;
	case 0x89: reset_bit(m_registers.c, 1); break;
	case 0x8A: reset_bit(m_registers.d, 1); break;
	case 0x8B: reset_bit(m_registers.e, 1); break;
	case 0x8C: reset_bit(m_registers.h, 1); break;
	case 0x8D: reset_bit(m_registers.l, 1); break;
	case 0x8E: reset_bit_mem(m_registers.hl, 1); break;
	case 0x8F: reset_bit(m_registers.a, 1); break;
	case 0x90: reset_bit(m_registers.b, 2); break;
	case 0x91: reset_bit(m_registers.c, 2); break;
	case 0x92: reset_bit(m_registers.d, 2); break;
	case 0x93: reset_bit(m_registers.e, 2); break;
	case 0x94: reset_bit(m_registers.h, 2); break;
	case 0x95: reset_bit(m_registers.l, 2); break;
	case 0x96: reset_bit_mem(m_registers.hl, 2); break;
	case 0x97: reset_bit(m_registers.a, 2); break;
	case 0x98: reset_bit(m_registers.b, 3); break;
	case 0x99: reset_bit(m_registers.c, 3); break;
	case 0x9A: reset_bit(m_registers.d, 3); break;
	case 0x9B: reset_bit(m_registers.e, 3); break;
	case 0x9C: reset_bit(m_registers.h, 3); break;
	case 0x9D: reset_bit(m_registers.l, 3); break;
	case 0x9E: reset_bit_mem(m_registers.hl, 3); break;
	case 0x9F: reset_bit(m_registers.a, 3); break;
	case 0xA0: reset_bit(m_registers.b, 4); break;
	case 0xA1: reset_bit(m_registers.c, 4); break;
	case 0xA2: reset_bit(m_registers.d, 4); break;
	case 0xA3: reset_bit(m_registers.e, 4); break;
	case 0xA4: reset_bit(m_registers.h, 4); break;
	case 0xA5: reset_bit(m_registers.l, 4); break;
	case 0xA6: reset_bit_mem(m_registers.hl, 4); break;
	case 0xA7: reset_bit(m_registers.a, 4); break;
	case 0xA8: reset_bit(m_registers.b, 5); break;
	case 0xA9: reset_bit(m_registers.c, 5); break;
	case 0xAA: reset_bit(m_registers.d, 5); break;
	case 0xAB: reset_bit(m_registers.e, 5); break;
	case 0xAC: reset_bit(m_registers.h, 5); break;
	case 0xAD: reset_bit(m_registers.l, 5); break;
	case 0xAE: reset_bit_mem(m_registers.hl, 5); break;
	case 0xAF: reset_bit(m_registers.a, 5); break;
	case 0xB0: reset_bit(m_registers.b, 6); break;
	case 0xB1: reset_bit(m_registers.c, 6); break;
	case 0xB2: reset_bit(m_registers.d, 6); break;
	case 0xB3: reset_bit(m_registers.e, 6); break;
	case 0xB4: reset_bit(m_registers.h, 6); break;
	case 0xB5: reset_bit(m_registers.l, 6); break;
	case 0xB6: reset_bit_mem(m_registers.hl, 6); break;
	case 0xB7: reset_bit(m_registers.a, 6); break;
	case 0xB8: reset_bit(m_registers.b, 7); break;
	case 0xB9: reset_bit(m_registers.c, 7); break;
	case 0xBA: reset_bit(m_registers.d, 7); break;
	case 0xBB: reset_bit(m_registers.e, 7); break;
	case 0xBC: reset_bit(m_registers.h, 7); break;
	case 0xBD: reset_bit(m_registers.l, 7); break;
	case 0xBE: reset_bit_mem(m_registers.hl, 7); break;
	case 0xBF: reset_bit(m_registers.a, 7); break;
	case 0xC0: set_bit(m_registers.b, 0); break;
	case 0xC1: set_bit(m_registers.c, 0); break;
	case 0xC2: set_bit(m_registers.d, 0); break;
	case 0xC3: set_bit(m_registers.e, 0); break;
	case 0xC4: set_bit(m_registers.h, 0); break;
	case 0xC5: set_bit(m_registers.l, 0); break;
	case 0xC6: set_bit_mem(m_registers.hl, 0); break;
	case 0xC7: set_bit(m_registers.a, 0); break;
	case 0xC8: set_bit(m_registers.b, 1); break;
	case 0xC9: set_bit(m_registers.c, 1); break;
	case 0xCA: set_bit(m_registers.d, 1); break;
	case 0xCB: set_bit(m_registers.e, 1); break;
	case 0xCC: set_bit(m_registers.h, 1); break;
	case 0xCD: set_bit(m_registers.l, 1); break;
	case 0xCE: set_bit_mem(m_registers.hl, 1); break;
	case 0xCF: set_bit(m_registers.a, 1); break;
	case 0xD0: set_bit(m_registers.b, 2); break;
	case 0xD1: set_bit(m_registers.c, 2); break;
	case 0xD2: set_bit(m_registers.d, 2); break;
	case 0xD3: set_bit(m_registers.e, 2); break;
	case 0xD4: set_bit(m_registers.h, 2); break;
	case 0xD5: set_bit(m_registers.l, 2); break;
	case 0xD6: set_bit_mem(m_registers.hl, 2); break;
	case 0xD7: set_bit(m_registers.a, 2); break;
	case 0xD8: set_bit(m_registers.b, 3); break;
	case 0xD9: set_bit(m_registers.c, 3); break;
	case 0xDA: set_bit(m_registers.d, 3); break;
	case 0xDB: set_bit(m_registers.e, 3); break;
	case 0xDC: set_bit(m_registers.h, 3); break;
	case 0xDD: set_bit(m_registers.l, 3); break;
	case 0xDE: set_bit_mem(m_registers.hl, 3); break;
	case 0xDF: set_bit(m_registers.a, 3); break;
	case 0xE0: set_bit(m_registers.b, 4); break;
	case 0xE1: set_bit(m_registers.c, 4); break;
	case 0xE2: set_bit(m_registers.d, 4); break;
	case 0xE3: set_bit(m_registers.e, 4); break;
	case 0xE4: set_bit(m_registers.h, 4); break;
	case 0xE5: set_bit(m_registers.l, 4); break;
	case 0xE6: set_bit_mem(m_registers.hl, 4); break;
	case 0xE7: set_bit(m_registers.a, 4); break;
	case 0xE8: set_bit(m_registers.b, 5); break;
	case 0xE9: set_bit(m_registers.c, 5); break;
	case 0xEA: set_bit(m_registers.d, 5); break;
	case 0xEB: set_bit(m_registers.e, 5); break;
	case 0xEC: set_bit(m_registers.h, 5); break;
	case 0xED: set_bit(m_registers.l, 5); break;
	case 0xEE: set_bit_mem(m_registers.hl, 5); break;
	case 0xEF: set_bit(m_registers.a, 5); break;
	case 0xF0: set_bit(m_registers.b, 6); break;
	case 0xF1: set_bit(m_registers.c, 6); break;
	case 0xF2: set_bit(m_registers.d, 6); break;
	case 0xF3: set_bit(m_registers.e, 6); break;
	case 0xF4: set_bit(m_registers.h, 6); break;
	case 0xF5: set_bit(m_registers.l, 6); break;
	case 0xF6: set_bit_mem(m_registers.hl, 6); break;
	case 0xF7: set_bit(m_registers.a, 6); break;
	case 0xF8: set_bit(m_registers.b, 7); break;
	case 0xF9: set_bit(m_registers.c, 7); break;
	case 0xFA: set_bit(m_registers.d, 7); break;
	case 0xFB: set_bit(m_registers.e, 7); break;
	case 0xFC: set_bit(m_registers.h, 7); break;
	case 0xFD: set_bit(m_registers.l, 7); break;
	case 0xFE: set_bit_mem(m_registers.hl, 7); break;
	case 0xFF: set_bit(m_registers.a, 7); break;
	default:
		s_debugRegisters = true;
		std::cout << "Unimplemented ext opcode 0x" << std::hex << (unsigned short)0xCB << " 0x" << (unsigned short)n << " " << instr.instruction << std::endl;
		ret = false;
		break;
	}
	return ret;
}

void processor::rotate_left(unsigned char& reg, bool carryBit7)
{
	const unsigned char c = m_registers.c_f;
	m_registers.c_f = (reg & 0x80) >> 7;
	reg <<= 1;
	reg |= (carryBit7) ? m_registers.c_f : c;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
}

void processor::rotate_left_mem(unsigned short addr, bool carryBit7)
{
	unsigned char n = m_memory->read_byte(addr);
	execute_cycle();
	rotate_left(n, carryBit7);
	m_memory->write_byte(addr, n);
	execute_cycle();
}

void processor::rotate_right(unsigned char& reg, bool carryBit7)
{
	const unsigned char c = m_registers.c_f;
	m_registers.c_f = reg & 0x01;
	reg >>= 1;
	reg |= (carryBit7) ? (m_registers.c_f << 7) : (c << 7);
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
}

void processor::rotate_right_mem(unsigned short addr, bool carryBit7)
{
	unsigned char n = m_memory->read_byte(addr);
	execute_cycle();
	rotate_right(n, carryBit7);
	m_memory->write_byte(addr, n);
	execute_cycle();
}

void processor::shift_right(unsigned char& reg, bool keepMSB)
{
	const unsigned char c = reg & 0x1;
	const unsigned char msb = reg & 0x80;
	reg >>= 1;
	if (keepMSB) reg |= msb;
	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
	m_registers.c_f = c;
}

void processor::shift_right_mem(unsigned short addr, bool keepMSB)
{
	unsigned char n = m_memory->read_byte(addr);

	execute_cycle();
	shift_right(n, keepMSB);
	m_memory->write_byte(addr, n);
	execute_cycle();
}

void processor::shift_left(unsigned char& reg)
{
	m_registers.c_f = (reg & 0x80) >> 7;
	
	reg <<= 1;

	m_registers.z_f = (reg == 0) ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
}

void processor::shift_left_mem(unsigned short addr)
{
	unsigned char n = m_memory->read_byte(addr);

	execute_cycle();

	shift_left(n);

	m_memory->write_byte(addr, n);

	execute_cycle();
}

void processor::swap(unsigned char& reg)
{
	reg = ((reg & 0x0F) << 4) | ((reg & 0xF0) >> 4);

	m_registers.z_f = reg == 0 ? 1 : 0;
	m_registers.n_f = 0;
	m_registers.h_f = 0;
	m_registers.c_f = 0;
}

void processor::swap_mem(unsigned short addr)
{
	unsigned char n = m_memory->read_byte(addr);

	execute_cycle();

	swap(n);

	m_memory->write_byte(addr, n);

	execute_cycle();
}
