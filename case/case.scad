
include <case_base.scad>

preview()
{
	pcb();
	on_pcb()
	{
		at_front() usb_c();
	}
}

bottom();

next() flip() top()
{
	top_features();
};
