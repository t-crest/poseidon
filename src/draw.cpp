#include "draw.hpp"

draw::draw(network_t& _n) 
:	n(_n), scale(60), bezel(20), router_size(scale/2)
{
	const string bezel_s = ::lex_cast<string>(bezel);

	const int width = bezel + n.cols()*scale + bezel;
	const int height = bezel + n.rows()*scale + bezel;
	
	root.Tag("svg")
		.Set("width",width).Set("height",height).Set("version",1.1)
		.Set("xmlns","http://www.w3.org/2000/svg")
		.Set("xmlns:xlink", "http://www.w3.org/1999/xlink");

	element background = element().Tag("rect")
		.Set("id","background")
		.Set("x",0).Set("y",0)
		.Set("width",width).Set("height", height)
		.Set("fill","white");
	root.AddChild(background);

	element g = element().Tag("g")
		.Set("transform","translate("+bezel_s+","+bezel_s+")");

	element layer1 = element().Tag("g");
	element layer2 = element().Tag("g");
	
	
	for (int x = 0; x < n.cols(); x++) 
	for (int y = 0; y < n.rows(); y++) {
		
		const int rect_x = x*scale;
		const int rect_y = y*scale;
		
		layer1.AddChild(element().Tag("rect")
			.Set("x",rect_x).Set("y",rect_y) // coordinates are to the top left corner
			.Set("width",scale).Set("height",scale)
			.Set("fill", "none").Set("stroke","#999999").Set("stroke-width",1));

		layer1.AddChild(element().Tag("text")
			.Set("x",rect_x).Set("y",rect_y)
			.Set("dy","1.5ex").Set("fill", "#D0D0D0")
			.Set("font-size", 10).Set("text-anchor","start").SetValue(router_id(x,y)));
		
		if (!n.has({x,y})) continue; // skip non-existant routers

		const int circle_x = x*scale+scale/2;
		const int circle_y = y*scale+scale/2;
		
		layer1.AddChild(element().Tag("circle")
			.Set("cx",circle_x).Set("cy",circle_y) // coordinates are to the center 
			.Set("r",router_size/2)
			.Set("fill", "#ffeecc").Set("stroke", "#ddaa66").Set("stroke-width", 2)
		);
	}

	
	for_each(n.links(), [&layer2,this](link_t *l){
		layer2.AddChild(this->link(l));
		debugf(*l);
	});
	
	
	g.AddChild(layer1);
	g.AddChild(layer2);
	root.AddChild(g);
}

std::pair<int,int> draw::coords(const port_t& p) {
	const int router_x = p.parent.address.first*scale + router_size;
	const int router_y = p.parent.address.second*scale + router_size;
	
	int x = router_x;
	int y = router_y;

	switch (p.corner) {
		case N: y -= router_size/2; break;
		case S: y += router_size/2; break;
		case E: x += router_size/2; break;
		case W: x -= router_size/2; break;
	}
	
	return make_pair(x,y);
}

element draw::link(link_t *l) {
	std::pair<int,int> p1 = coords(l->source);
	std::pair<int,int> p2 = coords(l->sink);

	if (!l->wrapped) {
		return this->arrow(p1.first,p1.second, p2.first,p2.second);
	} else {
		return this->curve_arrow(p1.first, p1.second, p2.first, p2.second);
		
//		const bool same_row = (p1.second == p2.second);
//		if (same_row) {
//			if (p1.first < p2.first) 
//				return arrow_rightwards(p1.first, p1.second, p2.first);
//			else
//				return arrow_leftwards(p1.first, p1.second, p2.first);
//		}
		
	}
}


element draw::arrow_rightwards(float x1, float y, float x2){
	debugf("right");
	
	element g = element().Tag("g")
//		.Set("transform","translate("+::lex_cast<string>(x2)+","+::lex_cast<string>(y)+")")
//		.AddChild(this->curve_arrow(x1,x2)
	;
	
	return g;
}

element draw::arrow_leftwards(float x1, float y, float x2){
	debugf("left");

	element g = element().Tag("g")
//		.Set("transform","translate("+::lex_cast<string>(x2)+","+::lex_cast<string>(y)+") scale(-1,1)")
//		.AddChild(this->curve_arrow(x1,x2)
	;
	
	return g;
}


string draw::arrow_head(const double offset_angle) 
{
	const double a = 45;
	const double angle1 = 180-a + offset_angle;
	const double angle2 = angle1 + 2*a;

	const double len = 5.0;
	static double pi = std::atan(1.0)*4.0;
	static double deg2rad = pi/180.0;

	double x1 = len*std::cos(angle1*deg2rad);
	double y1 = len*std::sin(angle1*deg2rad);

	double x2 = len*std::cos(angle2*deg2rad);
	double y2 = len*std::sin(angle2*deg2rad);
	
	return boost::str( boost::format(
		"l %1%,%2% "
		"l %3%,%4% "
		"l %5%,%6% "
	)	% x1 % y1	// 1 and 2
		% (x2-x1) % (y2-y1)
		% -x2 % -y2
	);
	
	
//	return boost::str( boost::format(
//		"l -%1%,-%2% "	// The arrow head starts here...
//		"l %3%,%2% "	// ...
//		"l -%3%,%2% "	// ...
//		"l %1%,-%2% "	// ...
//	) % w /*head width*/ % l /*head length*/ % m /*miter joint*/);
}


