
include <case_base.scad>

leg_height = 1.4;

pcb_depth = 22;
pcb_radius_back = 2;
pcb_thickness = 1;

hole_dist_y=20;
hole_dist_x=30;

has_leds = false;
has_buttons = false;
has_dfu_button = true;

grid_x_off = 2;
grid_cols = 10;
grid_height_bottom = 1.2;

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

module pcb_shape(height)
{
	hull()
	{
		at_pcb_holes() cylinder(height, r=pcb_radius);
		for (x = [pcb_depth-pcb_radius-pcb_radius_back])
		for (y = [pcb_radius_back-pcb_radius, hole_dist_y+pcb_radius-pcb_radius_back])
		translate([x,y,0])
		cylinder(height, r=pcb_radius_back);
	}
}

function breadboard_pos(x, y) = [ 15 + 2.54*x, hole_dist_y/2 + (4.5-y)*2.54, component_z ];

module pcb()
{
	difference()
	{
		union()
		{
			color("green")
			translate([0,0,component_z-pcb_thickness])
			pcb_shape(pcb_thickness);
 
			for (y=[0:9])
			breadboard_hole_silk(0, y);
 
			for (y=[6:9])
			breadboard_hole_silk(-1, y);
		}
 
		union()
		{
			translate([0,0,component_z-pcb_thickness-b])
			at_holes()
			cylinder(pcb_thickness+2*b, r=pcb_screw_hole_diameter/2);
 
			for (y=[0:9])
			breadboard_hole(0, y);
 
			for (y=[6:9])
			breadboard_hole(-1, y);
		}
	}

}

