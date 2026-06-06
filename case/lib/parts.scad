
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

module qfn(n, A, D, D1, E, E1, L, L1, T, b, e)
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

module sot_pin()
{
	metallic()
	{
		translate([0,.3,0])
		{
			block([.5,.6,.2], [0,-1,-1]);
			block([.5,.3,.8], [0,0,-1]);
			translate([0,0,.8])
			block([.5,.6,.2], [0,1,1]);
		}
	}
}

module sot235(n1, n2)
{
	A=2.9;
	B=1.6;
	C=1.2;
	chip_package()
	block([B,A,C],[0,0,-1]);

	for (y=[-.95,0,.95])
	translate([-B/2,y,0])
	rotate([0,0,90])
	sot_pin();

	for (y=[-.95,.95])
	translate([B/2,y,0])
	rotate([0,0,-90])
	sot_pin();

	chip_notch()
	translate([-B/2+.5,A/2-.8, A-.001])
	cylinder(.021, r=.2, $fn=12);
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

preview()
{
	color("green")
	block([200,200,1],[0,0,1]);

	translate([0,0,0])
	ufqfpn28();

	translate([10,0,0])
	sot235();

	translate([20,0,0])
	kmr2();

	translate([30,0,0])
	sot666();

	translate([40,0,0])
	r0402();

	translate([50,0,0])
	r0603();

	translate([60,0,0])
	c0402();

	translate([70,0,0])
	c0603();
}

