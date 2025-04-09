#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#define GL_PI 3.1415926f

using namespace	std;
/* Az OpenGL ablak szélesség és magasság értéke. */
/* Width and height of the OpenGL window. */
GLint		windowWidth = 600;
GLint		windowHeight = 600;
GLchar		windowTitle[] = "Elso feladat!";
GLFWwindow* window = nullptr;

void drawCircle() {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 75.0f);
	// Specify the next vertex for the triangle fan
	glVertex2f(windowWidth / 2.0f, windowHeight / 2.0f);
	float angle;
	for (angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 64.0f)) {
		float x = 100 * cosf(angle);
		float y = 100 * sinf(angle);
		glVertex2f(windowWidth / 2.0f + x, windowHeight / 2.0f + y);
	}

	float x = 100.0f * cosf(0.0f);
	float y = 100.0f * sinf(0.0f);
	glVertex2f(windowWidth / 2.0f + x, windowHeight / 2.0f + y);

	glEnd();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Reset projection matrix stack
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawCircle();
}

void cleanUpScene(int returnCode) {
	glfwTerminate();
	exit(returnCode);
}

int main(void) {
	if (!glfwInit())
		cleanUpScene(EXIT_FAILURE);
	/** A használni kívánt OpenGL verzió: 3.3. */
	/** The needed OpenGL version: 3.3. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__ // To make macOS happy; should not be needed.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	if ((window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr)) == nullptr) {
		cerr << "Failed to create GLFW window." << endl;
		cleanUpScene(EXIT_FAILURE);
	}
	
	glfwMakeContextCurrent(window);
	
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to init the GLEW system." << endl;
		cleanUpScene(EXIT_FAILURE);
	}
	
	glfwSwapInterval(1);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowPos(window, (mode->width - windowWidth) / 2, (mode->height - windowHeight) / 2);
	glfwSetWindowAspectRatio(window, 1, 1);

	glClearColor(GLclampf(255.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(102.0 / 255.0), 1.0);
	
	while (!glfwWindowShouldClose(window)) {
		
		display();
		
		glfwSwapBuffers(window);
		
		glfwPollEvents();
	}
	
	cleanUpScene(EXIT_SUCCESS);
	
	return EXIT_SUCCESS;
}