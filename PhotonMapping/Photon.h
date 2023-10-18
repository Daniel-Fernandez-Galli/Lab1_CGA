#ifndef PHOTON_H
#define PHOTON_H

#include "utils.h"

using namespace math;

class Photon
{
public:

	Vector3 position;

	Vector3 direction;

	Color power;

	bool hasSpecularReflection;

	Photon(Vector3 pos, Vector3 dir, Color col, bool hasSpecularReflection);

	Vector3 get_position() const;

	Vector3 get_direction() const;

	Color get_power() const;

};

#endif

