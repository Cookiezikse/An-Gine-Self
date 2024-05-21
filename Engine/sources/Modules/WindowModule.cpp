#include "Modules/WindowModule.h"
#include "Engine/CoreEngine.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

// std
#include <cassert>
#include <chrono>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include "Modules/FileManager.h"
#include "Modules/RHIVulkanModule.h"
#include "Modules/TimeModule.h"
#include "Scene/SceneManager.h"
#include "Tools/functionHelpers.h"

using json = nlohmann::json;


WindowModule::WindowModule(): windowName("An-Gine"), window(nullptr), size(WIDTH, HEIGHT),
                              lastWindowedSize(glm::ivec2(0)), lastWindowedPos(glm::ivec2(0))
{
	lastNonFullscreenWindowMode = WindowMode::WINDOWED;
	updateWindowTitleFrequency  = 0.2f;
}

// Destructeur de la classe LveWindow
WindowModule::~WindowModule()
{
	// Détruit la fenêtre GLFW
	glfwDestroyWindow(window);
	// Termine GLFW
	glfwTerminate();
}

void WindowModule::CreateWindowSurface(const vk::Instance _instance, vk::SurfaceKHR* _surface) const
{
	if (glfwCreateWindowSurface(_instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(_surface)) !=
	    VK_SUCCESS)
		throw std::runtime_error("failed to create window surface");
}

void WindowModule::FrameBufferResizeCallBack(GLFWwindow* _window, const int _width, const int _height)
{
	const auto window_module          = static_cast<WindowModule*>(glfwGetWindowUserPointer(_window));
	window_module->bFrameBufferResize = true;
	window_module->SetSize(_width, _height);
}

void WindowModule::CubeCursorWindow(const int _color) const
{
	unsigned char pixels[16 * 16 * 4];
	memset(pixels, _color, sizeof(pixels));

	GLFWimage image;
	image.width  = 16;
	image.height = 16;
	image.pixels = pixels;

	GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
	glfwSetCursor(window, cursor);
}

void WindowModule::StandardCursorWindow(const GlfwCursorType _cursorType) const
{
	GLFWcursor* glfw_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

	switch (_cursorType)
	{
		case ARROW:
			glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
			break;
		case IBEAM:
			glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
			break;
		case CROSSHAIR:
			glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
			break;
		case POINTING_HAND:
			glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
			break;
		case RESIZE_EW:
			glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
			break;
		case RESIZE_NS:
			glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
			break;
		case RESIZE_NWSE:
			glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
			break;
		case RESIZE_NESW:
			glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
			break;
		case RESIZE_ALL:
			glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
			break;
		case NOT_ALLOWED:
			glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
			break;
	}
	glfwSetCursor(window, glfw_cursor);
}

