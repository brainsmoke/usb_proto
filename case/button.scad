e=.001;

w=6;
d=7;
height=10;
thickness=.8;

use <sculpt.scad>;

module button_carveout(w, d, thickness, height, top_width=2, top_height=3, gap=0.5, flex_thickness=0.2)
{
rotate([0,0,180])
translate([0,(d-top_width)/2,-height])
linear_extrude(height+e)
	translate([-w/2-gap, -d/2-gap])
	square([w+2*gap, d+gap]);

}

module button_shape(w, d, thickness, height, top_width=2, top_height=3, gap=0.5, flex_thickness=0.2)
{

h=height-thickness;

rotate([0,0,180])
union()
{

translate([0,(d-top_width)/2,-thickness])
difference()
{
	linear_extrude(thickness)
	translate([-w/2, -d/2])
	square([w, d+e]);

	for (i=[d/2-gap, -gap])
	translate([-w/2-gap, i, -e])
	cube([w+2*gap, gap, thickness-flex_thickness+e]);
}

translate([0,(d-top_width)/2,-thickness+e])
{

translate([0,-d/2+thickness,0])
	rotate([90,0,0])
	linear_extrude(thickness)
	polygon([ [-w/2,0], [w/2, 0], [top_width/2, -h-e], [-top_width/2, -h-e]]);


for (i=[-w/2-gap-thickness-e, w/2+gap+e])
translate([i,0,0])
rotate([90,0,90])
	linear_extrude(thickness)
	polygon([ [-d/2, e], [d/2, e], [-d/2+top_width, -h-e], [-d/2, -h-e]]);

translate([-top_width/2,0,0])
rotate([90,0,90])
	linear_extrude(top_width)
	polygon([ [-d/2, -h-e+top_height], [-d/2+top_width, -h-e], [-d/2, -h-e]]);


translate([-w/2-gap-e,-d/2,-h])
	cube([w+(gap+e)*2, top_width, flex_thickness]);

}

}
}

module button(w, d, thickness, height, top_width=2, top_height=3, gap=0.5, flex_thickness=0.2)
{
	sculpt()
	{
		sculpt_base_carve()
			button_carveout(w, d, thickness, height, top_width, top_height, gap, flex_thickness);

		sculpt_add()
			button_shape(w, d, thickness, height, top_width, top_height, gap, flex_thickness);
	}
}

module flip()
{
	rotate([180,0,0]) children();
}

pitch=4*2.54;


flip()
sculpt()
{
	sculpt_base()
	translate([-20,-10, -thickness])
	cube([40,20,thickness]);

	for (i=[-pitch,0,pitch])
	translate([i,0,0])
	button(w, d, thickness, height);
}
