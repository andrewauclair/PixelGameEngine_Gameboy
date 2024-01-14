#pragma once

#include <memory>

class sound
{
private:
	unsigned char m_NR10{};
	unsigned char m_NR11{};
	unsigned char m_NR12{};
	unsigned char m_NR13{};
	unsigned char m_NR14{};
	unsigned char m_NR21{};
	unsigned char m_NR22{};
	unsigned char m_NR23{};
	unsigned char m_NR24{};
	unsigned char m_NR30{};
	unsigned char m_NR31{};
	unsigned char m_NR32{};
	unsigned char m_NR33{};
	unsigned char m_NR34{};
	unsigned char m_NR41{};
	unsigned char m_NR42{};
	unsigned char m_NR43{};
	unsigned char m_NR44{};
	unsigned char m_NR50{};
	unsigned char m_NR51{};
	unsigned char m_NR52{};
	unsigned char m_waveRAM[16]{};
	bool m_soundOn{};

public:
	uint8_t read_byte(uint16_t address) const;
	void write_byte(uint16_t addrss, uint8_t value);

private:
	void clear_registers();
};
