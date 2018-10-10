#include <iostream>
#include <memory>

#include "glad.h"
#include "glfw3.h"

#include "Shader.h"
#include "GeometryGenerator.h"
#include "Application.h"
#include "PhisApplication.h"

int main() {
	
	PhisApplication theApp;
	if (!theApp.initialize()) {
		return 0;
	}
	return theApp.run();
}