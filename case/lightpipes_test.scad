
include <case_base.scad>


with_buttons=true;
with_light_pipes=true;
flip()
difference()
{
union(){
translate([55,-8,total_height-1])
cube([10,30,1]);

			hull() { at_light_pipes() light_pipe();


		}}
keepout_zones();
}

