#include "Tools/functionHelpers.h"

namespace lve
{
	float Lerp(const float _a, const float _b, float _t)
	{
		return _a * (1.0f - _t) + _b * _t;
	}

	glm::vec2 Lerp(const glm::vec2& _a, const glm::vec2& _b, float _t)
	{
		return _a * (1.0f - _t) + _b * _t;
	}

	glm::vec3 Lerp(const glm::vec3& _a, const glm::vec3& _b, float _t)
	{
		return _a * (1.0f - _t) + _b * _t;
	}

	glm::vec4 Lerp(const glm::vec4& _a, const glm::vec4& _b, float _t)
	{
		return _a * (1.0f - _t) + _b * _t;
	}
} // namespace lve
