
module xz_toward_y()
{
	scale([1, 1, -1])
	rotate([-90,0,0])
	children();
}

module anchor(dim, anchor=[0,0,0])
{
	translate([-(1+anchor.x)*dim.x/2, -(1+anchor.y)*dim.y/2, -(1+anchor.z)*dim.z/2])
	children();
}

module block(dim, anchor=[0,0,0])
{
	anchor(dim, anchor)
	cube(dim);
}

module rounded_block_y(dim, anchor=[0,0,0], r=0)
{
	anchor(dim, anchor)
	xz_toward_y()
	linear_extrude(dim.y)
	hull()
	for (p=[ [r,r], [dim.x-r, r], [dim.x-r, dim.z-r], [r, dim.z-r] ] )
	translate(p)
	circle(r);
}

module chamfer_block_y(dim, anchor=[0,0,0], r=0)
{
	anchor(dim, anchor)
	hull()
	{
		translate([r,0,0])
		cube([dim.x-r*2, dim.y, dim.z]);

		translate([0,0,r])
		cube([dim.x, dim.y, dim.z-r*2]);
	}
}

module chamfer_block(dim, anchor=[0,0,0], r=0)
{
	anchor(dim, anchor)
	hull()
	{
		translate([r,r,0])
		cube([dim.x-r*2, dim.y-r*2, dim.z]);

		translate([r,0,r])
		cube([dim.x-r*2, dim.y, dim.z-r*2]);

		translate([0,r,r])
		cube([dim.x, dim.y-r*2, dim.z-r*2]);
	}
}

module preview()
{
	if ($preview)
	children();
}

