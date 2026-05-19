#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char *vertextShaderSource = "#version 400 core\n"
                                  "layout (location = 0) in vec3 aPos;\n"
                                  "layout (location = 1) in vec3 vecColor;\n"
                                  "out vec3 theColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "  gl_Position = vec4(aPos, 1.0);\n"
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
unsigned int VAO;
unsigned int VBO;
unsigned int EBO;
unsigned int v_posLoc = 0;
unsigned int v_colorLoc = 1;

void setupData() {
  float vertices[] = {
      0.5,  0.5,  0.5,  1.0, 0.0, 0.0, // first point
      0.5,  -0.5, 0.5,  1.0, 0.0, 0.0, // second point
      -0.5, 0.5,  0.5,  1.0, 0.0, 0.0, // third point
      -0.5, -0.5, 0.5,  1.0, 0.0, 0.0, // fourth point
      0.5,  0.5,  -0.5, 0.0, 0.0, 1.0, // fifth point
      0.5,  -0.5, -0.5, 0.0, 0.0, 1.0, // sixth point
      -0.5, 0.5,  -0.5, 0.0, 1.0, 0.0, // seventh point
      -0.5, -0.5, -0.5, 0.0, 1.0, 0.0  // eight point
  };

  unsigned int indices[] = {
      0,
      2,
      1, // first triangle
      3,
      1,
      2, // second triangle
      4,
      6,
      0, // third triangle
      2,
      0,
      6, // fourth triangle
      // 5, 7, 4, // fifth triangle
      // 6, 4, 7, // sixth triangle
      1,
      3,
      5, // seventh triangle
      7,
      5,
      3, // eighth triangle
      0,
      1,
      4, // ninth triangle
      5,
      4,
      1, // tenth triangle
      3,
      2,
      7, // eleventh triangle
      6,
      7,
      2, // twelfth triangle
  };

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(v_posLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)0);
  glVertexAttribPointer(v_colorLoc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(v_posLoc);
  glEnableVertexAttribArray(v_colorLoc);
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

  shaderProgram = glCreateProgram();
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

  window = glfwCreateWindow(900, 900, "stars-triangles", NULL, NULL);
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

  glViewport(0, 0, 800, 800);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  setupShaders();
  setupData();

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    //  glDrawArrays(GL_TRIANGLES, 0, 12);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
