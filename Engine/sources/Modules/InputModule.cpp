#include "Modules/InputModule.h"
#include "Modules/ModuleManager.h"
#include <iostream>
#include <imgui.h>

InputModule::~InputModule()
{

}

//void InputModule::ProcessInput()
//{
//	// Reset mouse position
//	mouseX = 0.0;
//	mouseY = 0.0;
//
//	// Process joystick and gamepad input
//	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid)
//	{
//		if (glfwJoystickPresent(jid))
//		{
//			if (const float* axes = glfwGetJoystickAxes(jid, nullptr))
//				// Process joystick axes
//				for (int i = 0; i < 6; ++i)
//				{
//					if (axes[i] != 0.0)
//					{
//						// Example: process joystick axis input
//					}
//				}
//
//			if (const unsigned char* buttons = glfwGetJoystickButtons(jid, nullptr))
//				// Process joystick buttons
//				for (int i = 0; i < 16; ++i)
//				{
//					if (buttons[i] == GLFW_PRESS)
//					{
//						// Example: process joystick button input
//					}
//				}
//
//			if (const unsigned char* hats = glfwGetJoystickHats(jid, nullptr))
//				// Process joystick hats
//				for (int i = 0; i < 4; ++i)
//				{
//					if (hats[i] != GLFW_HAT_CENTERED)
//					{
//						// Example: process joystick hat input
//					}
//				}
//		}
//	}
//
//	glfwPollEvents();
//}
//
//
//bool InputModule::IsKeyPressed(const int _key)
//{
//	return false;//keys[_key];
//}
//
//bool InputModule::IsMouseButtonPressed(const int _button)
//{
//	return mouseButtons[_button];
//}
//
//void InputModule::GetMousePosition(double& _xPos, double& _yPos) const
//{
//	_xPos = mouseX;
//	_yPos = mouseY;
//}
//
//void InputModule::KeyCallback(GLFWwindow* _window, const int _key, int _scancode, const int _action, int _mods)
//{
//	const auto input_module = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));
//
//	if (_action == GLFW_PRESS) input_module->keys[_key] = true;
//	else if (_action == GLFW_RELEASE) input_module->keys[_key] = false;
//}
//
//void InputModule::MouseButtonCallback(GLFWwindow* _window, const int _button, const int _action, int _mods)
//{
//	const auto input_module = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));
//
//	if (_action == GLFW_PRESS) input_module->mouseButtons[_button] = true;
//	else if (_action == GLFW_RELEASE) input_module->mouseButtons[_button] = false;
//}
//
//void InputModule::CursorPositionCallback(GLFWwindow* _window, const double _xPos, const double _yPos)
//{
//	const auto input_module = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));
//
//	input_module->mouseX = _xPos;
//	input_module->mouseY = _yPos;
//}
//
//void InputModule::JoystickCallback(const int _jid, const int _event)
//{
//	if (_event == GLFW_CONNECTED) std::cout << "Joystick connected: " << _jid << std::endl;
//	else if (_event == GLFW_DISCONNECTED) std::cout << "Joystick disconnected: " << _jid << std::endl;
//}
//
//void InputModule::GamepadInput(int _jid, const GLFWgamepadstate* _state)
//{
//	// Example: Process gamepad input
//	if (_state->buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS)
//	{
//		// Button A pressed
//	}
//
//	// Example: Use gamepad axes
//	float left_stick_x = _state->axes[GLFW_GAMEPAD_AXIS_LEFT_X];
//	float left_stick_y = _state->axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
//}

void InputModule::Init()
{
	Module::Init();

	mouseX = 0.0;
	mouseY = 0.0;

	windowModule = moduleManager->GetModule<WindowModule>();

	//glfwSetMouseButtonCallback(window, MouseButtonCallback);
	//glfwSetCursorPosCallback(window, CursorPositionCallback);
	//glfwSetJoystickCallback(JoystickCallback);

	ClearAllInputs();
}

void InputModule::Start()
{
	Module::Start();
}

void InputModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void InputModule::Update()
{
	Module::Update();

	//ProcessInput();
	glfwPollEvents();

	if (!windowModule->HasFocus())
	{
		//ClearAllInputs();
		//return;
	}

	// Keyboard keys
	for (auto& keyPair : keys)
	{
		if (keyPair.second.down > 0)
		{
			++keyPair.second.down;
		}
	}

	for (uint32_t i = 0; i < MOUSE_BUTTON_COUNT; ++i)
	{
		m_MouseButtonsPressed &= ~(1 << i);
		m_MouseButtonsReleased &= ~(1 << i);
		if (m_MouseButtonStates & (1 << i))
		{
			m_MouseButtonDrags[i].endLocation = m_MousePosition;
		}
	}
}

