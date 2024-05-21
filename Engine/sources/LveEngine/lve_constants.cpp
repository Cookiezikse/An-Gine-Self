#include "LveEngine/lve_constants.h"

// Librairies

// std
#include <string>


namespace lve
{
	const glm::vec3 VEC3_RIGHT         = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 VEC3_UP            = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 VEC3_FORWARD       = glm::vec3(0.0f, 0.0f, 1.0f);
	const glm::vec2 VEC2_ONE           = glm::vec2(1.0f);
	const glm::vec2 VEC2_NEG_ONE       = glm::vec2(-1.0f);
	const glm::vec2 VEC2_ZERO          = glm::vec2(0.0f);
	const glm::vec3 VEC3_ONE           = glm::vec3(1.0f);
	const glm::vec3 VEC3_NEG_ONE       = glm::vec3(-1.0f);
	const glm::vec3 VEC3_ZERO          = glm::vec3(0.0f);
	const glm::vec3 VEC3_GAMMA         = glm::vec3(2.2f);
	const glm::vec3 VEC3_GAMMA_INVERSE = glm::vec3(1.0f / 2.2f);
	const glm::vec4 VEC4_ONE           = glm::vec4(1.0f);
	const glm::vec4 VEC4_NEG_ONE       = glm::vec4(-1.0f);
	const glm::vec4 VEC4_ZERO          = glm::vec4(0.0f);
	const glm::vec4 VEC4_GAMMA         = glm::vec4(2.2f);
	const glm::vec4 VEC4_GAMMA_INVERSE = glm::vec4(1.0f / 2.2f);
	const glm::quat QUAT_IDENTITY      = glm::quat(VEC3_ZERO);
	const glm::mat2 MAT2_IDENTITY      = glm::mat2(1.0f);
	const glm::mat3 MAT3_IDENTITY      = glm::mat3(1.0f);
	const glm::mat4 MAT4_IDENTITY      = glm::mat4(1.0f);
	const glm::mat4 MAT4_ZERO          = glm::mat4(0.0f);
	const uint32_t  COLOUR32U_WHITE    = 0xFFFFFFFF;
	const uint32_t  COLOUR32U_BLACK    = 0x00000000;
	const glm::vec4 COLOUR128F_WHITE   = VEC4_ONE;
	const glm::vec4 COLOUR128F_BLACK   = VEC4_ZERO;

	const std::string EMPTY_STRING = std::string();

	const uint32_t MAX_TEXTURE_DIM = 65536;
} // namespace lve
