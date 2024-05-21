#include "LveEngine/lve_window.h"

#include <stdexcept>

namespace lve
{
	// Constructeur  de la classe LveWindow
	LveWindow::LveWindow(const int _width, const int _height, const std::string& _name) : width{_width},
		height{_height}, windowName{_name}
	{
		// Initialise la fenêtre GLFW
		InitWindow();
	}

	void LveWindow::InitWindow()
	{
		// Initialise GLFW
		glfwInit();

		// Configure la fenêtre pour ne pas être redimensionnable
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		// Crée la fenêtre GLFW avec la taille et le nom spécifiés
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallBack);
	}

	void LveWindow::CreateWindowSurface(const vk::Instance _instance, vk::SurfaceKHR* _surface) const
	{
		if (glfwCreateWindowSurface(_instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(_surface)) !=
		    VK_SUCCESS)
			throw std::runtime_error("failed to create window surface");
	}

	// Destructeur de la classe LveWindow
	LveWindow::~LveWindow()
	{
		// Détruit la fenêtre GLFW
		glfwDestroyWindow(window);
		// Termine GLFW
		glfwTerminate();
	}

	void LveWindow::FrameBufferResizeCallBack(GLFWwindow* _window, const int _width, const int _height)
	{
		const auto lve_window         = static_cast<LveWindow*>(glfwGetWindowUserPointer(_window));
		lve_window->frameBufferResize = true;
		lve_window->width             = _width;
		lve_window->height            = _height;
	}

	void LveWindow::CubeCursorWindow(const int _color) const
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

	void LveWindow::StandardCursorWindow(const GlfwCursorType _cursorType) const
	{
		GLFWcursor* GLFW_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

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
		glfwSetCursor(window, GLFW_cursor);
	}

	void LveWindow::SetInputCursorMode(const GlfwCursorMode _mode) const
	{
		switch (_mode)
		{
			case CAPTURED:
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
				break;
			case NORMAL:
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case DISABLED:
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
			case HIDDEN:
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				break;
		}
	}
} // namespace lve