void WindowModule::SetInputCursorMode(const GlfwCursorMode _mode)
{
	if (cursorMode != _mode)
	{
		SetCursorMode(_mode);

		glm::i32 glfw_cursor_mode = 0;

		switch (_mode)
		{
			case NORMAL:
				glfw_cursor_mode = GLFW_CURSOR_NORMAL;
				break;
			case HIDDEN:
				glfw_cursor_mode = GLFW_CURSOR_HIDDEN;
				break;
			case DISABLED:
				glfw_cursor_mode = GLFW_CURSOR_DISABLED;
				break;
			case CAPTURED:
				glfw_cursor_mode = GLFW_CURSOR_CAPTURED;
				break;
			case _NONE:
			default:
				std::cout << ("Unhandled cursor mode passed to GLFWWindowWrapper::SetCursorMode: %i\n", static_cast<
					              glm::i32>(_mode));
				break;
		}

		glfwSetInputMode(window, GLFW_CURSOR, glfw_cursor_mode);

		// Enable raw motion when cursor disabled for smoother camera controls
		if (glfw_cursor_mode == GLFW_CURSOR_DISABLED) if (glfwRawMouseMotionSupported()) glfwSetInputMode(
			window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}

Inputs::KeyAction WindowModule::GLFWActionToInputManagerAction(int32_t _glfwAction)
{
	Inputs::KeyAction inputAction = Inputs::KeyAction::_NONE;

	switch (_glfwAction)
	{
	case GLFW_PRESS: inputAction = Inputs::KeyAction::KEY_PRESS; break;
	case GLFW_REPEAT: inputAction = Inputs::KeyAction::KEY_REPEAT; break;
	case GLFW_RELEASE: inputAction = Inputs::KeyAction::KEY_RELEASE; break;
	case -1: break; // We don't care about events GLFW can't handle
	default: std::cout << "Unhandled glfw action passed to GLFWActionToInputManagerAction in GLFWWIndowWrapper: %i\n" << _glfwAction;
	}

	return inputAction;
}

Inputs::KeyCode WindowModule::GLFWKeyToInputManagerKey(int32_t _glfwKey)
{
	Inputs::KeyCode inputKey = Inputs::KeyCode::_NONE;

	switch (_glfwKey)
	{
	case GLFW_KEY_SPACE: inputKey = Inputs::KeyCode::KEY_SPACE; break;
	case GLFW_KEY_APOSTROPHE: inputKey = Inputs::KeyCode::KEY_APOSTROPHE; break;
	case GLFW_KEY_COMMA: inputKey = Inputs::KeyCode::KEY_COMMA; break;
	case GLFW_KEY_MINUS: inputKey = Inputs::KeyCode::KEY_MINUS; break;
	case GLFW_KEY_PERIOD: inputKey = Inputs::KeyCode::KEY_PERIOD; break;
	case GLFW_KEY_SLASH: inputKey = Inputs::KeyCode::KEY_SLASH; break;
	case GLFW_KEY_0: inputKey = Inputs::KeyCode::KEY_0; break;
	case GLFW_KEY_1: inputKey = Inputs::KeyCode::KEY_1; break;
	case GLFW_KEY_2: inputKey = Inputs::KeyCode::KEY_2; break;
	case GLFW_KEY_3: inputKey = Inputs::KeyCode::KEY_3; break;
	case GLFW_KEY_4: inputKey = Inputs::KeyCode::KEY_4; break;
	case GLFW_KEY_5: inputKey = Inputs::KeyCode::KEY_5; break;
	case GLFW_KEY_6: inputKey = Inputs::KeyCode::KEY_6; break;
	case GLFW_KEY_7: inputKey = Inputs::KeyCode::KEY_7; break;
	case GLFW_KEY_8: inputKey = Inputs::KeyCode::KEY_8; break;
	case GLFW_KEY_9: inputKey = Inputs::KeyCode::KEY_9; break;
	case GLFW_KEY_SEMICOLON: inputKey = Inputs::KeyCode::KEY_SEMICOLON; break;
	case GLFW_KEY_EQUAL: inputKey = Inputs::KeyCode::KEY_EQUAL; break;
	case GLFW_KEY_A: inputKey = Inputs::KeyCode::KEY_A; break;
	case GLFW_KEY_B: inputKey = Inputs::KeyCode::KEY_B; break;
	case GLFW_KEY_C: inputKey = Inputs::KeyCode::KEY_C; break;
	case GLFW_KEY_D: inputKey = Inputs::KeyCode::KEY_D; break;
	case GLFW_KEY_E: inputKey = Inputs::KeyCode::KEY_E; break;
	case GLFW_KEY_F: inputKey = Inputs::KeyCode::KEY_F; break;
	case GLFW_KEY_G: inputKey = Inputs::KeyCode::KEY_G; break;
	case GLFW_KEY_H: inputKey = Inputs::KeyCode::KEY_H; break;
	case GLFW_KEY_I: inputKey = Inputs::KeyCode::KEY_I; break;
	case GLFW_KEY_J: inputKey = Inputs::KeyCode::KEY_J; break;
	case GLFW_KEY_K: inputKey = Inputs::KeyCode::KEY_K; break;
	case GLFW_KEY_L: inputKey = Inputs::KeyCode::KEY_L; break;
	case GLFW_KEY_M: inputKey = Inputs::KeyCode::KEY_M; break;
	case GLFW_KEY_N: inputKey = Inputs::KeyCode::KEY_N; break;
	case GLFW_KEY_O: inputKey = Inputs::KeyCode::KEY_O; break;
	case GLFW_KEY_P: inputKey = Inputs::KeyCode::KEY_P; break;
	case GLFW_KEY_Q: inputKey = Inputs::KeyCode::KEY_Q; break;
	case GLFW_KEY_R: inputKey = Inputs::KeyCode::KEY_R; break;
	case GLFW_KEY_S: inputKey = Inputs::KeyCode::KEY_S; break;
	case GLFW_KEY_T: inputKey = Inputs::KeyCode::KEY_T; break;
	case GLFW_KEY_U: inputKey = Inputs::KeyCode::KEY_U; break;
	case GLFW_KEY_V: inputKey = Inputs::KeyCode::KEY_V; break;
	case GLFW_KEY_W: inputKey = Inputs::KeyCode::KEY_W; break;
	case GLFW_KEY_X: inputKey = Inputs::KeyCode::KEY_X; break;
	case GLFW_KEY_Y: inputKey = Inputs::KeyCode::KEY_Y; break;
	case GLFW_KEY_Z: inputKey = Inputs::KeyCode::KEY_Z; break;
	case GLFW_KEY_LEFT_BRACKET: inputKey = Inputs::KeyCode::KEY_LEFT_BRACKET; break;
	case GLFW_KEY_BACKSLASH: inputKey = Inputs::KeyCode::KEY_BACKSLASH; break;
	case GLFW_KEY_RIGHT_BRACKET: inputKey = Inputs::KeyCode::KEY_RIGHT_BRACKET; break;
	case GLFW_KEY_GRAVE_ACCENT: inputKey = Inputs::KeyCode::KEY_GRAVE_ACCENT; break;
	case GLFW_KEY_WORLD_1: inputKey = Inputs::KeyCode::KEY_WORLD_1; break;
	case GLFW_KEY_WORLD_2: inputKey = Inputs::KeyCode::KEY_WORLD_2; break;
	case GLFW_KEY_ESCAPE: inputKey = Inputs::KeyCode::KEY_ESCAPE; break;
	case GLFW_KEY_ENTER: inputKey = Inputs::KeyCode::KEY_ENTER; break;
	case GLFW_KEY_TAB: inputKey = Inputs::KeyCode::KEY_TAB; break;
	case GLFW_KEY_BACKSPACE: inputKey = Inputs::KeyCode::KEY_BACKSPACE; break;
	case GLFW_KEY_INSERT: inputKey = Inputs::KeyCode::KEY_INSERT; break;
	case GLFW_KEY_DELETE: inputKey = Inputs::KeyCode::KEY_DELETE; break;
	case GLFW_KEY_RIGHT: inputKey = Inputs::KeyCode::KEY_RIGHT; break;
	case GLFW_KEY_LEFT: inputKey = Inputs::KeyCode::KEY_LEFT; break;
	case GLFW_KEY_DOWN: inputKey = Inputs::KeyCode::KEY_DOWN; break;
	case GLFW_KEY_UP: inputKey = Inputs::KeyCode::KEY_UP; break;
	case GLFW_KEY_PAGE_UP: inputKey = Inputs::KeyCode::KEY_PAGE_UP; break;
	case GLFW_KEY_PAGE_DOWN: inputKey = Inputs::KeyCode::KEY_PAGE_DOWN; break;
	case GLFW_KEY_HOME: inputKey = Inputs::KeyCode::KEY_HOME; break;
	case GLFW_KEY_END: inputKey = Inputs::KeyCode::KEY_END; break;
	case GLFW_KEY_CAPS_LOCK: inputKey = Inputs::KeyCode::KEY_CAPS_LOCK; break;
	case GLFW_KEY_SCROLL_LOCK: inputKey = Inputs::KeyCode::KEY_SCROLL_LOCK; break;
	case GLFW_KEY_NUM_LOCK: inputKey = Inputs::KeyCode::KEY_NUM_LOCK; break;
	case GLFW_KEY_PRINT_SCREEN: inputKey = Inputs::KeyCode::KEY_PRINT_SCREEN; break;
	case GLFW_KEY_PAUSE: inputKey = Inputs::KeyCode::KEY_PAUSE; break;
	case GLFW_KEY_F1: inputKey = Inputs::KeyCode::KEY_F1; break;
	case GLFW_KEY_F2: inputKey = Inputs::KeyCode::KEY_F2; break;
	case GLFW_KEY_F3: inputKey = Inputs::KeyCode::KEY_F3; break;
	case GLFW_KEY_F4: inputKey = Inputs::KeyCode::KEY_F4; break;
	case GLFW_KEY_F5: inputKey = Inputs::KeyCode::KEY_F5; break;
	case GLFW_KEY_F6: inputKey = Inputs::KeyCode::KEY_F6; break;
	case GLFW_KEY_F7: inputKey = Inputs::KeyCode::KEY_F7; break;
	case GLFW_KEY_F8: inputKey = Inputs::KeyCode::KEY_F8; break;
	case GLFW_KEY_F9: inputKey = Inputs::KeyCode::KEY_F9; break;
	case GLFW_KEY_F10: inputKey = Inputs::KeyCode::KEY_F10; break;
	case GLFW_KEY_F11: inputKey = Inputs::KeyCode::KEY_F11; break;
	case GLFW_KEY_F12: inputKey = Inputs::KeyCode::KEY_F12; break;
	case GLFW_KEY_F13: inputKey = Inputs::KeyCode::KEY_F13; break;
	case GLFW_KEY_F14: inputKey = Inputs::KeyCode::KEY_F14; break;
	case GLFW_KEY_F15: inputKey = Inputs::KeyCode::KEY_F15; break;
	case GLFW_KEY_F16: inputKey = Inputs::KeyCode::KEY_F16; break;
	case GLFW_KEY_F17: inputKey = Inputs::KeyCode::KEY_F17; break;
	case GLFW_KEY_F18: inputKey = Inputs::KeyCode::KEY_F18; break;
	case GLFW_KEY_F19: inputKey = Inputs::KeyCode::KEY_F19; break;
	case GLFW_KEY_F20: inputKey = Inputs::KeyCode::KEY_F20; break;
	case GLFW_KEY_F21: inputKey = Inputs::KeyCode::KEY_F21; break;
	case GLFW_KEY_F22: inputKey = Inputs::KeyCode::KEY_F22; break;
	case GLFW_KEY_F23: inputKey = Inputs::KeyCode::KEY_F23; break;
	case GLFW_KEY_F24: inputKey = Inputs::KeyCode::KEY_F24; break;
	case GLFW_KEY_F25: inputKey = Inputs::KeyCode::KEY_F25; break;
	case GLFW_KEY_KP_0: inputKey = Inputs::KeyCode::KEY_KP_0; break;
	case GLFW_KEY_KP_1: inputKey = Inputs::KeyCode::KEY_KP_1; break;
	case GLFW_KEY_KP_2: inputKey = Inputs::KeyCode::KEY_KP_2; break;
	case GLFW_KEY_KP_3: inputKey = Inputs::KeyCode::KEY_KP_3; break;
	case GLFW_KEY_KP_4: inputKey = Inputs::KeyCode::KEY_KP_4; break;
	case GLFW_KEY_KP_5: inputKey = Inputs::KeyCode::KEY_KP_5; break;
	case GLFW_KEY_KP_6: inputKey = Inputs::KeyCode::KEY_KP_6; break;
	case GLFW_KEY_KP_7: inputKey = Inputs::KeyCode::KEY_KP_7; break;
	case GLFW_KEY_KP_8: inputKey = Inputs::KeyCode::KEY_KP_8; break;
	case GLFW_KEY_KP_9: inputKey = Inputs::KeyCode::KEY_KP_9; break;
	case GLFW_KEY_KP_DECIMAL: inputKey = Inputs::KeyCode::KEY_KP_DECIMAL; break;
	case GLFW_KEY_KP_DIVIDE: inputKey = Inputs::KeyCode::KEY_KP_DIVIDE; break;
	case GLFW_KEY_KP_MULTIPLY: inputKey = Inputs::KeyCode::KEY_KP_MULTIPLY; break;
	case GLFW_KEY_KP_SUBTRACT: inputKey = Inputs::KeyCode::KEY_KP_SUBTRACT; break;
	case GLFW_KEY_KP_ADD: inputKey = Inputs::KeyCode::KEY_KP_ADD; break;
	case GLFW_KEY_KP_ENTER: inputKey = Inputs::KeyCode::KEY_KP_ENTER; break;
	case GLFW_KEY_KP_EQUAL: inputKey = Inputs::KeyCode::KEY_KP_EQUAL; break;
	case GLFW_KEY_LEFT_SHIFT: inputKey = Inputs::KeyCode::KEY_LEFT_SHIFT; break;
	case GLFW_KEY_LEFT_CONTROL: inputKey = Inputs::KeyCode::KEY_LEFT_CONTROL; break;
	case GLFW_KEY_LEFT_ALT: inputKey = Inputs::KeyCode::KEY_LEFT_ALT; break;
	case GLFW_KEY_LEFT_SUPER: inputKey = Inputs::KeyCode::KEY_LEFT_SUPER; break;
	case GLFW_KEY_RIGHT_SHIFT: inputKey = Inputs::KeyCode::KEY_RIGHT_SHIFT; break;
	case GLFW_KEY_RIGHT_CONTROL: inputKey = Inputs::KeyCode::KEY_RIGHT_CONTROL; break;
	case GLFW_KEY_RIGHT_ALT: inputKey = Inputs::KeyCode::KEY_RIGHT_ALT; break;
	case GLFW_KEY_RIGHT_SUPER: inputKey = Inputs::KeyCode::KEY_RIGHT_SUPER; break;
	case GLFW_KEY_MENU: inputKey = Inputs::KeyCode::KEY_MENU; break;
	case -1: break; // We don't care about events GLFW can't handle
	default:
		std::cout << "Unhandled glfw key passed to GLFWKeyToInputManagerKey in GLFWWIndowWrapper: %i\n" << _glfwKey;
		break;
	}

	return inputKey;
}

int32_t WindowModule::GLFWModsToInputManagerMods(int32_t _glfwMods)
{
	int32_t inputMods = 0;

	if (_glfwMods & GLFW_MOD_SHIFT) inputMods |= (int32_t)Inputs::InputModifier::SHIFT;
	if (_glfwMods & GLFW_MOD_ALT) inputMods |= (int32_t)Inputs::InputModifier::ALT;
	if (_glfwMods & GLFW_MOD_CONTROL) inputMods |= (int32_t)Inputs::InputModifier::CONTROL;
	if (_glfwMods & GLFW_MOD_SUPER) inputMods |= (int32_t)Inputs::InputModifier::SUPER;
	if (_glfwMods & GLFW_MOD_CAPS_LOCK) inputMods |= (int32_t)Inputs::InputModifier::CAPS_LOCK;
	if (_glfwMods & GLFW_MOD_NUM_LOCK) inputMods |= (int32_t)Inputs::InputModifier::NUM_LOCK;

	return inputMods;
}

Inputs::MouseButton WindowModule::GLFWButtonToInputManagerMouseButton(int32_t _glfwButton)
{
	Inputs::MouseButton inputMouseButton = Inputs::MouseButton::_NONE;

	switch (_glfwButton)
	{
	case GLFW_MOUSE_BUTTON_1: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_1; break;
	case GLFW_MOUSE_BUTTON_2: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_2; break;
	case GLFW_MOUSE_BUTTON_3: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_3; break;
	case GLFW_MOUSE_BUTTON_4: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_4; break;
	case GLFW_MOUSE_BUTTON_5: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_5; break;
	case GLFW_MOUSE_BUTTON_6: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_6; break;
	case GLFW_MOUSE_BUTTON_7: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_7; break;
	case GLFW_MOUSE_BUTTON_8: inputMouseButton = Inputs::MouseButton::MOUSE_BUTTON_8; break;
	case -1: break; // We don't care about events GLFW can't handle
	default: std::cout << "Unhandled glfw button passed to GLFWButtonToInputManagerMouseButton in GLFWWIndowWrapper: %i\n" << _glfwButton;
		break;
	}

	return inputMouseButton;
}

void WindowModule::GLFWMouseButtonCallback(GLFWwindow* _glfwWindow, int32_t _button, int32_t _action, int32_t _mods)
{
	WindowModule* window = static_cast<WindowModule*>(glfwGetWindowUserPointer(_glfwWindow));
	const Inputs::KeyAction inputAction = GLFWActionToInputManagerAction(_action);
	const int32_t inputMods = GLFWModsToInputManagerMods(_mods);
	const Inputs::MouseButton mouseButton = GLFWButtonToInputManagerMouseButton(_button);

	window->MouseButtonCallback(mouseButton, inputAction, inputMods);
}

void WindowModule::GLFWKeyCallback(GLFWwindow* _glfwWindow, int32_t _key, int32_t _scancode, int32_t _action, int32_t _mods)
{
	WindowModule* window = static_cast<WindowModule*>(glfwGetWindowUserPointer(_glfwWindow));
	const Inputs::KeyAction inputAction = GLFWActionToInputManagerAction(_action);
	const Inputs::KeyCode inputKey = GLFWKeyToInputManagerKey(_key);
	const int32_t inputMods = GLFWModsToInputManagerMods(_mods);

	window->KeyCallback(inputKey, inputAction, inputMods);
}

void WindowModule::GLFWCursorPosCallback(GLFWwindow* _glfwWindow, double _x, double _y)
{
	WindowModule* window = static_cast<WindowModule*>(glfwGetWindowUserPointer(_glfwWindow));
	window->CursorPosCallback(_x, _y);
}

void WindowModule::MouseButtonCallback(Inputs::MouseButton _mouseButton, Inputs::KeyAction _action, int32_t _mods)
{
	inputModule->MouseButtonCallback(_mouseButton, _action, _mods);
}

void WindowModule::KeyCallback(Inputs::KeyCode _inputKey, Inputs::KeyAction _inputAction, int32_t _inputMods)
{
	inputModule->KeyCallback(_inputKey, _inputAction, _inputMods);
}

void WindowModule::CursorPosCallback(double _x, double _y) {
	inputModule->CursorPosCallback(_x, _y);
}


WindowMode WindowModule::StrToWindowMode(const char* _modeStr)
{
	for (uint32_t i = 0; i < static_cast<uint32_t>(WindowMode::_NONE); ++i)
	{
		if (strcmp(WindowModeStrings[i], _modeStr) == 0) return static_cast<WindowMode>(i);
	}

	std::cout << ("Unhandled window mode passed to StrToWindowMode: %s, returning WindowMode::WINDOWED\n", _modeStr) <<
		std::endl;

	return WindowMode::WINDOWED;
}

void WindowModule::Init()
{
	Module::Init();
	// Initialisation GLFW
	if (!glfwInit())
	{
		std::cerr << "Erreur lors de l'initialisation de GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Configure la fenêtre pour ne pas être redimensionnable
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Crée la fenêtre GLFW avec la taille et le nom spécifiés
	window = glfwCreateWindow(size.x, size.y, windowName.c_str(), nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallBack);
}

void WindowModule::Start()
{
	Module::Start();
	sceneManager = moduleManager->GetModule<SceneManager>();
}

void WindowModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void WindowModule::Update()
{
	Module::Update();

	if (!ShouldClose())
	{
		glfwPollEvents();
		secondsSinceTitleUpdate += TimeModule::GetDeltaTime();
		if (secondsSinceTitleUpdate >= updateWindowTitleFrequency)
		{
			secondsSinceTitleUpdate = 0.0f;
			SetWindowTitle(GenerateWindowTitle());
		}
	}
	else
	{
		moduleManager->GetModule<RHIModule>()->GetDevice()->Device().waitIdle();
		Engine::GetInstance()->Quit();
	}
}

void WindowModule::PreRender()
{
	Module::PreRender();
}

void WindowModule::Render()
{
	Module::Render();
}

void WindowModule::RenderGui()
{
	Module::RenderGui();
}

void WindowModule::PostRender()
{
	Module::PostRender();

	//window->display();
}

void WindowModule::Release()
{
	Module::Release();

	//window->close();
}

void WindowModule::Finalize()
{
	Module::Finalize();

	//window->close();
}

std::string WindowModule::GenerateWindowTitle() const
{
	std::string result = windowName;
	result += " | " + sceneManager->GetMainScene()->GetName();
	if (bShowMSInWindowTitle) result += " | " + lve::FloatToString(TimeModule::GetDeltaTimeMilliseconds(), 2) + "ms";
	if (bShowFPSInWindowTitle) result += " - " + lve::FloatToString(1.0f / TimeModule::GetDeltaTime(), 0) + " FPS ";
	return result;
}


bool WindowModule::InitFromConfig()
{
	if (FileManager::FileExists(WINDOW_CONFIG_LOCATION))
		try
		{
			std::ifstream config_file(WINDOW_CONFIG_LOCATION);
			if (!config_file.is_open())
			{
				std::cerr << (std::string("Failed to open window settings config file %s\n") + WINDOW_CONFIG_LOCATION)
					<< std::endl;
				return false;
			}

			json root_object;
			config_file >> root_object;

			bMoveConsoleToOtherMonitor = root_object.value("move console to other monitor on bootup",
			                                               bMoveConsoleToOtherMonitor);
			bAutoRestoreStateOnBootup = root_object.value("auto restore state", bAutoRestoreStateOnBootup);

			if (bAutoRestoreStateOnBootup)
			{
				glm::ivec2 initial_window_pos;

				if (auto it = root_object.find("initial window position");
					it != root_object.end() && it.value().is_array() && it.value().size() == 2)
				{
					initial_window_pos.x = it.value()[0].get<int>();
					initial_window_pos.y = it.value()[1].get<int>();
				}
				else
				{
					initial_window_pos = glm::ivec2(0, 0);
					// Valeur par défaut si la clé n'est pas trouvée ou si elle est mal formée
				}
				position = glm::ivec2(initial_window_pos);

				glm::ivec2 initial_window_size;
				if (auto it = root_object.find("initial window size");
					it != root_object.end() && it.value().is_array() && it.value().size() == 2)
				{
					initial_window_size.x = it.value()[0].get<int>();
					initial_window_size.y = it.value()[1].get<int>();
				}
				else
				{
					initial_window_size = glm::ivec2(800.0f, 600.0f);
					// Valeur par défaut si la clé n'est pas trouvée ou si elle est mal formée
				}
				size = glm::ivec2(initial_window_size);

				bMaximized = root_object.value("maximized", bMaximized);

				const std::string window_mode_str = root_object.value("window mode", "default");
				currentWindowMode                 = StrToWindowMode(window_mode_str.c_str());
			}

			bool b_v_sync_enabled = root_object.value("v-sync", true);
			SetVSyncEnabled(b_v_sync_enabled);

			return true;
		}
		catch (const std::exception& e)
		{
			std::cout << (
				std::string("Failed to parse window settings config file ") + WINDOW_CONFIG_LOCATION + "\n\terror: " + e
				.what()) << std::endl;
			return false;
		}
	std::cout << (std::string("Window settings config file not found: %s\n") + WINDOW_CONFIG_LOCATION) << std::endl;
	return false;
}


void WindowModule::SaveToConfig()
{
	json root_object;

	root_object["move console to other monitor on bootup"] = bMoveConsoleToOtherMonitor;
	root_object["auto restore state"]                      = bAutoRestoreStateOnBootup;
	root_object["initial window position"]                 = {position.x, position.y};
	root_object["initial window size"]                     = {size.x, size.y};
	root_object["maximized"]                               = bMaximized;
	root_object["window mode"]                             = WindowModeToStr();
	root_object["v-sync"]                                  = bVSyncEnabled;

	if (std::ofstream config_file(WINDOW_CONFIG_LOCATION); config_file.is_open())
	{
		config_file << root_object.dump(4); // 4-space indentation for pretty printing
		config_file.close();
	}
	else
	{
		std::cerr << ("Failed to write window settings config file\n");
	}
}
