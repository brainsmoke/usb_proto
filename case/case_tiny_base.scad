
include <case_base.scad>

use <utils.scad>
use <snap.scad>

leg_height = 1.4;

pcb_depth = 22;
pcb_radius_back = 2;
pcb_thickness = 1;

hole_dist_y=20;
hole_dist_x=30; /* flexible, minimum: 20 */

has_leds = false;
has_buttons = false;
has_dfu_button = true;

grid_x_off = 2;
grid_cols = 10;
grid_height_bottom = 1.4;

module at_pcb_holes()
{
    for (x = [0])
        for (y = [0, hole_dist_y])
            translate([x,y,0])
                children();
}

module at_extra_holes()
{
    for (x = [hole_dist_x])
        for (y = [0, hole_dist_y])
            translate([x,y,0])
                children();
}

module pcb_clip()
{
	translate([pcb_depth-pcb_radius+.4,hole_dist_y/2,bottom_thickness])
	{
		snap([4,12,component_z-bottom_thickness+.8],[1.4,4,2.2],1);
		graft_remove()
		block([2.5,12,component_z-bottom_thickness-pcb_thickness+e],[1,0,-1]);
	}

}

module bottom_extra()
{
	at_pcb_pads()
	graft_remove()
	translate([0,0,-pcb_thickness+e])
	block([3,3,leg_height+e],[0,0,1]);

	pcb_clip();
}

module pcb_shape()
{
	hull()
	{
		at_pcb_holes() cylinder(pcb_thickness, r=pcb_radius);
		for (x = [pcb_depth-pcb_radius-pcb_radius_back])
		for (y = [pcb_radius_back-pcb_radius, hole_dist_y+pcb_radius-pcb_radius_back])
		translate([x,y,0])
		cylinder(pcb_thickness, r=pcb_radius_back);
	}
}

function breadboard_pos(x, y) = [ 15 + 2.54*x, hole_dist_y/2 + (4.5-y)*2.54, component_z ];

module at_pcb_pads()
{
	for (y=[0:9])
	translate(breadboard_pos(0, y))
	children();

	for (y=[6:9])
	translate(breadboard_pos(-1, y))
	children();

}

module pcb()
{
	difference()
	{
		union()
		{
			color("green")
			translate([0,0,component_z-pcb_thickness])
			pcb_shape();
 
			color("white")
			at_pcb_pads()
			pcb_pad_silk();
		}
 
		union()
		{
			translate([0,0,component_z-pcb_thickness-b])
			at_pcb_holes()
			cylinder(pcb_thickness+2*b, r=pcb_screw_hole_diameter/2);
 
			at_pcb_pads()
			pcb_pad();
		}
	}

}

