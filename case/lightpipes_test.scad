
include <case_base.scad>
include <sculpt.scad>

has_leds=true;
flip()
sculpt()
{
	sculpt_base()
	translate([55,-8,total_height-1])
		cube([10,30,1]);

	light_pipes();
}

