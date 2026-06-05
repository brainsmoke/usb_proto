
use <button.scad>
use <light_pipes.scad>

include <case_base.scad>

padding = 5;

top_component_z = case_split;
pcb_pos = [ 0, 0, top_component_z ];
pcb_angle = 0;

has_buttons=false;
has_dfu_button=false;
has_leds=false;

button_w=5;
button_d=5;
button_depth = total_height-top_component_z-button_height();

light_pipe_diameter = 2.2;
light_pipe_border = 1.2;
light_pipe_base_border = 3.2;
light_pipe_depth = total_height-top_component_z-led_height();
light_pipe_base_height = light_pipe_depth/3;


function _rot(x,y,a) = [ x*cos(a)-y*sin(a), x*sin(a)+y*cos(a) ];

holes_min_y = min( [for(p=holes_pos()) pcb_pos[1]+_rot(p[0],p[1], pcb_angle)[1]] );
holes_max_y = max( [for(p=holes_pos()) pcb_pos[1]+_rot(p[0],p[1], pcb_angle)[1]] );
function case_width() = holes_max_y-holes_min_y+2*outer_radius;
function origin_y_off_center() = (holes_max_y+holes_min_y)/2;

/* override with manual implementation for custom / non-convex shapes */
/* module case_shape(height, radius) { ... } */

/* override with manual implementation for custom / non-convex shapes */
/* module case_shape_chamfer(height, r1, r2) { ... } */

module at_extra_holes() { }

module at_holes()
{
	at_bottom_below_pcb()
	at_pcb_holes() children();
	at_extra_holes() children();
}

module on_pcb()
{
	translate(pcb_pos)
	rotate([0,0,pcb_angle])
	children();
}

module at_top()
{
	translate( [ 0,0, total_height])
	children();
}

module at_bottom_below_pcb()
{
	translate( [ pcb_pos[0], pcb_pos[1], 0])
	rotate([0,0,pcb_angle])
	children();
}

module at_top_above_pcb()
{
	at_bottom_below_pcb()
	at_top()
	children();
}

module keepout()
{
	on_pcb()
	pcb_keepout();
}

module case_button()
{
	button(button_w, button_d, top_thickness, button_depth);
}

module top_features()
{
	at_top_above_pcb()
	{
		if (has_dfu_button)
		at_dfu_button()
		case_button();

		if (has_buttons)
		at_buttons()
		case_button();

		if (has_leds)
		for_each_led_array()
		light_pipe_hull()
		at_leds()
		light_pipe(light_pipe_depth, light_pipe_diameter, light_pipe_border, light_pipe_base_border, light_pipe_base_height);
	}
}

module next()
{
	translate([ 0, case_width()+padding, 0 ]) children();
}

module flip()
{
	translate([ 0, origin_y_off_center(), 0])
	rotate([180,0,0])
	translate([ 0, -origin_y_off_center(),-total_height])
	children();
}

