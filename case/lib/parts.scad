
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

module gold()
{
	color("#c0c040")
	children();
}

module button_body()
{
	color("#a0a0a0")
	children();
}

module led_cathode()
{
	color("#2fff7f")
	children();
}

module xtal_base()
{
	color("#777777")
	children();
}

module coil_body()
{
	color("#777777")
	children();
}

/* measurement symbols from stm32f0[47]2 datasheets */
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

module so_block(dim, chamfer, anchor)
{
	anchor(dim, anchor)
	hull()
	{
		cube(dim+[0,-0, -chamfer]);

		translate([chamfer,0,0])
		cube(dim + [-2*chamfer,0, 0]);
	}
}

module xtal_block(dim, chamfer, anchor)
{
	anchor(dim, anchor)
	{
		hull()
		{
			translate([chamfer,0,0])
			cube(dim+[-2*chamfer, 0, -chamfer]);

			translate([0, chamfer,0])
			cube(dim+[0, -2*chamfer, -chamfer]);

			translate([chamfer,chamfer,0])
			cube(dim+[-2*chamfer, -2*chamfer, 0]);
		}
	}
}

module xtal(dim, chamfer, anchor)
{
	anchor(dim, anchor)
	{
		metallic()
		render()
		difference()
		{
			xtal_block(dim, chamfer, [-1,-1,-1]);
			translate([-1,-1,-1])
			cube(dim+[2,2,1-.01]);
		}

		gold()
		render()
		intersection()
		{
			xtal_block(dim, chamfer, [-1,-1,-1]);
			translate([-1,-1,dim[2]-chamfer])
			cube([dim[0]+2,dim[1]+2,chamfer-.01]);
		}

		xtal_base()
		render()
		intersection()
		{
			xtal_block(dim, chamfer, [-1,-1,-1]);
			translate([-1,-1,-1])
			cube(dim+[2,2,1-chamfer]);
		}
	}
}

module lqfp_block(dim, anchor)
{
	b = dim[2]/10;
	bulge_block(dim-[-b*2,-b/2, 0], b, anchor);
}

/* measurement symbols from stm32f0[47]2 datasheets */
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

/* measurement symbols from attiny204/1614 datasheets */
module sox(n,A1,A2,D,E,E1,L1,b,e,c=0)
{
	chip_package()
	so_block([E1,D,A2], c, [0,0,-1]);

	metallic()
	{
		for (x=[0:n/2-1])
		for (r=[90,270])
		rotate([0,0,r])
		translate([e*(x-(n/2-1)/2),E1/2,0])
		pin(b, L1, (A2+A1)/2);
	}

	chip_notch()
	translate([-E1/2+e/2+c/2, e*((n/2-1)/2),, A2-.001])
	cylinder(.021, r=e*.2, $fn=12);
}

module so8(width=3.9)
{
	sox(n=8,A1=.25,A2=1.25,D=4.9,E=6,E1=width,L1=1.04,b=.41,e=1.27,c=.375);
}

module so14(width=3.9)
{
	sox(n=14,A1=.25,A2=1.25,D=8.65,E=6,E1=width,L1=1.04,b=.41,e=1.27,c=.375);
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

module led0603()
{
	inch = 25.4;
	D=.06*inch;
	E=.03*inch;
	A=.03*inch;

    metallic()
	block([D,E,A/3], [0,0,-1]);

	color("white")
	block([E,E,A], [0,0,-1]);

	translate([-E/2,0,A/3])
	led_cathode()
	block([(D-E)/5,E,.01],[1,0,-1]);
}

// DI ap02002
module diode(A,B,C,D,E,G,H,J)
{
	body_dim = [B,A,J];

	chip_package()
	bulge_block(body_dim, J/10, [0,0,-1]);

	metallic()
	render()
	for (r=[0,180])
	rotate([0,0,r])
	translate([E/2-H,0,0])
	difference()
	{
	block([H,C,J/2],[-1,0,-1]);
	translate([-1,-1,D])
	block([H+1-D,C+2,J/2-2*D],[-1,0,-1]);
	}

	chip_notch()
	translate([-B/2+J/10+A/10,0,J])
	block([A/10,A-J/5,.01],[-1,0,-1]);
}

module d_sma()
{
	diode(
	A=(2.29+2.92)/2,
	B=(4+4.6)/2,
	C=(1.27+1.63)/2,
	D=(.15+.31)/2,
	E=(4.8+5.59)/2,
	G=(.05+.2)/2,
	H=(.76+1.52)/2,
	J=(2.01+2.3)/2
	);
}

module coil(w,h)
{
	coil_body()
	{
		ngon_inner(h=.3,r=w/2, n=8);
		block([w,w*7/9,.3],anchor=[0,0,-1]);

		translate([0,0,h])
		{
		block([w,w*7/9,.3],anchor=[0,0,1]);
		translate([0,0,-.3])
		ngon_inner(h=.3,r=w/2, n=8);
		}
	}

	chip_package()
	translate([0,0,.3])
	union()
	{
		cylinder(h=h-2*.3,r1=w/2,r2=w/3,$fn=20);
		cylinder(h=h-2*.3,r1=w/3,r2=w/2,$fn=20);
		cylinder(h=h-2*.3,r=w/2.2,$fn=20);
	}
}

module inductor_3015()
{
	coil(3.,1.5);
}

module display(pitch=15)
{
	w = 8;
	h = floor(($children+w-1)/w);

	color("green")
	block([(w+1)*pitch, (h+1)*pitch,1],[-1,1,1]);
	for (x=[0:w-1])
	for (y=[0:h-1])
	{
		i=x+y*w;
		if (i < $children)
		translate([pitch*(x+1),-pitch*(y+1),0])
		children(i);
	}
}

module xtal3225()
{
	xtal([3.2,2.5,.6],.2, [0,0,-1]);
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
		so8();
		so14();
		xtal3225();
		d_sma();
		inductor_3015();
		kmr2();
		r0402();
		r0603();
		c0402();
		c0603();
		l0603();
	}
}

