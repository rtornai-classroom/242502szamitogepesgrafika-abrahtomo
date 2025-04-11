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

float circleRadius = 50.0f;
float speed = 100.0f;

void creatCircle(float x, float y) {
	glBegin(GL_TRIANGLE_FAN);
	//piros középpont
	glColor3f(GLclampf(128.0 / 255.0), GLclampf(0.0 / 255.0), GLclampf(0.0 / 255.0));
	// Specify the next vertex for the triangle fan
	glVertex2f(x, y);
	float angle;
	for (angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 64.0f)) {
		//átmenet zöld árnyalatba
		glColor3f(GLclampf(173.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(47.0 / 255.0));
		float cx = circleRadius * cosf(angle);
		float cy = circleRadius * sinf(angle);
		glVertex2f(x + cx, y + cy);
	}

	float cx = 50.0f * cosf(0.0f);
	float cy = 50.0f * sinf(0.0f);
	glVertex2f(x + cx, y + cy);

	glEnd();
}

void createLine() {
	//3 pixel vastag
	glLineWidth(3.0f);
	//kék szín
	glColor3f(GLclampf(65.0 / 255.0), GLclampf(105.0 / 255.0), GLclampf(225.0 / 255.0));

	//ablak közepének meghatározása
	float center = windowWidth / 2.0f;
	//a vonat hosszának felének a meghatározása
	float halfLineLength = windowWidth / 6.0f;

	glBegin(GL_LINES);
	//y értéke fix, x értéke változik a középpontól negatív és pozitív irányba féltávolsággal
	glVertex2f(center - halfLineLength, center);
	glVertex2f(center + halfLineLength, center);

	glEnd();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);
	// Reset projection matrix stack
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	double time = glfwGetTime();
	float range = windowWidth - 2 * circleRadius;
	float distance = fmod(time * speed, 2 * range);
	float x = circleRadius + (distance <= range ? distance : (2 * range - distance));
	float y = windowHeight / 2.0f;
	creatCircle(x, y);
	createLine();
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