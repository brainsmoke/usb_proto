
include <case_pcb.scad>

module mockup()
{
	preview()
	on_pcb()
	pcb();
}

mockup();

bottom()
{
	// ...
};

next() flip() top()
{
	top_features();
};

