
key_pitch = 19;

include <case_pcb.scad>

use <../lib/keyswitch.scad>

/* overrides */

grid_origin = pcb_center();
grid_rows = 5;
grid_cols = 9;
grid_pitch=key_pitch/2;
grid_width=1.2;

has_buttons=false;
has_dfu_button=false;
has_leds=false;

chamfer=1.2;

panel_dim = key_pitch-grid_width;

n_cols=4;
n_rows=2;

bottom();

next() flip() top()
{
	top_features();

    translate(pcb_center())
	at_top()
    translate([-(n_cols)*key_pitch/2,-(n_rows)*key_pitch/2,0])
    for (x = [ 0 : n_cols-1 ])
    for (y = [ 0 : n_rows-1 ])
    translate([x*key_pitch+key_pitch/2,y*key_pitch+key_pitch/2,0])
    keyswitch_panel(panel_dim);
};