void InputModule::PreRender()
{
	Module::PreRender();
}

void InputModule::Render()
{
	Module::Render();
}

void InputModule::RenderGui()
{
	Module::RenderGui();
}

void InputModule::PostRender()
{
	Module::PostRender();
}

void InputModule::Release()
{
	Module::Release();
}

void InputModule::Finalize()
{
	Module::Finalize();
}

int32_t InputModule::GetKeyDown(Inputs::KeyCode keyCode, bool bIgnoreImGui /* = false */) const
{
	if (!bIgnoreImGui && ImGui::GetIO().WantCaptureKeyboard)
	{
		return 0;
	}

	auto iter = keys.find(keyCode);
	if (iter != keys.end())
	{
		return keys.at(keyCode).down;
	}

	return 0;
}

bool InputModule::GetKeyPressed(Inputs::KeyCode keyCode, bool bIgnoreImGui /* = false */) const
{
	return GetKeyDown(keyCode, bIgnoreImGui) == 1;
}

bool InputModule::GetKeyReleased(Inputs::KeyCode keyCode, bool bIgnoreImGui /* = false */) const
{
	if (!bIgnoreImGui && ImGui::GetIO().WantCaptureKeyboard)
	{
		return false;
	}

	auto iter = keys.find(keyCode);
	if (iter != keys.end())
	{
		const Inputs::Key& key = iter->second;
		return key.down == 0 && key.pDown > 0;
	}

	return false;
}

void InputModule::ClearAllInputs()
{
	ClearMouseInput();
	ClearKeyboadInput();
}

void InputModule::ClearMouseInput()
{
	m_PrevMousePosition = m_MousePosition = glm::vec2(-1.0f);
	m_ScrollXOffset = 0.0f;
	m_ScrollYOffset = 0.0f;

	m_MouseButtonStates = 0;
	m_MouseButtonsPressed = 0;
	m_MouseButtonsReleased = 0;

	for (Inputs::MouseDrag& mouseDrag : m_MouseButtonDrags)
	{
		mouseDrag.startLocation = glm::vec2{ 0, 0 };
		mouseDrag.endLocation = glm::vec2{ 0, 0 };
	}

	windowModule->SetCursorMode(GlfwCursorMode::NORMAL);

	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		io.MouseWheel = m_ScrollYOffset;
	}
}

void InputModule::ClearKeyboadInput()
{
	for (auto& keyPair : keys)
	{
		keyPair.second.pDown = 0;
		keyPair.second.down = 0;
	}
}

glm::vec2 InputModule::GetMousePosition() const
{
	return m_MousePosition;
}

void InputModule::MouseButtonCallback(Inputs::MouseButton mouseButton, Inputs::KeyAction keyAction, int32_t mods)
{
	if (keyAction == Inputs::KeyAction::KEY_PRESS)
	{
		if (m_MousePosition.x == -1.0f)
		{
			m_MousePosition = windowModule->GetMousePosition();
		}

		m_MouseButtonStates |= (1 << (uint32_t)mouseButton);
		m_MouseButtonsPressed |= (1 << (uint32_t)mouseButton);
		m_MouseButtonsReleased &= ~(1 << (uint32_t)mouseButton);

		m_MouseButtonDrags[(uint32_t)mouseButton].startLocation = m_MousePosition;
		m_MouseButtonDrags[(uint32_t)mouseButton].endLocation = m_MousePosition;
	}
	else if (keyAction == Inputs::KeyAction::KEY_RELEASE)
	{
		m_MouseButtonStates &= ~(1 << (uint32_t)mouseButton);
		m_MouseButtonsPressed &= ~(1 << (uint32_t)mouseButton);
		m_MouseButtonsReleased |= (1 << (uint32_t)mouseButton);


		if (m_MousePosition.x == -1.0f)
		{
			m_MousePosition = windowModule->GetMousePosition();
		}

		m_MouseButtonDrags[(uint32_t)mouseButton].endLocation = m_MousePosition;
	}
}

