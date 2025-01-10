
include <case_base.scad>

preview()
{
	pcb();
	on_pcb()
	{
		at_front() usb_c();
	}
}

case();

next() flip() top() top_features();


