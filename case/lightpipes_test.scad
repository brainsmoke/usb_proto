
include <case_base.scad>
include <graft.scad>

has_leds=true;
flip()
graft()
{
	graft_base()
	translate([55,-8,total_height-1])
		cube([10,30,1]);

	light_pipes();
}

