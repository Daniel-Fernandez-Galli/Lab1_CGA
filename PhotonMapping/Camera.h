#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

#include <embree4/rtcore.h>
#include <SDL.h>

class Camera
{
private:

	SDL_Window* window;

	math::Matrix<4, 4> cam_to_world;

	math::Vector3 eye_point;

	float aspect_ratio;

	int width, height;

	float yfov; //radians, should be less than pi

	float znear;

	float zfar;

public:

	Camera(SDL_Window* window);

	RTCRayHit ray_to_pixel(int x, int y);

	void move(Direction dir);

	void transform(const math::Matrix<4, 4> &transform, bool local_origin = false, bool local_axis = false);

};

#endif