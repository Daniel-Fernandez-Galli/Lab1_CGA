#include "Photon.h"

Photon::Photon(Vector3 pos, Vector3 dir, Color power) : position(pos), direction(dir), power(power), hasSpecularReflection(false), isFirstHit(true){}

Vector3 Photon::get_position() const
{
	return position;
}

Vector3 Photon::get_direction() const
{
	return direction;
}

Color Photon::get_power() const
{
	return power;
}