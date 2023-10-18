#include "PointLight.h"
#include "utils.h"
using namespace std;

PointLight::PointLight(Vector3 center, float potencia, Color color) : Light(potencia, color), center(center){}

Photon PointLight::createPhoton(float potencia) {
	Vector3 dir = chooseARandomPointFromASphere();
	return Photon(this->center, dir, Color(this->color.r * potencia, this->color.g * potencia, this->color.b * potencia), false);
}