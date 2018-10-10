#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "glad.h"
#include "glfw3.h"
#include "GameTimer.h"

class Application {
protected:
	static Application* _app;
	HINSTANCE _hAppInstance = nullptr;
	HWND _hMainWindow = nullptr;
	GLFWwindow* _window;

	bool _appPaused = false;
	bool _minimized = false;
	bool _maximized = false;
	bool _resizing = false;
	bool _fullscreenState = false;

	GameTimer _timer;

	int _height = 720;
	int _width = 1280;

	float lastX = ((float)_width) / 2.0f;
	float lastY = ((float)_height) / 2.0f;
	bool firstMouse = true;

	std::wstring _mainWndCaption = L"Test D3D";

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void processInput(GLFWwindow *window, Application &app);

	Application(const Application &rhs) = delete;
	Application &operator=(const Application &rhs) = delete;

	Application();
	virtual ~Application();

public:
	virtual bool initialize();
	virtual void setView();
	virtual void draw(const GameTimer &gt) = 0;
	virtual void update(const GameTimer &gt) = 0;

	int run();
	
	virtual void onKeyboardInput(GLFWwindow *window);
	virtual void onResize();
	virtual void mouse(int button, int state, int x, int y);
	virtual void mouseDrag(int x, int y);

	float aspectRatio() const;
	void renderText(float x, float y, const char *text, void* font = NULL);
	void calculateFrameStatus();
};

#endif