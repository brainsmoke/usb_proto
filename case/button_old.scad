e=.001;

w=6;
d=5;
gap=.5;
h=10;
thickness=.8;
flex_d=3;
flex_thickness=.2;
flex_pitch=2;

module button_keepout()
{
translate([0,0,-e])
linear_extrude(thickness+2*e)
difference()
{
	translate([-w/2-gap, -d/2-gap])
	square([w+2*gap, d+gap]);

	translate([-w/2, -d/2])
	square([w, d+e]);
}

translate([0,0,flex_thickness])
linear_extrude(thickness-flex_thickness+e)
for (i=[d/2-gap, -d/2+thickness])
translate([-w/2-gap, i])
square([w+2*gap, gap]);
}

module button_shape()
{
translate([-10,-10, 0])
cube([20,20,thickness]);

translate([0,0,thickness-e])
{
for (i=[-d/2+thickness, -d/2-flex_d+thickness*2])
translate([0,i,0])
	rotate([90,0,0])
	linear_extrude(thickness)
	polygon([ [-w/2,0], [w/2, 0], [flex_d/2, h], [-flex_d/2, h]]);


translate([-thickness/2,-d/2+thickness-flex_d,h-flex_thickness*2])
	cube([thickness, flex_d, flex_thickness]);
}
}

difference()
{
button_shape();
button_keepout();
}

