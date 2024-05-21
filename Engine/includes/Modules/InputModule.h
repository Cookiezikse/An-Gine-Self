#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <unordered_map>
#include <GLFW/glfw3.h>
#include <map>
#include <glm.hpp>

#include "Inputs/InputKeyCode.h"
#include "Module.h"
#include "Modules/WindowModule.h"

class InputModule final : public Module
{
public:
	InputModule() = default;
	~InputModule();

	void ProcessInput();

	bool IsKeyPressed(int _key);
	bool IsMouseButtonPressed(int _button);
	void GetMousePosition(double& _xPos, double& _yPos) const;

	int32_t GetKeyDown(Inputs::KeyCode keyCode, bool bIgnoreImGui = false) const;
	bool GetKeyPressed(Inputs::KeyCode keyCode, bool bIgnoreImGui = false) const;
	bool GetKeyReleased(Inputs::KeyCode keyCode, bool bIgnoreImGui = false) const;

	void ClearAllInputs();
	void ClearMouseInput();
	void ClearKeyboadInput();

	glm::vec2 GetMousePosition() const;

	void MouseButtonCallback(Inputs::MouseButton mouseButton, Inputs::KeyAction keyAction, int32_t mods);
	void KeyCallback(Inputs::KeyCode _keyCode, Inputs::KeyAction _keyAction, int32_t _mods);
	void CursorPosCallback(double _x, double _y);

	/**
		* @brief Initialise le module.
		*/
	void Init() override;

	/**
	 * @brief Démarre le module.
	 */
	void Start() override;

	/**
	 * @brief Effectue une mise à jour fixe du module.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met à jour le module.
	 */
	void Update() override;

	/**
	 * @brief Fonction pré-rendu du module.
	 */
	void PreRender() override;

	/**
	 * @brief Rendu du module.
	 */
	void Render() override;

	/**
	 * @brief Rendu de l'interface graphique du module.
	 */
	void RenderGui() override;

	/**
	 * @brief Fonction post-rendu du module.
	 */
	void PostRender() override;

	/**
	 * @brief Libère les ressources utilisées par le module.
	 */
	void Release() override;

	/**
	 * @brief Finalise le module.
	 */
	void Finalize() override;

private:
	WindowModule* windowModule = nullptr;
	std::unordered_map<Inputs::KeyCode, Inputs::Key> keys;
	std::unordered_map<int, bool> mouseButtons;
	double                        mouseX, mouseY;

	static const int32_t MOUSE_BUTTON_COUNT = (int32_t)Inputs::MouseButton::_NONE;

	uint32_t m_MouseButtonStates;
	uint32_t m_MouseButtonsPressed;
	uint32_t m_MouseButtonsReleased;
	Inputs::MouseDrag m_MouseButtonDrags[MOUSE_BUTTON_COUNT];
	glm::vec2 m_MousePosition = { 0, 0 };
	glm::vec2 m_PrevMousePosition = { 0, 0 };
	float m_ScrollXOffset = 0;
	float m_ScrollYOffset = 0;

	//static void KeyCallback(GLFWwindow* _window, int32_t _key, int32_t _scancode, int32_t _action, int32_t _mods);
	//static void MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods);
	//static void CursorPositionCallback(GLFWwindow* _window, double _xPos, double _yPos);
	//static void JoystickCallback(int _jid, int _event);
	//static void GamepadInput(int _jid, const GLFWgamepadstate* _state);
};

#endif // INPUT_MANAGER_H
