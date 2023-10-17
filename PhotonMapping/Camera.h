#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

#include <embree4/rtcore.h>
#include <SDL.h>

struct CamData {

	math::Vector3 eye_point;

	float aspect_ratio;

	int width, height;

	float yfov;

	float znear;

	float zfar;

};

class Camera
{
private:

	SDL_Window* window;

	math::Matrix<4, 4> cam_to_world;

	math::Matrix<4, 4> world_to_cam;

	math::Vector3 eye_point;

	float aspect_ratio;

	int width, height;

	float yfov; //radians, should be less than pi

	float znear;

	float zfar;

public:

	Camera(SDL_Window* window);

	RTCRayHit ray_to_pixel(int x, int y) const;

	void move(Direction dir);

	void transform(const math::Matrix<4, 4> &transform, bool local_origin = false, bool local_axis = false);

	math::Vector2 to_raster_space(const math::Vector3 &v) const;

	CamData get_cam_data() const;

};

#endif