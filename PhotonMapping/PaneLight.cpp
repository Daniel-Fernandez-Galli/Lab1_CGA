#include "PaneLight.h"
#include "utils.h"
using namespace std;


PaneLight::PaneLight(float potencia, Color color, Vector3 center, float width, float height, Vector3 up, Vector3 right, Vector3 norm) : Light(potencia, color), center(center), width(width), height(height), up(up), right(right), norm(norm){}

Photon PaneLight::createPhoton(float potencia) {
	float width = getRandomFloat(- this->width / 2, this->width / 2);
	float heigh = getRandomFloat(- this->height / 2, this->height / 2);
	Vector3 center = this->up * heigh + this->right * width + this->center;
	//Vector3 dir = math::chooseAPointCosineDistribution(this->norm);
	Vector3 dir = math::chooseARandomPointFromASphere();
	return Photon(center, dir, Color(this->color.r * potencia, this->color.g * potencia, this->color.b * potencia));
}