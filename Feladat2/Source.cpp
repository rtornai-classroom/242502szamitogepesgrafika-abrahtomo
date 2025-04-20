#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::string LoadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Nem sikerült megnyitni a shadert: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint CompileShader(GLenum shaderType, const std::string& source) {
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

GLuint CreateProgram(const std::string& vertexPath, const std::string& tessControlPath, const std::string& tessEvalPath, const std::string& fragmentPath) {
    GLuint program = glCreateProgram();

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, LoadShaderSource(vertexPath));
    GLuint tessControlShader = CompileShader(GL_TESS_CONTROL_SHADER, LoadShaderSource(tessControlPath));
    GLuint tessEvalShader = CompileShader(GL_TESS_EVALUATION_SHADER, LoadShaderSource(tessEvalPath));
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, LoadShaderSource(fragmentPath));

    glAttachShader(program, vertexShader);
    glAttachShader(program, tessControlShader);
    glAttachShader(program, tessEvalShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(tessControlShader);
    glDeleteShader(tessEvalShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(600, 600, "Bezier Curve", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    /** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények, probléma esetén kilépés EXIT_FAILURE értékkel. */
    /** Initalize GLEW, so the OpenGL functions will be available, on problem exit with EXIT_FAILURE code. */
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        return -1;
    }

    GLuint shaderProgram = CreateProgram(
        "CurveVertShader.glsl",
        "CurveTessContShader.glsl",
        "CurveTessEvalShader.glsl",
        "CurveFragShader.glsl"
    );

    std::vector<glm::vec4> controlPoints = {
        glm::vec4(-0.75f, -0.5f, 0.0f, 1.0f),
        glm::vec4(-0.25f, 0.75f, 0.0f, 1.0f),
        glm::vec4(0.25f, -0.75f, 0.0f, 1.0f),
        glm::vec4(0.75f, 0.5f, 0.0f, 1.0f)
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(glm::vec4), controlPoints.data(), GL_STATIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(posAttrib);

    GLint modelViewLoc = glGetUniformLocation(shaderProgram, "matModelView");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "matProjection");
    GLint controlPointsNumLoc = glGetUniformLocation(shaderProgram, "controlPointsNumber");

    glm::mat4 modelView = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 600.0f / 600.0f, 0.1f, 100.0f);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, &modelView[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    glUniform1i(controlPointsNumLoc, controlPoints.size());

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glClearColor(1.0f, 1.0f, 102.0f / 255.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_PATCHES, 0, controlPoints.size());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
