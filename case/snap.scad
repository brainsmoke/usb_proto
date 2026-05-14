
use <utils.scad>
use <graft.scad>

e=.001;

module snap(dim, dim_overhang, grip, border=.8)
{
	graft()
	{
		graft_add()
		{
			translate([0,dim.y/2,0])
			rotate([90,0,0])
			linear_extrude(height=dim.y)
			polygon([
				[0,0],
				[0,dim.z],
				[border*2,dim.z],
				[dim.x,border*2],
				[dim.x,0],
			]);

			hull()
			{
				translate([border,0,dim.z])
				block([dim_overhang.x+border,dim_overhang.y/2,e], [1,0,1]);

				translate([0,0,dim.z-dim_overhang.z/2])
				block([border,dim_overhang.y,e], [-1,0,-1]);

				translate([0,0,dim.z+dim_overhang.z/2])
				block([border,dim_overhang.y,e], [-1,0,1]);
			}

			hull()
			{
				translate([0,0,dim.z])
				block([border,dim.y,e], [-1,0,1]);

				translate([0,0,dim.z+dim_overhang.z/2])
				block([border,dim_overhang.y,grip],[-1,0,-1]);
			}
		}
		graft_remove()
		{
			translate([border,0,-e])
			block([dim.x-border+e,dim.y-2*border,dim.z+2*e], [-1,0,-1]);

			difference()
			{
				translate([-e,0,-e])
				block([border+2*e,dim.y-2*border,dim.z-dim_overhang.z/2-border+e], [-1,0,-1]);
				translate([-e,0,-e])
				block([border+4*e,dim_overhang.y,dim.z-dim_overhang.z/2-border+3*e], [-1,0,-1]);
			}

			translate([border,0,0])
			hull()
			{
				translate([border,0,dim.z])
				block([dim_overhang.x+border,dim_overhang.y/2-border*sqrt(2),e], [1,0,1]);

				translate([0,0,dim.z-dim_overhang.z/2])
				block([border,dim_overhang.y-border*sqrt(2),e], [-1,0,-1]);

				translate([0,0,dim.z+dim_overhang.z/2])
				block([border,dim_overhang.y-border*sqrt(2),e], [-1,0,1]);
			}
		}
	}
}


snap([4,12,3.2+.8],[1.4,4,2.2],1);
