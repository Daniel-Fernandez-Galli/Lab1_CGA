#ifndef PHOTON_H
#define PHOTON_H

#include "utils.h"

using namespace math;

class Photon
{
private:

	Vector position;

	Vector direction;

	Color color;

public:

	Photon(Vector pos, Vector dir, Color col);

	Vector get_position() const;

	Vector get_direction() const;

	Color get_color() const;

};

#endif

