#include "Photon.h"

Photon::Photon(Vector3 pos, Vector3 dir, Color power, bool hasSpecularReflection) : position(pos), direction(dir), power(power), hasSpecularReflection(hasSpecularReflection){}

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