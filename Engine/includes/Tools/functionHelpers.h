#pragma once

#include <iomanip>
#include <ios>
#include <sstream>

#include "LveEngine/lve_constants.h"

namespace lve
{
	FLEX_NO_DISCARD float     Lerp(float _a, float _b, float _t);
	FLEX_NO_DISCARD glm::vec2 Lerp(const glm::vec2& _a, const glm::vec2& _b, float _t);
	FLEX_NO_DISCARD glm::vec3 Lerp(const glm::vec3& _a, const glm::vec3& _b, float _t);
	FLEX_NO_DISCARD glm::vec4 Lerp(const glm::vec4& _a, const glm::vec4& _b, float _t);

	FLEX_NO_DISCARD inline float Saturate(const float _val)
	{
		return glm::clamp(_val, 0.0f, 1.0f);
	}

	template <typename T>
	FLEX_NO_DISCARD T Saturate(T _val)
	{
		return glm::clamp(_val, T(0), T(1));
	}

	inline std::string FloatToString(const float _f, const uint32_t _precision /* = DEFAULT_FLOAT_PRECISION */)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(_precision) << _f;
		return stream.str();
	}
}
