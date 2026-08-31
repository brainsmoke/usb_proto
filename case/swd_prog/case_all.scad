
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
 
		difference()
		{
			for_each_led_array()
			light_pipe_hull()
			at_leds()
			light_pipe(light_pipe_depth, light_pipe_diameter, light_pipe_border, light_pipe_base_border, light_pipe_base_height);

			graft_add()
			{
				block([8.5,10,10], anchor=[-1,0,0]);
			}
		}
	}
	swd_header();
	jtag_header();
	serial_header();
	vusb_header();
};

