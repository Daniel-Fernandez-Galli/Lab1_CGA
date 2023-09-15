#include "Photon.h"

Photon::Photon(Vector pos, Vector dir, Color col) : position(pos), direction(dir), color(col) {}

Vector Photon::get_position() const
{
	return position;
}

Color Photon::get_color() const
{
	return color;
}

Vector Photon::get_direction() const
{
	return direction;
}
