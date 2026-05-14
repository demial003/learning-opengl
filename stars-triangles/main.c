#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char *vertextShaderSource =
    "#version 400 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
    "}\0";

const char *fragmentShaderSource = "#version 400 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "FragColor = vec4(0.7f, 0.3f, 1.0f, 1.0f);\n"
                                   "}\0";

unsigned int shaderProgram;
unsigned int VAO;
unsigned int VBO;

void setupData() {
  float vertices[] = {
      0.0f,  0.0f,   0.0f,  0.75f, -0.1f, 0.1f,

      -0.1f, 0.1f,   -0.8f, 0.2f,

      -0.8f, 0.2f,   -0.2f, -0.1f,

      -0.2f, -0.1f,  -0.5f, -0.7f,

      0.0f,  -0.25f, 0.5f,  -0.7f,

      0.5f,  -0.7f,  0.2f,  -0.1f,

      0.2f,  -0.1f,  0.8f,  0.2f,

      0.8f,  0.2f,   0.1f,  0.1f,

      0.1f,  0.1f,   0.0f,  0.75f,

  };

  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void setupShaders() {
  unsigned int vertextShader;
  vertextShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertextShader, 1, &vertextShaderSource, NULL);
  glCompileShader(vertextShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertextShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertextShader, 512, NULL, infoLog);
    printf("ERROR SHADER FAILED %s\n", infoLog);
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glCreateProgram();
  glAttachShader(shaderProgram, vertextShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("Failed to compile shader program %s", infoLog);
  }

  glDeleteShader(vertextShader);
  glDeleteShader(fragmentShader);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
int main(void) {

  GLFWwindow *window;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(600, 600, "stars-triangles", NULL, NULL);
  if (window == NULL) {
    printf("failed to create GLFW widow");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("failed to initialize GLAD");
    exit(EXIT_FAILURE);
  }

  glViewport(0, 0, 600, 400);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  setupShaders();
  setupData();

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // glClearColor(0.7f, 0.8f, 0.8f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 20);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
