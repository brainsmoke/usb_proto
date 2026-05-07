
use <case_tiny_keyswitches.scad>

bottom();

preview()
{
	pcb();
	on_pcb()
	{
		at_front() usb_c();
	}
}

