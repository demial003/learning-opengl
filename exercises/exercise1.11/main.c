#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char *vertextShaderSource = "#version 400 core\n"
                                  "layout (location = 0) in vec2 aPos;\n"
                                  "layout (location = 1) in vec3 vecColor;\n"
                                  "out vec3 theColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                  "  theColor = vecColor;\n"
                                  "}\0";

const char *fragmentShaderSource = "#version 400 core\n"
                                   "in vec3 theColor;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "FragColor = vec4(theColor, 1.0);\n"
                                   "}\0";

unsigned int shaderProgram;
unsigned int VBO;
unsigned int EBO;
unsigned int VAO;
unsigned int v_posLoc = 0;
unsigned int v_colorLoc = 1;
unsigned int currentMode = 0;

void setupData() {

  float vertices[] = {
      0.8f,  0.0f, 0.7f, 0.7f, 0.7f, 0.8f,  0.8f,  0.7f, 0.7f, 0.7f,
      -0.8f, 0.0f, 0.7f, 0.7f, 0.7f, -0.8f, 0.0f,  0.7f, 0.7f, 0.7f,
      0.6f,  0.0f, 0.7f, 0.7f, 0.7f, 0.6f,  0.6f,  0.7f, 0.7f, 0.7f,
      -0.6f, 0.0f, 0.7f, 0.7f, 0.7f, -0.6f, -0.6f, 0.7f, 0.7f, 0.7f,

  };
  float indices[] = {};

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(v_posLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)0);
  glVertexAttribPointer(v_colorLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));

  glEnableVertexAttribArray(v_posLoc);
  glEnableVertexAttribArray(v_colorLoc);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void setupShaders() {
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertextShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR SHADER FAILED %s\n", infoLog);
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(fragmentShader);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR SHADER FAILED %s\n", infoLog);
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("Failed to compile shader program %s", infoLog);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void framebuffer_size_callback(GLFWwindow *window, int height, int width) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    currentMode = (currentMode + 1) % 2;
}

int main(void) {
  GLFWwindow *window;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(900, 900, "exercies 1.11", NULL, NULL);
  if (window == NULL) {
    printf("failed to create glfw window");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("failed to initialize GLAD");
    exit(EXIT_FAILURE);
  }

  glViewport(0, 0, 600, 600);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  setupShaders();
  setupData();

  while (!glfwWindowShouldClose(window)) {
    processInput(window);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, 8);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
