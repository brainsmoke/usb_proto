
use <graft.scad>

e=.001;
b=1;

u=19.05;

module keyswitch_panel(size=u, thickness=1.5, hole_size=13.8)
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

