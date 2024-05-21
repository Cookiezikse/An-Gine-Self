#include "Camera/lve_camera.h"

//std
#include <cassert>
#include <limits>

namespace lve
{
	void LveCamera::SetOrthographicProjection(
		const float _left, const float _right, const float _top, const float _bottom, const float _near,
		const float _far)
	{
		projectionMatrix       = glm::mat4{1.0f};
		projectionMatrix[0][0] = 2.f / (_right - _left);
		projectionMatrix[1][1] = 2.f / (_bottom - _top);
		projectionMatrix[2][2] = 1.f / (_far - _near);
		projectionMatrix[3][0] = -(_right + _left) / (_right - _left);
		projectionMatrix[3][1] = -(_bottom + _top) / (_bottom - _top);
		projectionMatrix[3][2] = -_near / (_far - _near);
	}

	void LveCamera::SetPerspectiveProjection(const float _fovY, const float _aspect, const float _near,
	                                         const float _far)
	{
		assert(glm::abs(_aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tan_half_fov_y = tan(_fovY / 2.f);
		projectionMatrix           = glm::mat4{0.0f};
		projectionMatrix[0][0]     = 1.f / (_aspect * tan_half_fov_y);
		projectionMatrix[1][1]     = 1.f / (tan_half_fov_y);
		projectionMatrix[2][2]     = _far / (_far - _near);
		projectionMatrix[2][3]     = 1.f;
		projectionMatrix[3][2]     = -(_far * _near) / (_far - _near);
	}

	void LveCamera::SetViewDirection(const glm::vec3 _position, const glm::vec3 _direction, const glm::vec3 _up)
	{
		const glm::vec3 w{normalize(_direction)};
		const glm::vec3 u{normalize(cross(w, _up))};
		const glm::vec3 v{cross(w, u)};

		viewMatrix       = glm::mat4{1.f};
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -dot(u, _position);
		viewMatrix[3][1] = -dot(v, _position);
		viewMatrix[3][2] = -dot(w, _position);

		inverseViewMatrix       = glm::mat4{1.f};
		inverseViewMatrix[0][0] = u.x;
		inverseViewMatrix[0][1] = u.y;
		inverseViewMatrix[0][2] = u.z;
		inverseViewMatrix[1][0] = v.x;
		inverseViewMatrix[1][1] = v.y;
		inverseViewMatrix[1][2] = v.z;
		inverseViewMatrix[2][0] = w.x;
		inverseViewMatrix[2][1] = w.y;
		inverseViewMatrix[2][2] = w.z;
		inverseViewMatrix[3][0] = _position.x;
		inverseViewMatrix[3][1] = _position.y;
		inverseViewMatrix[3][2] = _position.z;
	}

	void LveCamera::SetViewTarget(const glm::vec3 _position, const glm::vec3 _target, const glm::vec3 _up)
	{
		SetViewDirection(_position, _target - _position, _up);
	}

	void LveCamera::SetViewYXZ(const glm::vec3 _position, const glm::vec3 _rotation)
	{
		const float     c3 = glm::cos(_rotation.z);
		const float     s3 = glm::sin(_rotation.z);
		const float     c2 = glm::cos(_rotation.x);
		const float     s2 = glm::sin(_rotation.x);
		const float     c1 = glm::cos(_rotation.y);
		const float     s1 = glm::sin(_rotation.y);
		const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
		const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
		const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
		viewMatrix       = glm::mat4{1.f};
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -dot(u, _position);
		viewMatrix[3][1] = -dot(v, _position);
		viewMatrix[3][2] = -dot(w, _position);

		inverseViewMatrix       = glm::mat4{1.f};
		inverseViewMatrix[0][0] = u.x;
		inverseViewMatrix[0][1] = u.y;
		inverseViewMatrix[0][2] = u.z;
		inverseViewMatrix[1][0] = v.x;
		inverseViewMatrix[1][1] = v.y;
		inverseViewMatrix[1][2] = v.z;
		inverseViewMatrix[2][0] = w.x;
		inverseViewMatrix[2][1] = w.y;
		inverseViewMatrix[2][2] = w.z;
		inverseViewMatrix[3][0] = _position.x;
		inverseViewMatrix[3][1] = _position.y;
		inverseViewMatrix[3][2] = _position.z;
	}
}
