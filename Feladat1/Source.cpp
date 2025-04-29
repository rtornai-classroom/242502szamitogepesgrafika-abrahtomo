#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#define GL_PI 3.1415926f

using namespace std;

GLint windowWidth = 600;
GLint windowHeight = 600;
GLchar windowTitle[] = "Elso feladat!";
GLFWwindow* window = nullptr;

float circleRadius = 50.0f;
float speed = 100.0f;
float velocityX = 150.0f;
float lastTime = 0.0f;

GLuint shaderProgram;

float circlePosY = 300.0f;
float circlePosX = 300.f;

string loadShaders(const char* shaders) {
    string shaderCode;
    ifstream shaderFile;
    shaderFile.open(shaders);
    stringstream shaderStream;

    shaderStream << shaderFile.rdbuf();
    shaderCode = shaderStream.str();

    return shaderCode;
}

GLuint compileShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        cerr << "Shader Compilation Failed: " << infoLog << endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    string vertexSource = loadShaders(vertexPath);
    string fragmentSource = loadShaders(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        cerr << "Program Linking Failed: " << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

float getCirclePosX() {
    float time = glfwGetTime();
    float range = windowWidth - 2 * circleRadius;
    float distance = fmod(time * speed, 2 * range);
    return circleRadius + (distance <= range ? distance : (2 * range - distance));
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, windowWidth, windowHeight);

    glUseProgram(shaderProgram);

    GLuint circlePosLoc = glGetUniformLocation(shaderProgram, "circlePos");
    GLuint radiusLoc = glGetUniformLocation(shaderProgram, "radius");
    GLuint resolutionLoc = glGetUniformLocation(shaderProgram, "resolution");

    glUniform2f(circlePosLoc, circlePosX, circlePosY);
    glUniform1f(radiusLoc, circleRadius);
    glUniform2f(resolutionLoc, windowWidth, windowHeight);

    float quadVertices[] = {
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    glUniform2f(circlePosLoc, circlePosX, circlePosY);
    glUniform1f(glGetUniformLocation(shaderProgram, "radius"), circleRadius);
    glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), windowWidth, windowHeight);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
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
    /** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények, probléma esetén kilépés EXIT_FAILURE értékkel. */
    /** Initalize GLEW, so the OpenGL functions will be available, on problem exit with EXIT_FAILURE code. */
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init the GLEW system." << endl;
        cleanUpScene(EXIT_FAILURE);
    }
    /** 0 = v-sync kikapcsolva, 1 = v-sync bekapcsolva, n = n db képkockányi idõt várakozunk */
    /** 0 = v-sync off, 1 = v-sync on, n = n pieces frame time waiting */
    glfwSwapInterval(1);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    /** Ablak középre helyezése. */
    /** Putting window in the center. */
    glfwSetWindowPos(window, (mode->width - windowWidth) / 2, (mode->height - windowHeight) / 2);
    /** A window oldalarányának megadása a számláló és az osztó segítségével. (pl. 16:9)*/
    /** Setting the aspect ratio using the numerator and the denominator values. (eg. 16:9) */
    glfwSetWindowAspectRatio(window, 1, 1);

    glClearColor(GLclampf(255.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(102.0 / 255.0), 1.0);

    shaderProgram = createShaderProgram("vertexShader.glsl", "fragmentShader.glsl");

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        circlePosX += velocityX * deltaTime;


        if (circlePosX - circleRadius < 0 || circlePosX + circleRadius > windowWidth) {
            velocityX = -velocityX;

            circlePosX = std::max(circleRadius, std::min(circlePosX, windowWidth - circleRadius));
        }

        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUpScene(EXIT_SUCCESS);

    return EXIT_SUCCESS;
}