void InputModule::KeyCallback(Inputs::KeyCode _keyCode, Inputs::KeyAction _keyAction, int32_t _mods)
{
	//FLEX_UNUSED(mods);

	keys[_keyCode].pDown = keys[_keyCode].down;

	if (_keyAction == Inputs::KeyAction::KEY_PRESS)
	{
		keys[_keyCode].down = 1;
		std::cout << "Pressed key: ";
	}
	else if (_keyAction == Inputs::KeyAction::KEY_REPEAT)
	{
		// Ignore repeat events (we're already counting how long the key is down for
		std::cout << "Holding key: ";
	}
	else if (_keyAction == Inputs::KeyAction::KEY_RELEASE)
	{
		keys[_keyCode].down = 0;
		std::cout << "Released key: ";
	}

	std::cout << Inputs::KeyCodeStrings[static_cast<int>(_keyCode)] << std::endl;

	//ImGuiIO& io = ImGui::GetIO();
	//io.KeysDown[(int32_t)_keyCode] = keys[_keyCode].down > 0;

	//const bool bCtrlDown = GetKeyDown(Inputs::KeyCode::KEY_LEFT_CONTROL, true) || GetKeyDown(Inputs::KeyCode::KEY_RIGHT_CONTROL, true);
	//const bool bShiftDown = GetKeyDown(Inputs::KeyCode::KEY_LEFT_SHIFT, true) || GetKeyDown(Inputs::KeyCode::KEY_RIGHT_SHIFT, true);
	//const bool bAltDown = GetKeyDown(Inputs::KeyCode::KEY_LEFT_ALT, true) || GetKeyDown(Inputs::KeyCode::KEY_RIGHT_ALT, true);
	//const bool bSuperDown = GetKeyDown(Inputs::KeyCode::KEY_LEFT_SUPER, true) || GetKeyDown(Inputs::KeyCode::KEY_RIGHT_SUPER, true);
	//const bool bModiferDown = (bCtrlDown || bShiftDown || bAltDown || bSuperDown);

	//io.KeyCtrl = bCtrlDown;
	//io.KeyShift = bShiftDown;
	//io.KeyAlt = bAltDown;
	//io.KeySuper = bSuperDown;

	//if (!io.WantCaptureKeyboard)
	//{
	//	// Call callbacks present in m_ActionCallbacks & m_KeyEventCallbacks according to priorities
	//	auto actionIter = m_ActionCallbacks.end();
	//	auto keyEventIter = m_KeyEventCallbacks.begin();
	//	Action keyPressAction = Action::_NONE;
	//	ActionEvent actionEvent = keyAction == KeyAction::KEY_PRESS ? ActionEvent::ACTION_TRIGGER : ActionEvent::ACTION_RELEASE;

	//	// TODO: Allow modifiers to be down once supported properly
	//	if ((keyAction == KeyAction::KEY_PRESS || keyAction == KeyAction::KEY_RELEASE) && !bModiferDown)
	//	{
	//		keyPressAction = GetActionFromKeyCode(keyCode);
	//		if (keyPressAction != Action::_NONE)
	//		{
	//			actionIter = m_ActionCallbacks.begin();
	//		}
	//	}

	//	bool bEventsInQueue = (actionIter != m_ActionCallbacks.end()) ||
	//		(keyEventIter != m_KeyEventCallbacks.end());
	//	while (bEventsInQueue)
	//	{
	//		if (actionIter == m_ActionCallbacks.end())
	//		{
	//			if (keyEventIter->first->Execute(keyCode, keyAction, mods) == EventReply::CONSUMED)
	//			{
	//				break;
	//			}
	//			++keyEventIter;
	//		}
	//		else if (keyEventIter == m_KeyEventCallbacks.end())
	//		{
	//			if (actionIter->first->Execute(keyPressAction, actionEvent) == EventReply::CONSUMED)
	//			{
	//				break;
	//			}
	//			++actionIter;
	//		}
	//		else
	//		{
	//			if (actionIter->second >= keyEventIter->second)
	//			{
	//				if (actionIter->first->Execute(keyPressAction, actionEvent) == EventReply::CONSUMED)
	//				{
	//					break;
	//				}
	//				++actionIter;
	//			}
	//			else
	//			{
	//				if (keyEventIter->first->Execute(keyCode, keyAction, mods) == EventReply::CONSUMED)
	//				{
	//					break;
	//				}
	//				++keyEventIter;
	//			}
	//		}

	//		bEventsInQueue = (actionIter != m_ActionCallbacks.end()) || (keyEventIter != m_KeyEventCallbacks.end());
	//	}
	//}
}

void InputModule::CursorPosCallback(double _x, double _y)
{
	m_MousePosition = glm::vec2((double)_x, (double)_y);

#if 0
	{
		glm::vec2 pos = g_Window->GetMousePosition();
		Print("%.2f, %.2f (%.2f, %.2f)\n", x, y, pos.x, pos.y);
	}
#endif
}