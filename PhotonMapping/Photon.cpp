#include "Photon.h"

Photon::Photon(Vector3 pos, Vector3 dir, Color col) : position(pos), direction(dir), color(col) {}

Vector3 Photon::get_position() const
{
	return position;
}

Vector3 Photon::get_direction() const
{
	return direction;
}

Color Photon::get_color() const
{
	return color;
}