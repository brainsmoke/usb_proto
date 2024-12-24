
include <case_base.scad>

module at_buttons()
{   
	at_back()
		translate([-4*2.54, -pcb_radius, total_height])
			children();

	at_dfu_button() children();
}

tube_d=5.;
tube_width_base=1.6;
tube_width_top=.8;
tube_height=5;
module cable_tube()
{
	translate([0,0,e])
	cylinder(tube_height-e, tube_d/2+tube_width_base, tube_d/2+tube_width_top);
}

module cable_tube_keepout()
{
	translate([0,0,-e])
	cylinder(8+2*e, r=tube_d/2);
}


difference()
{
union()
{
for (x=[15,25])
translate([x, 10, 0])
cable_tube();
flip() top();
}
union()
{
for (x=[15,25])
translate([x, 10, 0])
cable_tube_keepout();
}
}
