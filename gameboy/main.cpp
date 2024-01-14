#include "application.h"

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		return 0;
	}

	auto app{ std::make_shared<application>(std::string{ argv[1] }, std::string{ argv[2] }) };

	if (app->Construct(160, 144, 4, 4, false, true))
	{
		app->Start();
	}

	return 0;
}


/*

blarggs test progress

cpu_instr: pass
dmg_sound: --fail--
instr_timing: --fail--
interrupt_time: --fail--
mem_timing: pass
mem_timing-2: pass
oam_bug: --fail--
halt_bug: --fail--


mooneye test progress

bits/mem_oam: 
bits/reg_f:
bits/unused_hwio-GS:

gpu/hblank_ly_scx_timing-GS:
gpu/intr_1_2_timing-GS:
gpu/intr_2_0_timing:
gpu/intr_2_mode0_timing:

call_cc_timing:


failed: rapid_toggle.gb tima_reload.gb tima_write_reloading.gb tma_write_reloading.gb
*/