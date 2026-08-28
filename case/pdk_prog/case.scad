
use <../lib/pinheader.scad>

include <../lib/utils.scad>
include <case_pcb.scad>

module mockup()
{
	preview()
	on_pcb()
	pcb();
}

mockup();
//next()flip()mockup();

bottom()
{
	// ...
};

next() flip() top()
{
	at_top_above_pcb()
	{
		at_dfu_button()
		case_button();
 
		for_each_led_array()
		light_pipe_hull()
		at_leds()
		light_pipe(light_pipe_depth, light_pipe_diameter, light_pipe_border, light_pipe_base_border, light_pipe_base_height);

		translate([35, 15, 0])
		{
			socketheader_case_hole(total_height-top_component_z-1, 1, 8);
			translate([-2.54*2,0,0])
			socketheader_case_hole(total_height-top_component_z-1, 1, 8);
			translate([-2.54*5,0,0])
			socketheader_case_hole(total_height-top_component_z-1, 1, 8);
		}
	}
}

