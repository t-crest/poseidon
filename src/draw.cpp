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
		layer2.AddChild(this->draw_link(l));
		debugf(*l);
	});
	
	
	g.AddChild(layer1);
	g.AddChild(layer2);
	root.AddChild(g);
}

std::pair<int,int> draw::coords(const port_t& p) {
	const int center_x = p.parent.address.first*scale + router_size;
	const int center_y = p.parent.address.second*scale + router_size;
	
	int x = center_x;
	int y = center_y;

	switch(p.corner) {
		case N: y -= router_size/2; break;
		case S: y += router_size/2; break;
		case E: x += router_size/2; break;
		case W: x -= router_size/2; break;
	}
	
	return make_pair(x,y);
}

element draw::draw_link(link_t *l) {
	std::pair<int,int> p1 = coords(l->source);
	std::pair<int,int> p2 = coords(l->sink);
	
	return this->arrow(p1.first,p1.second, p2.first,p2.second);
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
			.Set("d",	"M 0,0 "	// always origo
						"L "+::lex_cast<string>(length)+",0 " // some length
						"l -20,-5 "	// The arrow head starts here...
						"l 5,5 "	// ...
						"l -5,5 "	// ...
						"l 20,-5 "	// ...
			));
	
	return g;
}

