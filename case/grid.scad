
module bars(width, height, n, pitch, bar_width)
{
	for (i=[0:n-1])
		translate([0, (i-(n-1)/2)*pitch, 0])
			cube([width, bar_width, height]);
}

module grid(width, depth, height, n_rows, n_cols, pitch=10, x_pitch=-1, y_pitch=-1, bar_width=1, x_bar_width=-1, y_bar_width=-1)
{
	x_pitch = x_pitch > 0 ? x_pitch : pitch;
	y_pitch = y_pitch > 0 ? y_pitch : pitch;
	x_bar_width = x_bar_width > 0 ? x_bar_width : bar_width;
	y_bar_width = y_bar_width > 0 ? y_bar_width : bar_width;

	translate( [0, depth/2, 0] )
		bars( width, height, n_rows, y_pitch, y_bar_width );
	translate( [width/2, 0, 0] )
		rotate([0,0,90])
			bars( depth, height, n_cols, x_pitch, x_bar_width) ;
}

grid(100, 50, 1, 4, 8);
