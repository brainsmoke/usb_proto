
include <case_base.scad>;

use <jst.scad>;


preview()
{
	pcb();
	on_pcb()
	{
		at_front() usb_c();
	}
}

case();


module led_connector_at(x, y)
{
z=total_height-component_z-2.54;
translate(breadboard_pos(x,y))
translate([0,0,z+2.54])
{
jst_sm_case_hole(3, .3, z, .8);
//graft_preview()translate([0,0,-z])jst_sm(3);
}
}



next()
flip()
top()
{
	at_dfu_button() case_button();
	for(x=[2:6:20])
	for(y=[4, 12])
	led_connector_at(x, y);
};
