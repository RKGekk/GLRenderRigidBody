#include <iostream>

#include "Application.h"

Application *Application::_app = nullptr;

void Application::processInput(GLFWwindow *window, Application &app) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
	}
	app.onKeyboardInput(window);
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	(*_app).mouse(0, 0, xpos, ypos);
}


Application::Application() {
	_app = this;
}

Application::~Application() {
	glfwTerminate();	
}

bool Application::initialize() {

	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(_width, _height, "PhisOpenGL", NULL, NULL);
    if (_window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(_window, mouse_callback);

	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }    
	return true;
}

float Application::aspectRatio() const {
	return static_cast<float>(_width) / _height;
}

int Application::run() {
	_timer.reset();
	while (!glfwWindowShouldClose(_window)) {
		_timer.tick();
		processInput(_window, *_app);
		if (!_appPaused) {
			calculateFrameStatus();
			update(_timer);
			draw(_timer);
			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
		else {
			Sleep(100);
		}
	}
	return 0;
}

void Application::setView() {
	
}

void Application::onKeyboardInput(GLFWwindow *window) {

}

void Application::onResize() {
	
}

void Application::mouse(int button, int state, int x, int y) {
}

void Application::mouseDrag(int x, int y) {
}

void Application::renderText(float x, float y, const char *text, void *font) {
	
}

void Application::calculateFrameStatus() {

}