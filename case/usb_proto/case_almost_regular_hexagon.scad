
include <case_pcb.scad>

grid_rows = 10;
chamfer=1;

module next()
{
	translate([40,75,0])
	children();
}

module at_extra_holes()
{
	r = sqrt( pow(pcb_center()[0], 2) + pow(pcb_center()[1], 2) );
	translate(pcb_center())
	for (y = [-r, r])
	translate([0,y,0])
	children();
}

module mockup()
{
	preview()
	on_pcb()
	pcb();
}
 
bottom()
{
	mockup();
};
 
next() flip() top()
{
	top_features();
};

