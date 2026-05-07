
use <case_tiny_keyswitches.scad>

flip() top()
{
	top_features();
	
	preview()
	{
		pcb();
		on_pcb()
		{
			at_front() usb_c();
		}
	}
}

