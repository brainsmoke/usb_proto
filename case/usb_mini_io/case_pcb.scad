
use <../lib/utils.scad>
use <../lib/snap.scad>

use <pcb.scad>

include <../lib/case_base.scad>
include <../lib/case_pcb_standard.scad>

hole_dist_x=30; /* flexible, minimum: 20 */
assert(hole_dist_x >= 20);

leg_height = 2.4;

has_leds = false;
has_buttons = false;
has_dfu_button = true;

chamfer = 0;

grid_height_bottom = 1.;

module at_extra_holes()
{
	at_bottom_below_pcb()
	at_pcb_holes()
	translate([hole_dist_x, 0, 0])
	children();
}

module pcb_clip()
{
	translate([18.9,10,bottom_thickness])
	{
		snap([4,12,case_split-bottom_thickness+.7],[1.3,4,2],pcb_thickness()+1);
		graft_remove()
		block([2.5,12,case_split-bottom_thickness-pcb_thickness()+e],[1,0,-1]);
	}

	graft_add()
	for (y=[-10,10])
	{
		translate([16.5,10+y,bottom_thickness])
		{
			block([4,0.8,case_split-bottom_thickness-pcb_thickness()+e],[-1,0,-1]);

			hull()
			{
				block([.8,.8,case_split-bottom_thickness-pcb_thickness()+e],[-1,0,-1]);
				block([.8,4, e],[-1,0,-1]);
			}
		}
	}
}

module bottom_extra()
{
	graft()
	{
		on_pcb()
		at_pcb_pads()
		graft_remove()
		translate([0,0,-e])
		block([3,3,leg_height-e],[0,0,1]);

		at_bottom_below_pcb()
		pcb_clip();
	}
}

module bottom()
{
	render()
	graft()
	{
		bottom_extra();
		_bottom()
		children();
	}
}

