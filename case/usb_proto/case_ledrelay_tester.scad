
include <case_pcb.scad>

use <../lib/graft.scad>

tube_d=5.;
tube_width_base=1.6;
tube_width_top=.8;
tube_height=5;
total_height=18;

chamfer = 1;

module cable_tube(h, d, width_top, width_base)
{
	graft()
	translate([ 0, 20, total_height-h])
	{
		graft_add()
		cylinder(h-e, d/2+width_top, d/2+width_base);
		graft_remove()
		translate([0,0,-b])
		cylinder(8+2*b, r=d/2);
	}
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

	for (x=[15,25])
	translate([x, -10, 0])
	cable_tube(tube_height, tube_d, tube_width_top, tube_width_base);

};

