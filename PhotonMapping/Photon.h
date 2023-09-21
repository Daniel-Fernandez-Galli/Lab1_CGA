#ifndef PHOTON_H
#define PHOTON_H

#include "utils.h"

using namespace math;

class Photon
{
private:

	Vector3 position;

	Vector3 direction;

	Color color;

public:

	Photon(Vector3 pos, Vector3 dir, Color col);

	Vector3 get_position() const;

	Vector3 get_direction() const;

	Color get_color() const;

};

#endif

