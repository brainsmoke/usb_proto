
use <graft.scad>
use <utils.scad>

e=.001;
b=1;
$fn=30;

u=19.05;
mx_panel_hole=14;
mx_edge_dim= [15.6,15.6,0.8];
mx_panel_height=5;
mx_body_height=11.6;
mx_pin_height=3.3;
mx_center_fix_d=4;
mx_fix_pin_d=2;
mx_pin_d=.8;
mx_keystem_w = 4;
mx_keystem_d1 = 1.31;
mx_keystem_d2 = 1.17;
mx_keystem_h = 3.6;

mx_fix_pin_holes =  [ [-5.08, 0], [5.08,0] ];
mx_pin_holes =  [ [-3.81, 2.54], [2.54,5.08] ];


module at_mx_panel()
{
	translate([0,0,-mx_panel_height]) children();
}

module mx_keepout(fix_pins=true)
{
	color("gray")
	{
		translate([0,0,mx_panel_height])
		block(mx_edge_dim, [0,0,-1]);

		block([mx_panel_hole,mx_panel_hole,mx_body_height], [0,0,-1]);
		block([4,mx_panel_hole+2,mx_pin_height+e], [0,0,-1]);

		translate([0,0,mx_body_height-e])
		{
			block([4,mx_keystem_d1,mx_keystem_h+e], [0,0,-1]);
			block([mx_keystem_d2,4,mx_keystem_h+e], [0,0,-1]);
		}
	}

	translate([0,0,-mx_pin_height])
	{
		color("gray")
		{
			cylinder(h=mx_pin_height+e, d=mx_center_fix_d);

			if (fix_pins)
			for (p=mx_fix_pin_holes)
			translate(p)
			cylinder(h=mx_pin_height+e, d=mx_fix_pin_d);
		}
		color("#ffdd99")
		{
			for (p=mx_pin_holes)
			translate(p)
			cylinder(h=mx_pin_height+e, d=mx_pin_d);
		}
	}
}

module keyswitch_panel(size=u, thickness=1.5, hole_size=mx_panel_hole)
{
	graft()
	{
		graft_add()
		{
			translate([-size/2, -size/2, -thickness-e])
			cube([size, size, thickness+e]);
		}
		graft_remove()
		{
			cube([hole_size, hole_size, thickness*3+b], center=true);
			translate([-size/2, -size/2, -thickness*2])
			cube([size, size, thickness]);
		}
	}
}

graft()
keyswitch_panel();

preview()
at_mx_panel()
mx_keepout();

