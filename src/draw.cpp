
#include "draw.hpp"

using namespace std;

namespace snts {

/**
 * The constructor of the draw class when the caller whishes to draw the network topology.
 * @param _n The network data structure.
 */
draw::draw(const snts::network_t& _n)
: n(_n), scale(60), bezel(20), router_size(scale / 2) {
	this->init();
}

/**
 * The constructor of the draw class when the caller whishes to draw the communication channels in the given timeslot.
 * @param _n The network data structure.
 * @param _t The timeslot in which to draw the communication channels.
 */
draw::draw(const snts::network_t& _n, timeslot _t)
: n(_n), scale(60), bezel(20), router_size(scale / 2), t(_t) {
	this->init();
}

/**
 * Initializing the xml structure and calls the drawing functions.
 */
void draw::init() {
	const string bezel_s = ::lex_cast<string > (bezel);

	const int width = bezel + n.cols() * scale + bezel;
	const int height = bezel + n.rows() * scale + bezel;

	root.Tag("svg")
		.Set("width", width).Set("height", height).Set("version", 1.1)
		.Set("xmlns", "http://www.w3.org/2000/svg")
		.Set("xmlns:xlink", "http://www.w3.org/1999/xlink");

	element background = element().Tag("rect")
		.Set("id", "background")
		.Set("x", 0).Set("y", 0)
		.Set("width", width).Set("height", height)
		.Set("fill", "white");
	root.AddChild(background);

	element g = element().Tag("g")
		.Set("transform", "translate(" + bezel_s + "," + bezel_s + ")");

	element layer1 = element().Tag("g");
	element layer2 = element().Tag("g");


	for (int x = 0; x < n.cols(); x++)
		for (int y = 0; y < n.rows(); y++) {

			const int rect_x = x*scale;
			const int rect_y = y*scale;

			//		layer1.AddChild(element().Tag("rect") // draw tile as background
			//			.Set("x",rect_x)
			//			.Set("y",rect_y) // coordinates are to the top left corner
			//			.Set("width",scale)
			//			.Set("height",scale)
			//			.Set("fill", "lightgrey")
			//			.Set("stroke","white")
			//			.Set("stroke-width",2));

			const int circle_x = x * scale + scale / 2;
			const int circle_y = y * scale + scale / 2;

			if (n.has({x, y})) {
			layer1.AddChild(element().Tag("circle")
				.Set("cx", circle_x)
				.Set("cy", circle_y) // coordinates are to the center 
				.Set("r", router_size / 2)
				.Set("fill", "#ffeecc")
				.Set("stroke", "#ddaa66")
				.Set("stroke-width", 2)
				);
		}

			layer1.AddChild(element().Tag("text")
				.Set("x", circle_x)
				.Set("y", circle_y)
				.Set("dy", "0.5ex")
				.Set("fill", "grey")
				.Set("font-size", 10)
				.Set("text-anchor", "middle")
				.SetValue(router_id(x, y)));
		}

	for_each(n.links(), [&layer2, this](link_t * l) {
		layer2.AddChild(this->link(l));
	});


	g.AddChild(layer1);
	g.AddChild(layer2);
	root.AddChild(g);
}

/**
 * Calculating the coordinates of a router.
 * @param p The port of the wanted router.
 * @return Returns a pair with the coordinates of the router.
 */
std::pair<int, int> draw::coords(const port_t& p) {
	const int router_x = p.parent.address.first * scale + router_size;
	const int router_y = p.parent.address.second * scale + router_size;

	int x = router_x;
	int y = router_y;

	switch (p.corner) {
		case N: y -= router_size / 2;
			break;
		case S: y += router_size / 2;
			break;
		case E: x += router_size / 2;
			break;
		case W: x -= router_size / 2;
			break;
	}

	return make_pair(x, y);
}

/**
 * Draws the given link.
 * @param l The given link.
 * @return Returns the element containing the svg element representing the link.
 */
element draw::link(link_t *l) {
	std::pair<int, int> p1 = coords(l->source);
	std::pair<int, int> p2 = coords(l->sink);

	const bool same_row = (p1.second == p2.second);
	const bool same_col = (p1.first == p2.first);
	const int d = 2; // half-distance

	string color;

	if (!this->t) {
		color = "black";
	} else {
		string hsv = "grey";

		if (l->best_schedule.has(*t)) {
			const float hue_step = 360.0 / this->n.channels().size();
			for (int i = 0; i < this->n.channels().size(); i++) {
				if (&this->n.channels()[i] == l->best_schedule.get(*this->t)) {
					char buf[10] = {0};
					sprintf(buf, "#%06x", this->hsv(i * hue_step, 1.0, 1.0));
					hsv = buf;
					//		debugf(hsv);
					break;
				}
			}
		}

		color = hsv;
	}

	if (!l->wrapped) {
		if (same_row) p1.second = p2.second = p1.second + (p1.first < p2.first ? -d : d);
		if (same_col) p1.first = p2.first = p1.first + (p1.second < p2.second ? -d : d);
		return this->arrow_straight(p1.first, p1.second, p2.first, p2.second, color);
	} else {
		if (same_row) p1.second = p2.second = p1.second + (p1.first < p2.first ? d : -d);
		if (same_col) p1.first = p2.first = p1.first + (p1.second < p2.second ? d : -d);
		return this->arrow_wrapped(p1.first, p1.second, p2.first, p2.second, color);
	}
}

/**
 * Draws an arrow head.
 * @param offset_angle The angle of the arrow head.
 * @return Returns a string representing the arrow head.
 */
string draw::arrow_head(const double offset_angle) {
	const double a = 25; // half-angle of head
	const double len = 3.0; // side-length of head

	const double angle1 = 180 - a + offset_angle;
	const double angle2 = angle1 + 2 * a;

	static double pi = std::atan(1.0)*4.0;
	static double deg2rad = pi / 180.0;

	double x1 = len * std::cos(angle1 * deg2rad);
	double y1 = len * std::sin(angle1 * deg2rad);

	double x2 = len * std::cos(angle2 * deg2rad);
	double y2 = len * std::sin(angle2 * deg2rad);

	return boost::str(boost::format(
		"l %1%,%2% "
		"l %3%,%4% "
		"l %5%,%6% "
		) % x1 % y1 // 1 and 2
		% (x2 - x1) % (y2 - y1)
		% -x2 % -y2
		);
}

element draw::arrow_straight(const float x1, const float y1, const float x2, const float y2, const string color) {
	static const float pi = std::atan(1.0)*4.0;
	static const float rad2deg = 180.0 / pi;

	const float angle = std::atan2(y2 - y1, x2 - x1) * rad2deg;

	const float dx = x2 - x1;
	const float dy = y2 - y1;
	const float length = std::sqrt(dx * dx + dy * dy);

	/* This arrow is made up of a path and a transformation.
	 * The path is always horizontal, starts in 0,0 and goes some length L to L,0.
	 * This is then transformed to correspond to the desired starting and end point.
	 * In SVG, uppercase letters denote absolute coordinates; lowercase is relative.
	 */
	element g = element().Tag("g")
		.Set("transform", "translate(" + ::lex_cast<string > (x1) + "," + ::lex_cast<string > (y1) + ") rotate(" + ::lex_cast<string > (angle) + ")")
		.AddChild(element().Tag("path")
		.Set("style", "fill")
		.Set("stroke", color)
		.Set("stroke-linejoin", "round")
		.Set("stroke-linecap", "round")
		.Set("d",
		boost::str(boost::format(
		"M 0,0 " // always origo
		"L %1%,0 " // length of the arrow
		"%2% "
		) % length % this->arrow_head(0))
		));

	return g;
}

element draw::arrow_wrapped(const float x1, const float y1, const float x2, const float y2, const string color) {
	const int stub = 3; // the small straight line which touches the node
	const int r = 10; // "radius" of bezier curve (appoximately a circle)
	int w = 26; // width of curve

	const bool same_row = (y1 == y2);
	const bool same_col = (x1 == x2);
	assert(same_col ^ same_row && "custom positions not allowed, must be in either same row or col");

	const bool down = (same_col * (y1 < y2));
	const bool right = (same_row * (x1 < x2));
	if (down || right) w *= -1;


	const auto s1 = (same_row * ((x1 < x2) ? -stub : stub));
	const auto s2 = (same_col * ((y1 < y2) ? -stub : stub));

	const auto c1 = (same_row * ((x1 < x2) ? -r : r));
	const auto c2 = (same_col * ((y1 < y2) ? -r : r));
	const auto c3 = (same_row * ((x1 < x2) ? -r : r) | same_col * (-w));
	const auto c4 = (same_col * ((y1 < y2) ? -r : r) | same_row * (-w));
	const auto c5 = (same_col * (-w));
	const auto c6 = (same_row * (-w));


	return element().Tag("path")
		.Set("style", "fill")
		.Set("stroke", color)
		.Set("stroke-linejoin", "round")
		.Set("stroke-linecap", "round")
		.Set("d",
		boost::str(boost::format(
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
		) % x1 // 1
		% y1 // 2

		% s1 // 3
		% s2 // 4

		% c1 // 5
		% c2 // 6
		% c3 // 7
		% c4 // 8
		% c5 // 9
		% c6 // 10

		% (x2 - x1 - 2 * s1) // 11
		% (y2 - y1 - 2 * s2) // 12

		% -c1 // 13
		% -c2 // 14
		% -c3 // 15
		% -c4 // 16
		% -c5 // 17
		% -c6 // 18

		% this->arrow_head(same_row * 180 * (x2 > x1) | -same_col * (90 + 180 * (y2 < y1))) // 19

		% -s1 // 20
		% -s2 // 21

		% (c1 * (same_col ? 1 : -1)) // 22
		% (c2 * (same_row ? 1 : -1)) // 23
		% (c3 * (same_col ? 1 : -1)) // 24
		% (c4 * (same_row ? 1 : -1)) // 25
		% (c5 * (same_col ? 1 : -1)) // 26
		% (c6 * (same_row ? 1 : -1)) // 27

		% -(x2 - x1 - 2 * s1) // 28
		% -(y2 - y1 - 2 * s2) // 29

		% -(c1 * (same_col ? 1 : -1)) // 30
		% -(c2 * (same_row ? 1 : -1)) // 31
		% -(c3 * (same_col ? 1 : -1)) // 32
		% -(c4 * (same_row ? 1 : -1)) // 33
		% -(c5 * (same_col ? 1 : -1)) // 34
		% -(c6 * (same_row ? 1 : -1)) // 35
		));
}

uint32_t draw::hsv(float h, float s, float v) {

	/*
	 * Kilde:
	 * http://en.wikipedia.org/w/index.php?title=HSL_and_HSV&oldid=282264028#Conversion_from_HSV_to_RGB
	 */

	struct colour {
		float r, g, b;

		colour() {
			r = g = b = 1.0;
		} // Default til hvid
	} c;

	float f = h / 60;
	int hi = int(floor(f)) % 6;
	f -= floor(f);

	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch (hi) {
		case 0: c.r = v;
			c.g = t;
			c.b = p;
			break;
		case 1: c.r = q;
			c.g = v;
			c.b = p;
			break;
		case 2: c.r = p;
			c.g = v;
			c.b = t;
			break;
		case 3: c.r = p;
			c.g = q;
			c.b = v;
			break;
		case 4: c.r = t;
			c.g = p;
			c.b = v;
			break;
		case 5: c.r = v;
			c.g = p;
			c.b = q;
			break;
	}

	c.r *= 255; // Gå fra [0; 1] til [0; 255]
	c.g *= 255; // ...
	c.b *= 255;

	// Pack farve komponenterne sammen til én værdi
	return (uint32_t(c.b) << 16) | (uint32_t(c.g) << 8) | uint32_t(c.r);
}

}