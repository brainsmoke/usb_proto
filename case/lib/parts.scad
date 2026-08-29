
include <utils.scad>

module chip_package()
{
	color("#303030")
	children();
}

module chip_notch()
{
	color("#707070")
	children();
}

module metallic()
{
	color("#c0c0c0")
	children();
}

module button_body()
{
	color("#a0a0a0")
	children();
}

module qfn(A, D, D1, E, E1, L, L1, T, b, e)
{
	chip_package()
	block([D,E,A], [0,0,-1]);

	metallic()
	{
		n_e = round(E1/e);
		if (n_e > 0)
		{
			for (x=[1:n_e-1])
			for (m=[1,-1])
			translate([-E1/2+x*e,m*(D/2+.02),0])
			block([b,L, A/3], [0,m,-1]);

			for (x=[0,n_e])
			for (m=[1,-1])
			translate([-E1/2+x*e,m*(D/2+.02),0])
			block([b,L1, A/3], [0,m,-1]);
		}

		n_d = round(D1/e);
		if (n_d > 0)
		{
			for (y=[1:n_d-1])
			for (m=[1,-1])
			translate([m*(E/2+.02),-D1/2+y*e,0])
			block([L,b, A/3], [m,0,-1]);

			for (y=[0,n_d])
			for (m=[1,-1])
			translate([m*(E/2+.02),-D1/2+y*e,0])
			block([L1,b, A/3], [m,0,-1]);
		}
	}

	chip_notch()
	translate([-E1/2, D1/2, A-.001])
	cylinder(.021, r=e/2, $fn=12);
}

module ufqfpn28()
{
	qfn(A=.55, D=4, D1=3, E=4, E1=3, L= .4, L1=.35, b=.25, e=.5);
}

module ufqfpn32()
{
	qfn(A=.55, D=5, D1=3.5, E=5, E1=3.5, L= .4, L1=.4, b=.23, e=.5);
}

module ufqfpn48()
{
	qfn(A=.55, D=7, D1=5.5, E=7, E1=5.5, L= .4, L1=.4, b=.25, e=.5);
}


module pin(w=.5,d=1.2,h=.8)
{
	metallic()
	{
		translate([0,d/2,0])
		{
			block([w,d/2,.2], [0,-1,-1]);
			block([w,.3,h], [0,0,-1]);
			translate([0,0,h])
			block([w,d/2,.2], [0,1,1]);
		}
	}
}

module bulge_block(dim, b, anchor)
{
	anchor(dim, anchor)
	hull()
	{
		translate([b,b,0])
		cube([dim[0]-2*b,dim[1]-2*b,dim[2]]);

		translate([0,0,dim[2]/2-.0005])
		cube([dim[0],dim[1],.001]);
	}
}

module lqfp_block(dim, anchor)
{
	b = dim[2]/10;
	bulge_block(dim-[-b*2,-b/2, 0], b, anchor);
}

module lqfp(A1, A2, D, D1, D3, E, E1, E3, e, L, L1, T, b)
{
	chip_package()
	lqfp_block([D1,E1,A2], [0,0,-1]);

	metallic()
	{
		n_e = round(E3/e);
		if (n_e > 0)
		{
			for (r=[0, 180])
			rotate([0,0,r])
			for (x=[0:n_e])
			translate([-E3/2+x*e,D1/2,0])
			pin(b, L1, (A2+A1)/2);
		}

		n_d = round(D3/e);
		if (n_d > 0)
		{
			for (r=[90, 270])
			rotate([0,0,r])
			for (x=[0:n_d])
			translate([-D3/2+x*e,E1/2,0])
			pin(b, L1, (A2+A1)/2);
		}
	}
	chip_notch()
	translate([-E3/2, D3/2, A2-.001])
	cylinder(.021, r=e/2, $fn=12);
}


module lqfp32()
{
	lqfp(A1=.1, A2=1.4, D=9, D1=7, D3=5.6, E=9, E1=7, E3=5.6, L=.6, L1=1, e=.8, b=.37);
}

module lqfp48()
{
	lqfp(A1=.1, A2=1.4, D=9, D1=7, D3=5.5, E=9, E1=7, E3=5.5, L=.6, L1=1, e=.5, b=.22);
}

module lqfp64()
{
	lqfp(A1=.1, A2=1.4, D=12, D1=10, D3=7.5, E=12, E1=10, E3=7.5, L=.6, L1=1, e=.5, b=.22);
}

