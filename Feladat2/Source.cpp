#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include "glm/glm.hpp"
#include <Windows.h>
#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

static int WIN_WIDTH = 600;
static int WIN_HEIGHT = 600;

GLint dragged = -1;
GLdouble updateFrequency = 0.01, lastUpdate;

static std::vector<glm::vec3> controlPoints = {
    glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
};


int controllPoints = 4;

GLuint VBO;
GLuint VAO;
GLFWwindow* window;
GLuint shaderProgram;
GLchar windowTitle[] = "Masodik feladat!";

void updateVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), controlPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

string readShaderSource(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

void checkOpenGLError() {
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        cout << "Error found: " << glErr << endl;
        glErr = glGetError();
    }
}

void checkShaderError(GLuint shader) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    cerr << "Shader Compilation Failed: " << infoLog << endl;
}

GLuint createShaderProgram() {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    string vertShaderStr = readShaderSource("vertexShader.glsl");
    string fragShaderStr = readShaderSource("fragmentShader.glsl");

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();

    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);

    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        checkShaderError(vShader);
    }

    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        checkShaderError(fShader);
    }

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);

    glLinkProgram(vfProgram);
    checkOpenGLError();
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        char infoLog[512];
        glGetProgramInfoLog(vfProgram, 512, nullptr, infoLog);
        cerr << "Program Linking Failed: " << infoLog << endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return vfProgram;
}

int fact(int n) {
    if (n == 0)
        return 1;
    return n * fact(n - 1);
}

GLfloat poly(int n, int i, GLfloat t) {
    GLfloat B = (fact(n) / (fact(i) * fact(n - i)) * pow(t, i) * pow(1 - t, n - i));
    return B;
}

GLfloat dist2(glm::vec3 P1, glm::vec3 P2) {
    GLfloat t1 = P1.x - P2.x;
    GLfloat t2 = P1.y - P2.y;
    return t1 * t1 + t2 * t2;
}

GLint getActivePoint(vector<glm::vec3> p, GLint size, GLfloat sens, GLfloat x, GLfloat y) {
    GLint i;
    GLfloat s = sens * sens;
    float xNorm = x / (WIN_WIDTH / 2) - 1.0f;
    float yNorm = y / (WIN_HEIGHT / 2) - 1.0f;
    glm::vec3 P = glm::vec3(xNorm, yNorm, 0.0f);
    for (i = 0; i < size; i++) {
        if (dist2(p[i], P) < s) {
            return i;
        }
    }
    return -1;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
    GLint i;
    GLfloat xNorm = xPos / (WIN_WIDTH / 2) - 1.0f;
    GLfloat yNorm = (WIN_HEIGHT - yPos) / (WIN_HEIGHT / 2) - 1.0f;
    if (dragged >= 0) {
        controlPoints.at(dragged).x = xNorm;
        controlPoints.at(dragged).y = yNorm;
        controlPoints[dragged + 1] = { 0.0f, 0.0f, 0.0f };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), controlPoints.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void addPoint(double xPos, double yPos) {
    GLfloat xNorm = xPos / (WIN_WIDTH / 2) - 1.0f;
    GLfloat yNorm = (WIN_HEIGHT - yPos) / (WIN_HEIGHT / 2) - 1.0f;
    controlPoints.insert((controlPoints.begin() + (controllPoints * 2)), glm::vec3(xNorm, yNorm, 0.0f));
    controlPoints.insert((controlPoints.begin() + (controllPoints * 2 + 1)), glm::vec3(0.0f, 0.0f, 0.0f));
    controllPoints++;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), controlPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void removePoint(int i) {
    controlPoints.erase((controlPoints.begin() + (i + 1)));
    controlPoints.erase((controlPoints.begin() + (i)));
    controllPoints--;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    GLint i;
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if ((i = getActivePoint(controlPoints, controlPoints.size(), 0.1f, x, WIN_HEIGHT - y)) != -1 && i % 2 == 0 && i < 17) {
            dragged = i;
        }
        else {
            addPoint(x, y);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        if (dragged >= 0)
            controlPoints[dragged + 1] = { 0.0f, 0.0f, 0.0f };
        dragged = -1;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if ((i = getActivePoint(controlPoints, controlPoints.size(), 0.1f, x, WIN_HEIGHT - y)) != -1 && i % 2 == 0 && i < 17) {
            removePoint(i);
        }
    }
}

void BezierCurve() {
    if (controlPoints.size() > controllPoints * 2) {
        controlPoints.erase(controlPoints.begin() + controllPoints * 2, controlPoints.end());
    }

    glm::vec3 nextPoint;
    GLfloat t = 0.0f;
    GLfloat increment = 0.001f;
    while (t <= 1.0f) {
        nextPoint = glm::vec3(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < controllPoints; i++) {
            nextPoint.x += poly(controllPoints - 1, i, t) * controlPoints.at(0 + i * 2).x;
            nextPoint.y += poly(controllPoints - 1, i, t) * controlPoints.at(0 + i * 2).y;
            nextPoint.z += poly(controllPoints - 1, i, t) * controlPoints.at(0 + i * 2).z;
        }
        controlPoints.push_back(nextPoint);
        controlPoints.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        t += increment;
    }
    updateVBO();
}

void init(GLFWwindow* window) {
    shaderProgram = createShaderProgram();
    controlPoints.insert(controlPoints.begin() + 8, glm::vec3(controlPoints[6].x + (controlPoints[6].x - controlPoints[4].x), controlPoints[6].y + (controlPoints[6].y - controlPoints[4].y), 0.0f));
    controlPoints.insert(controlPoints.begin() + 9, glm::vec3(0.5f, 0.5f, 0.5f));
    updateVBO();
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec3), controlPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void cleanUpScene(int returnCode) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    exit(returnCode);
}

void display(GLFWwindow* window, double currentTime) {
    glClearColor(GLclampf(255.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(102.0 / 255.0), 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    BezierCurve();
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, controllPoints);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(7.0f);
    glDrawArrays(GL_POINTS, 0, controllPoints);
    glPointSize(1.0f);
    glDrawArrays(GL_POINTS, controllPoints, controlPoints.size());
    if (currentTime - lastUpdate >= updateFrequency) {
        BezierCurve();
        lastUpdate = currentTime;
        while (controlPoints.size() > 44) {
            controlPoints.pop_back();
        }
        updateVBO();
    }
    glBindVertexArray(0);
}

int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__ // To make macOS happy; should not be needed.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if ((window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, windowTitle, nullptr, nullptr)) == nullptr) {
        cerr << "Failed to create GLFW window." << endl;
        cleanUpScene(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init the GLEW system." << endl;
        cleanUpScene(EXIT_FAILURE);
    }
    init(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
