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
	
	
	
	
	if (! l->wrapped) {
		return this->arrow_straight(p1.first,p1.second, p2.first,p2.second);
	} else {
		return this->arrow_wrapped(p1.first, p1.second, p2.first, p2.second);
	}
}

string draw::arrow_head(const double offset_angle) 
{
	const double a = 25;	// half-angle of head
	const double len = 3.0;	// side-length of head

	const double angle1 = 180-a + offset_angle;
	const double angle2 = angle1 + 2*a;

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
}

element draw::arrow_straight(const float x1, const float y1, const float x2, const float y2) 
{
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
		.Set("style", "fill")
		.Set("stroke", "#000000")
		.Set("stroke-linejoin", "round")
		.Set("stroke-linecap", "round")
		.Set("d",	
			boost::str( boost::format(
				"M 0,0 "		// always origo
				"L %1%,0 "		// length of the arrow
				"%2% "
				) % length % this->arrow_head(0))
			));

	return g;
}

element draw::arrow_wrapped(const float x1, const float y1, const float x2, const float y2) 
{
	const int stub = 3;	// the small straight line which touches the node
	const int r = 10;	// "radius" of bezier curve (appoximately a circle)
	const int w = -28;	// width of curve

	const bool same_row = (x1==x2);
	const bool same_col = (y1==y2);
	assert(same_row ^ same_col && "Arbitary positions not allowed, must be in either same row or col");
	
	const auto s1 = (same_col * ((x1 < x2) ? -stub : stub));
	const auto s2 = (same_row * ((y1 < y2) ? -stub : stub));
	
	const auto c1 = (same_col * ((x1 < x2) ? -r : r));
	const auto c2 = (same_row * ((y1 < y2) ? -r : r));
	const auto c3 = (same_col * ((x1 < x2) ? -r : r) | same_row * (-w));
	const auto c4 = (same_row * ((y1 < y2) ? -r : r) | same_col * (-w));
	const auto c5 = (same_row * (-w));
	const auto c6 = (same_col * (-w));
	
	
	return element().Tag("path")
		.Set("style", "fill")
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

				"l %20%,%21%  "
				"c %22%,%23% %24%,%25% %26%,%27%  "
				"l %28%,%29%  "
				"c %30%,%31% %32%,%33% %34%,%35%  "
			)	% x1	// 1
				% y1	// 2
		
				% s1	// 3
				% s2	// 4
			
				% c1	// 5
				% c2	// 6
				% c3	// 7
				% c4	// 8
				% c5	// 9
				% c6	// 10

				% (x2-x1 - 2*s1) // 11
				% (y2-y1 - 2*s2) // 12

				% -c1	// 13
				% -c2	// 14
				% -c3	// 15
				% -c4	// 16
				% -c5	// 17
				% -c6	// 18
			
				% this->arrow_head(same_col*180*(x2>x1) | -same_row*(90+180*(y2<y1))) // 19

				% -s1	// 20
				% -s2	// 21
			
				% (c1*(same_row ? 1 : -1))	// 22
				% (c2*(same_col ? 1 : -1))	// 23
				% (c3*(same_row ? 1 : -1))	// 24
				% (c4*(same_col ? 1 : -1))	// 25
				% (c5*(same_row ? 1 : -1))	// 26
				% (c6*(same_col ? 1 : -1))	// 27
			
				% -(x2-x1 - 2*s1) // 28
				% -(y2-y1 - 2*s2) // 29
			
				% -(c1*(same_row ? 1 : -1))	// 30
				% -(c2*(same_col ? 1 : -1))	// 31
				% -(c3*(same_row ? 1 : -1))	// 32
				% -(c4*(same_col ? 1 : -1))	// 33
				% -(c5*(same_row ? 1 : -1))	// 34
				% -(c6*(same_col ? 1 : -1))	// 35
			));
}

