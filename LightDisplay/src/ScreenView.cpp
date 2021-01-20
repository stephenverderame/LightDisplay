#include <ScreenView.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <screen/2DScreenDisplay.h>
#include <unordered_map>

/**
 * Keeps track of active windows and their displays for GLFW callback functions
 * Empty when there are no active windows
 * Each window must take care to add and remove itself from this map
 */
static std::unordered_map<GLFWwindow*, ScreenDisplay*> displays;

struct ScreenView::impl {
	GLFWwindow* window = nullptr;
	std::unique_ptr<ScreenDisplay2D> display = nullptr;
};

void windowResizeCallback(GLFWwindow* wind, int width, int height) {
	displays.at(wind)->resize(width, height);
}

ScreenView::ScreenView() : pimpl(std::make_unique<impl>())
{
	if (displays.empty() && 
		glfwInit() == GLFW_FALSE) throw std::runtime_error("Could not init glfw");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	pimpl->window = glfwCreateWindow(800, 800, "ScreenVisualizer", nullptr, nullptr);
	if (pimpl->window == nullptr) {
		throw std::runtime_error("Could not create window");
	}
	glfwMakeContextCurrent(pimpl->window);
	if (displays.empty() && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Could not load opengl");
	pimpl->display = std::make_unique<ScreenDisplay2D>();
	pimpl->display->resize(800, 800);
	glfwSetWindowSizeCallback(pimpl->window, windowResizeCallback);
	displays.emplace(pimpl->window, pimpl->display.get());
}

void ScreenView::draw(const std::vector<Visual>& visuals)
{
	pimpl->display->render(visuals);
	glfwSwapBuffers(pimpl->window);
	glfwPollEvents();
}

bool ScreenView::isActive()
{
	return !glfwWindowShouldClose(pimpl->window);
}

ScreenView::~ScreenView() {
	displays.erase(pimpl->window);
	glfwDestroyWindow(pimpl->window);
	if(displays.empty()) glfwTerminate();
};
