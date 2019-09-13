#include "Utilities.h"
#include "GameScenes.h"

#include <iostream>

using namespace std;

int main(void)
{
	GLFWwindow* window;
	
	/* Initialize GLFW library */
	if (!glfwInit()) {
		cout << "GLFW initialization failed!" << endl;
		return -1;
	}
	
	glfwWindowHint(GLFW_RESIZABLE, 0);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(Renderer::screenWidth, Renderer::screenHeight, "The Light", NULL, NULL);
	if (!window)
	{
		cout << "creating window failed!" << endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Initialize GLEW library */
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "GLEW initialization failed! Error message:" << glewGetErrorString(err) << endl;
		glfwTerminate();
		return -1;
	}

	/* Initialize image load library */
	ilInit();

	const char* version = (char *)glGetString(GL_VERSION);
	cout << version << endl;

	Renderer::init(window);
	Keyboard::init(window);

	Director::getInstance()->useScene(shared_ptr<MasterScene>(new MasterScene));

	while (!glfwWindowShouldClose(window))
	{
		Director::getInstance()->update();
		Renderer::getInstance()->render();

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
