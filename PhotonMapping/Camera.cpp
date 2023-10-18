#include "Camera.h"
#include "utils.h"

#include <cmath>

using namespace math;

Matrix<4, 4> default_cam_to_world = identity<4>();
Vector3 default_eye_point(0.0f, 0.0f, 0.0f);
constexpr float default_yfov = pi * 0.5f;
constexpr float default_znear = 0.001f;
constexpr float default_zfar = 1000.0f;
constexpr float default_camera_speed = 0.5f;

Camera::Camera(SDL_Window* window) :
	window(window),
	cam_to_world(default_cam_to_world),
	world_to_cam(default_cam_to_world),
	eye_point(default_eye_point),
	yfov(default_yfov), znear(default_znear), zfar(default_zfar)
{
	SDL_GetWindowSize(window, &width, &height);
	aspect_ratio = static_cast<float>(width) / height;
	invert_matrix(world_to_cam);
}

Camera::Camera(SDL_Window* window, CamConstructorData data) :
	window(window),
	cam_to_world(default_cam_to_world),
	yfov(data.yfov), znear(default_znear), zfar(default_zfar)
{
	SDL_GetWindowSize(window, &width, &height);
	aspect_ratio = static_cast<float>(width) / height;

	Matrix<4,4> rot = quaternion_to_rotation_matrix(data.eye_rotation);
	Matrix <4, 4> tras = identity<4>();
	tras[0][3] = data.eye_translation.x;
	tras[1][3] = data.eye_translation.y;
	tras[2][3] = data.eye_translation.z;
	eye_point = data.eye_translation;
	cam_to_world = tras * cam_to_world;
	world_to_cam = cam_to_world;
	invert_matrix(world_to_cam);
}

RTCRayHit Camera::ray_to_pixel(int px, int py) const {

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
		m[2][3] = -default_camera_speed;
		break;
	case Direction::BACKWARD:
		m[2][3] = default_camera_speed;
		break;
	case Direction::LEFT:
		m[0][3] = -default_camera_speed;
		break;
	case Direction::RIGHT:
		m[0][3] = default_camera_speed;
		break;
	case Direction::UP:
		m[1][3] = default_camera_speed;
		break;
	case Direction::DOWN:
		m[1][3] = -default_camera_speed;
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
	world_to_cam = cam_to_world;
	invert_matrix(world_to_cam);
	eye_point.x = cam_to_world[0][3];
	eye_point.y = cam_to_world[1][3];
	eye_point.z = cam_to_world[2][3];
}

math::Vector2 Camera::to_raster_space(const math::Vector3& p) const
{
	//auto rot = world_to_cam;
	//rot[0][3] = rot[1][3] = rot[2][3] = 0.0f;

	//Matrix<4, 1> local_z_axis_matrix;
	//local_z_axis_matrix[2][0] = 1.0f;
	//local_z_axis_matrix[3][0] = 1.0f;

	//local_z_axis_matrix = rot * local_z_axis_matrix;

	//Vector3 local_z_axis(local_z_axis_matrix[0][0], local_z_axis_matrix[1][0], local_z_axis_matrix[2][0]);
	Vector3 local_z_axis(cam_to_world[0][2], cam_to_world[1][2], cam_to_world[2][2]);
	local_z_axis = local_z_axis;

	Matrix<4, 1> p_world_matrix;
	p_world_matrix[0][0] = p.x;
	p_world_matrix[1][0] = p.y;
	p_world_matrix[2][0] = p.z;
	p_world_matrix[3][0] = 1.0f;

	Vector3 p_world(p_world_matrix[0][0], p_world_matrix[1][0], p_world_matrix[2][0]);

	Matrix<4,1> p_camera_matrix = world_to_cam * p_world_matrix;

	Vector3 p_camera(p_camera_matrix[0][0], p_camera_matrix[1][0], p_camera_matrix[2][0]);
	p_camera = p_camera / p_camera_matrix[3][0];

	if (dot_product(local_z_axis, p_world - eye_point) > 0.0f) {
		return Vector2(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
	}

	Vector2 p_screen;
	p_screen.x = p_camera.x / -(p_camera.z);
	p_screen.y = p_camera.y / -(p_camera.z);

	Vector2 p_ndc;
	float canvas = 2 * tan(yfov * 0.5);
	p_ndc.x = (p_screen.x + 0.5f * canvas)/canvas;
	p_ndc.y = (p_screen.y + 0.5f * canvas)/canvas;

	Vector2 p_raster;
	p_raster.x = std::floor(p_ndc.x * width);
	p_raster.y = std::floor((1 - p_ndc.y) * height);
	return p_raster;
}

CamData Camera::get_cam_data() const
{
	return CamData(eye_point, aspect_ratio, width, height, yfov, znear, zfar);
}
