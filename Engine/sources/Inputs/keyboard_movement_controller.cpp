#include "Inputs/keyboard_movement_controller.h"

#include <gtc/constants.hpp>

#include "GameObject/Components/Transform.h"

class GameObject;

namespace lve
{
	void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow*    _window, float _deltaTime,
	                                               GameObject& _gameObject) const
	{
		glm::vec3 rotate{0.f};
		if (glfwGetKey(_window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(_window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(_window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(_window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (dot(rotate, rotate) > std::numeric_limits<
			    float>::epsilon())
			_gameObject.GetTransform()->SetRotation(_gameObject.GetTransform()->GetRotation() + (lookSpeed * _deltaTime * normalize(rotate)));
		_gameObject.GetTransform()->SetRotation({ glm::clamp(_gameObject.GetRotation().x, -1.5f, 1.5f), _gameObject.GetTransform()->GetRotation().y, _gameObject.GetTransform()->GetRotation().z });
		_gameObject.GetTransform()->SetRotation({_gameObject.GetRotation().x, glm::mod(_gameObject.GetRotation().y, glm::two_pi<float>()), _gameObject.GetTransform()->GetRotation().z}) ;

		float               yaw = _gameObject.GetRotation().y;
		const glm::vec3     forward_dir{sin(yaw), 0.f, cos(yaw)};
		const glm::vec3     right_dir{forward_dir.z, 0.f, -forward_dir.x};
		constexpr glm::vec3 up_dir{0.f, -1.f, 0.f};

		glm::vec3 move_dir{0.f};
		if (glfwGetKey(_window, keys.moveForward) == GLFW_PRESS) move_dir += forward_dir;
		if (glfwGetKey(_window, keys.moveBackward) == GLFW_PRESS) move_dir -= forward_dir;
		if (glfwGetKey(_window, keys.moveRight) == GLFW_PRESS) move_dir += right_dir;
		if (glfwGetKey(_window, keys.moveLeft) == GLFW_PRESS) move_dir -= right_dir;
		if (glfwGetKey(_window, keys.moveUp) == GLFW_PRESS) move_dir += up_dir;
		if (glfwGetKey(_window, keys.moveDown) == GLFW_PRESS) move_dir -= up_dir;

		if (dot(move_dir, move_dir) > std::numeric_limits<
			    float>::epsilon())
			_gameObject.GetTransform()->SetPosition(_gameObject.GetTransform()->GetPosition() + (moveSpeed * _deltaTime * normalize(move_dir)));
	}
} // namespace lve
