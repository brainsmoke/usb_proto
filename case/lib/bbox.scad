
module get_x_axis(y, z)
{
	rotate([-90,0,0])
	linear_extrude(z, center=true)
	projection()
	rotate([90,0,0])
	linear_extrude(y, center=true)
	hull()
	projection()
	children();
}

module get_y_axis(z, x)
{
	rotate([0,0,-90])
	get_x_axis(z, x)
	rotate([0,0,90])
	children();
}

module get_z_axis(x, y)
{
	rotate([0,-90,0])
	get_x_axis(x, y)
	rotate([0,90,0])
	children();
}

module bbox(margin=0)
{
	if (margin == 0)
	{
		intersection()
		{
			translate([-1,-1,-1])
			bbox(1)
			children();

			translate([ 1, 1, 1])
			bbox(1)
			children();
		}
	}
	else
	minkowski()
	{
		get_x_axis(margin, margin)
		children();

		get_y_axis(margin, margin)
		children();

		get_z_axis(margin, margin)
		children();
	}
}

module _test_bbox()
{
	children();

	color("red")
	get_x_axis(1, 1)
	children();

	color("green")
	get_y_axis(1, 1)
	children();

	color("blue")
	get_z_axis(1, 1)
	children();

	%bbox() children();
}

_test_bbox()
{
	rotate([10,20,30])
    hull()
	for (x = [0,100])
	for (y = [0,50])
	translate([x, y,0])
    cylinder(12, r=10);
}

