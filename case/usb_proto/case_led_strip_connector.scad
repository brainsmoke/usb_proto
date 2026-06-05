include <case_pcb.scad>

use <../lib/jst.scad>
has_buttons=false;
has_dfu_button=true;
has_leds=false;

chamfer=1.2;

module led_connector_at(x, y)
{
	z=total_height-top_component_z-2.54;

	translate(breadboard_pos(x,y))
	translate([0,0,z+2.54])
	jst_sm_case_hole(3, .3, z, .8);
}

module mockup()
{
	preview()
	on_pcb()
	pcb();
}

mockup();

bottom()
{
}

next()
flip()
top()
{
	top_features();
	on_pcb()
	for(x=[2:6:20])
	for(y=[4, 12])
	led_connector_at(x, y);
};
