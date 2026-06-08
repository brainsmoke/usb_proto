e=.0001;
b=1;
n_slats_default = 511;
/* ^ ideally this could be realy big without much overhead since
 * CSG operations are cached, but there seems to be a linear
 * component to the slowdown incurred by using more slats.
 * 
 * My hypothesis is that there's caching for CSG results,
 * but no de-duplication at the 'AST-for-CSG' generation stage.
 */

use <bbox.scad>;

module example_case()
{
rotate([10,20,30])

    hull()
	for (x = [0,100])
	for (y = [0,50])
	translate([x, y,0])
    cylinder(12, r=10);
}

module grid_orient(origin, a)
{
	translate(origin)
	rotate([    0,    0, a[2]])
	rotate([    0, a[1],    0])
	rotate([ a[0],    0,    0])
	rotate([-90,0,0])
	children();
}

module grid_orient_inv(origin, a)
{
	rotate([90,0,0])
	rotate([-a[0],    0,    0])
	rotate([    0,-a[1],    0])
	rotate([    0,    0,-a[2]])
	translate(-origin)
	children();
}

module _slat(width)
{
	linear_extrude(width, center=true)
	hull()
	projection(cut=false)
	children();
}

module _slats_recursive(pitch, width, slat_count)
{
	/* save some nodes by using a fast-path.  */
	if (slat_count == 1)
	{
		_slat(width)
		children();
	}
	else
	{
		intersection()
		{
			union()
			{
				if (slat_count % 2 == 1)
				_slat(width)
				children();

				slat_count_div = (slat_count % 2 == 1) ? (slat_count-1)/2 : slat_count/2;
				slat_off = (slat_count % 2 == 1) ? pitch : 0;

				if (slat_count_div >= 1)
				{
					translate([0,0,slat_off])
					_slats_recursive(pitch, width, slat_count_div)
					children();

					translate([0,0,slat_off+pitch*slat_count_div])
					_slats_recursive(pitch, width, slat_count_div)
					children();
				}
			}
			children();
		}
	}
}

module _slats(pitch, width, n_slats=undef)
{
		slat_count = (n_slats == undef) ? n_slats_default : max(0,n_slats);

		if (slat_count % 2 == 1)
			_slat(width) children();

		mid_pitch = (slat_count % 2 == 1) ? pitch : pitch/2;
		slat_count_div = (slat_count % 2 == 1) ? (slat_count-1)/2 : slat_count/2;

		translate([0,0,-mid_pitch])
		_slats_recursive(-pitch, width, slat_count_div) translate([0,0,mid_pitch]) children();

		translate([0,0,mid_pitch])
		_slats_recursive(pitch, width, slat_count_div) translate([0,0,-mid_pitch]) children();
}


module slats(origin, rotation, pitch, width, n_slats=undef)
{
	intersection()
	{
		grid_orient(origin, rotation)
		_slats(pitch, width, n_slats)
		bbox(e)
		grid_orient_inv(origin, rotation)
		children();

		children();
	}
}

module grid(origin, angle, pitch, width, n_rows=undef, n_cols=undef)
{
	union()
	{
		slats(origin, [0,0,angle], pitch, width, n_rows)
		children();

		slats(origin, [0,0,angle+90], pitch, width, n_cols)
		children();
	}
}

module _test_slats()
{
	origin = [20,20,0];
	rotation = [0,0,35];
	pitch = 10;
	width = .8;

	render()
	slats(origin, rotation, pitch, width) children();

	%children();
}


/*
translate([0,100,0])
_test_slats()
example_case();
*/

module _test_grid()
{
	origin = [20,20,0];
	angle=30;
	pitch = 10;
	width = .8;

	render()
	grid(origin, angle, pitch, width) children();

	%children();
}


_test_grid()
example_case();


