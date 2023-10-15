#include "Camera.h"
#include "utils.h"

#include <cmath>

using namespace math;

#define DEFAULT_CAMTOWORLD identity<4>()
#define DEFAULT_EYE_POINT Vector3(0.0f, 0.0f, 0.0f)
#define DEFAULT_YFOV pi/2.0f
#define DEFAULT_ZNEAR 0.001f
#define DEFAULT_ZFAR 1000.0f
#define CAMERA_SPEED 0.1f

Camera::Camera(SDL_Window* window) :
	window(window),
	cam_to_world(DEFAULT_CAMTOWORLD),
	eye_point(DEFAULT_EYE_POINT),
	yfov(DEFAULT_YFOV), znear(DEFAULT_ZNEAR), zfar(DEFAULT_ZFAR)
{
	SDL_GetWindowSize(window, &width, &height);
	aspect_ratio = static_cast<float>(width) / height;
}

RTCRayHit Camera::ray_to_pixel(int px, int py) {

	Matrix<4, 1> dir{};
	dir[0][0] = px - (width * 0.5f);
	dir[1][0] = (height * 0.5f) - py;
	dir[2][0] = -(height * 0.5f) / (tan(yfov * 0.5f));
	dir[3][0] = 1.0f;

	dir = cam_to_world * dir;

	float dirx = dir[0][0];
	float diry = dir[1][0];
	float dirz = dir[2][0];

	struct RTCRayHit rayhit {};
	rayhit.ray.org_x = eye_point.x;
	rayhit.ray.org_y = eye_point.y;
	rayhit.ray.org_z = eye_point.z;
	rayhit.ray.dir_x = dirx;
	rayhit.ray.dir_y = diry;
	rayhit.ray.dir_z = dirz;
	rayhit.ray.tnear = znear;
	rayhit.ray.tfar = zfar;
	rayhit.ray.mask = -1;
	rayhit.ray.flags = 0;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

	return rayhit;
}

void Camera::move(Direction dir) {
	Matrix <4, 4> m = identity<4>();
	switch (dir) {
	case Direction::FORWARD:
		m[2][3] = -CAMERA_SPEED;
		break;
	case Direction::BACKWARD:
		m[2][3] = CAMERA_SPEED;
		break;
	case Direction::LEFT:
		m[0][3] = -CAMERA_SPEED;
		break;
	case Direction::RIGHT:
		m[0][3] = CAMERA_SPEED;
		break;
	case Direction::UP:
		m[1][3] = CAMERA_SPEED;
		break;
	case Direction::DOWN:
		m[1][3] = -CAMERA_SPEED;
		break;

	}

	transform(m, true, true);
}

void Camera::transform(const math::Matrix<4, 4>& transform, bool local_origin, bool local_axis)
{
	if (local_origin) {

		if (local_axis) {
			cam_to_world = cam_to_world * transform;
		}
		else {
			// reset position
			cam_to_world[0][3] = cam_to_world[1][3] = cam_to_world[2][3] = 0.0f;

			cam_to_world = transform * cam_to_world;

			//restore position
			cam_to_world[0][3] = eye_point.x;
			cam_to_world[1][3] = eye_point.y;
			cam_to_world[2][3] = eye_point.z;
		}

	}
	else {

		if (local_axis) {

			auto tras = identity<4>();
			tras[0][3] = eye_point.x;
			tras[1][3] = eye_point.y;
			tras[2][3] = eye_point.z;

			cam_to_world = cam_to_world * (transform * tras);
		}
		else {
			cam_to_world = transform * cam_to_world;
		}
	}

	eye_point.x = cam_to_world[0][3];
	eye_point.y = cam_to_world[1][3];
	eye_point.z = cam_to_world[2][3];
}
