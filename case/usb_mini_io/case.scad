
include <case_pcb.scad>

module mockup()
{
	preview()
	on_pcb()
	pcb();
}
 
bottom();
mockup();
 
next() flip() top()
{
	top_features();
};

