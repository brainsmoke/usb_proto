e=.001;

w=6;
d=7;
height=10;
thickness=.8;


module button_keepout(w, d, thickness, height, top_width=2, top_height=3, gap=0.5, flex_thickness=0.2)
{
rotate([0,0,180])
{
translate([0,(d-top_width)/2,-thickness-e])
linear_extrude(thickness+2*e)
difference()
{
	translate([-w/2-gap, -d/2-gap])
	square([w+2*gap, d+gap]);

	translate([-w/2, -d/2])
	square([w, d+e]);
}

translate([0,(d-top_width)/2,-thickness-e])
linear_extrude(thickness-flex_thickness+e)
for (i=[d/2-gap, -d/2+thickness])
translate([-w/2-gap, i])
square([w+2*gap, gap]);
}
}

module button_shape(w, d, thickness, height, top_width=2, top_height=3, gap=0.5, flex_thickness=0.2)
{

h=height-thickness;

rotate([0,0,180])
translate([0,(d-top_width)/2,-thickness+e])
{

translate([0,-d/2+thickness,0])
	rotate([90,0,0])
	linear_extrude(thickness)
	polygon([ [-w/2,0], [w/2, 0], [top_width/2, -h-e], [-top_width/2, -h-e]]);

for (i=[-w/2-gap-thickness, w/2+gap])
translate([i,0,0])
rotate([90,0,90])
	linear_extrude(thickness)
	polygon([ [-d/2,0], [d/2, 0], [-d/2+top_width, -h-e], [-d/2, -h-e]]);

translate([-top_width/2,0,0])
rotate([90,0,90])
	linear_extrude(top_width)
	polygon([ [-d/2, -h-e+top_height], [-d/2+top_width, -h-e], [-d/2, -h-e]]);


translate([-w/2-gap-e,-d/2,-h])
	cube([w+(gap+e)*2, top_width, flex_thickness]);

}
}


module flip()
{
	rotate([180,0,0]) children();
}

pitch=4*2.54;


flip()
{

for (i=[-pitch,0,pitch])
translate([i,0,0])
button_shape(w, d, thickness, height);

difference()
{
translate([-20,-10, -thickness])
cube([40,20,thickness]);
for (i=[-pitch,0,pitch])
translate([i,0,0])
button_keepout(w, d, thickness);
}
}