module lqfp100()
{
	lqfp(A1=.1, A2=1.4, D=16, D1=14, D3=12, E=16, E1=14, E3=12, L=.6, L1=1, e=.5, b=.22);
}

module so5()
{
	D=4.55;
	E=3.7;
	A=2.1;
	pitch = 1.27;

	for (y=[-pitch,pitch])
	translate([-1.5,y,0])
	rotate([0,0,90])
	pin(.5, 2., A/2);

	for (y=[-pitch,0,pitch])
	translate([1.5,y,0])
	rotate([0,0,-90])
	pin(.5, 2., A/2);

	chip_package()
	bulge_block([D,E,A], .1, [0,0,-1]);

	chip_notch()
	translate([-D/2+.8,E/2-.8,A-.001])
	cylinder(.021, r=.4, $fn=12);
}

module sot23x(row_top, row_bottom)
{
	D=1.6;
	E=2.9;
	A=1.2;
	chip_package()
	block([D,E,A],[0,0,-1]);

	for (y=row_bottom)
	translate([-D/2+.3,y,0])
	rotate([0,0,90])
	pin(.5, 1.2, .8);

	for (y=row_top)
	translate([D/2-.3,y,0])
	rotate([0,0,-90])
	pin(.5, 1.2, .8);

	chip_notch()
	translate([-D/2+.5,E/2-.5,A-.001])
	cylinder(.021, r=.2, $fn=12);
}

module sot23()
{
	sot23x([0], [-.95,.95]);
}

module sot233()
{
	sot23();
}

module sot235()
{
	sot23x([-.95,.95], [-.95,0,.95]);
}

module sot236()
{
	sot23x([-.95, 0,.95], [-.95,0,.95]);
}

module sot666()
{
	D=1.5;
	E=1.1;
	A=0.5;
	e=0.5;
	b=.23;
	c=.2;
	lp=.2;

	chip_package()
	block([E,D,A], [0,0,-1]);

	metallic()
	{
		for (y=[-e,0,e])
		for (m=[1,-1])
		translate([m*E/2,y,0])
		block([lp,b,c], [-m,0,-1]);
	}

	chip_notch()
	translate([-E/2+e/2,D/2-e/2, A-.001])
	cylinder(.021, r=e/4, $fn=12);
}

module kmr2()
{

	chip_package()
	block([4,2.2,1.2],[0,0,-1]);

	button_body()
	translate([0,0,1.4])
	block([4,2.2,.2],[0,0,1]);

	color("black")
	cylinder(1.9, d=1.6, $fn=12);

	metallic()
	for (y=[-.8, .8])
	for (m=[1,-1])
	translate([m*2.1,y,0])
	block([.2,.6,.2], [-m,0,-1]);
}

module rDEA(D,E,A,c)
{
	for (m=[1,-1])
	translate([m*D/2, 0, 0])
	metallic()
	block([D/4,E,A], [m,0,-1]);

	color(c)
	block([D*.99, E*.99, A*.99],[0,0,-1]);
}

module r0402()
{
	inch = 25.4;
	D=.04*inch;
	E=.02*inch;
	A=.014*inch;

	rDEA(D,E,A,"black");
}

module r0603()
{
	inch = 25.4;
	D=.06*inch;
	E=.03*inch;
	A=.018*inch;

	rDEA(D,E,A,"black");
}

module c0402()
{
	inch = 25.4;
	D=.04*inch;
	E=.02*inch;
	A=.02*inch;

	rDEA(D,E,A,"#807030");
}

module c0603()
{
	inch = 25.4;
	D=.06*inch;
	E=.03*inch;
	A=.03*inch;

	rDEA(D,E,A,"#807030");
}

module l0603()
{
	inch = 25.4;
	D=.06*inch;
	E=.03*inch;
	A=.03*inch;

    metallic()
	block([D,E,A/3], [0,0,-1]);

	color("white")
	block([E,E,A], [0,0,-1]);
}

module display(pitch=15)
{
	color("green")
	block([($children+1)*pitch, 2*pitch,1],[-1,0,1]);
	for (i = [0:$children-1])
	translate([pitch*(i+1),0,0])
	children(i);
}

preview()
{
	display()
	{
		ufqfpn28();
		ufqfpn32();
		ufqfpn48();
		lqfp32();
		lqfp48();
		lqfp64();
		lqfp100();
		so5();
		sot23();
		sot233();
		sot235();
		sot236();
		sot666();
		kmr2();
		r0402();
		r0603();
		c0402();
		c0603();
	}
}