element draw::curve_arrow(float x1, float y1, float x2, float y2) 
{
	const int stub = 3;
	const int a = 15;
	const int b = 20;

	const bool same_row = (x1 == x2);
	const bool same_col = (y1 == y2);
	assert(same_row ^ same_col && "Arbitary positions not allowed, must be in either same row or col");
	
	return element().Tag("path")
		.Set("style", "fill:none")
		.Set("stroke", "#000000")
		.Set("stroke-linejoin", "round")
		.Set("stroke-linecap", "round")
		.Set("d",	
			boost::str( boost::format(
				"m %1%,%2%  " // x1,y1, always start in p1

				"l %3%,%4%  " 
				"c %5%,%6% %7%,%8% %9%,%10%  "
				"l %11%,%12%  "
				"c %13%,%14% %15%,%16% %17%,%18%  "
				"l %3%,%4%  "
			
				"%19%  "
			
			)	% x1 % y1 // 1 2
		
				% ((y1==y2) * ((x1 < x2) ? -stub : stub))	// 3
				% ((x1==x2) * ((y1 < y2) ? -stub : stub))	// 4
			
				% ((y1==y2) * ((x1 < x2) ? -a : a))			// 5
				% ((x1==x2) * ((y1 < y2) ? -a : a))			// 6
				% ((y1==y2) * ((x1 < x2) ? -a : a) | (x1==x2) * (-b)) // 7
				% ((x1==x2) * ((y1 < y2) ? -a : a) | (y1==y2) * (-b)) // 8
				% ((x1==x2) * (-b))							// 9
				% ((y1==y2) * (-b))							// 10

				% (x2-x1 + 2*(y1==y2) * ((x1 < x2) ? stub : -stub)) // 11
				% (y2-y1 + 2*(x1==x2) * ((y1 < y2) ? stub : -stub)) // 12

				% ((y1==y2) * ((x1 < x2) ? a : -a))			// 13
				% ((x1==x2) * ((y1 < y2) ? a : -a))			// 14
				% ((y1==y2) * ((x1 < x2) ? a : -a) | (x1==x2) * (b)) // 15
				% ((x1==x2) * ((y1 < y2) ? a : -a) | (y1==y2) * (b)) // 16
				% ((x1==x2) * (b))							// 17
				% ((y1==y2) * (b))							// 18
			
				% this->arrow_head(  (y1==y2) *180* (x2>x1)  |   -(x1==x2)*(90 + 180*(y2<y1))     )
			
			));

//				"l %4%,0 "
//				"c 15,0 15,-20 0,-20 "
//				"l -%5%,0  "
//				"c -15,0 -15,20 0,20 "
//				"l %4%,0 "
//
//				"l -%4%,0 "
//				"c -15,0 -15,-20 0,-20 "
//				"l %5%,0 "
//				"c 15,0 15,20 0,20 "
//			)	% 5 /*head width*/ % 2.5 /*head length*/ % 1 /*miter joint*/
//				% linestub % (x2-x1+2*linestub) /*longline*/)
}




element draw::arrow(float x1, float y1, float x2, float y2) {
	static const float pi = std::atan(1.0)*4.0;
	static const float rad2deg = 180.0/pi;
	
	const float angle = std::atan2(y2-y1, x2-x1) * rad2deg;
	
	const float dx = x2-x1;
	const float dy = y2-y1;
	const float length = std::sqrt(dx*dx + dy*dy);

	/* This arrow is made up of a path and a transformation.
	 * The path is always horizontal, starts in 0,0 and goes some length L to L,0.
	 * This is then transformed to correspond to the desired starting and end point.
	 * In SVG, uppercase letters denote absolute coordinates; lowercase is relative.
     */
	element g = element().Tag("g")
		.Set("transform","translate("+::lex_cast<string>(x1)+","+::lex_cast<string>(y1)+") rotate("+::lex_cast<string>(angle)+")")
		.AddChild(element().Tag("path")
			.Set("style", "fill").Set("stroke", "#000000")
			.Set("d",	boost::str( boost::format(
							"M 0,0 "		// always origo
							"L %4%,0 "		// length of the arrow
							"l -%1%,-%2% "	// The arrow head starts here...
							"l %3%,%2% "	// ...
							"l -%3%,%2% "	// ...
							"l %1%,-%2% "	// ...
						) % 5 /*head width*/ % 2.5 /*head length*/ % 1 /*miter joint*/ % length)
			));

	return g;
}

