#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char *vertexShaderSource =
    "#version 400 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 vecColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 theColor;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
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
      1.0,  1.0,  1.0,  1.0, 1.0, 0.0, // first point
      1.0,  -1.0, 1.0,  1.0, 1.0, 0.0, // second point
      -1.0, 1.0,  1.0,  1.0, 1.0, 0.0, // third point
      -1.0, -1.0, 1.0,  1.0, 1.0, 0.0, // fourth point

      1.0,  1.0,  1.0,  1.0, 0.0, 0.0, // first point
      -1.0, 1.0,  1.0,  1.0, 0.0, 0.0, // third point
      1.0,  1.0,  -1.0, 1.0, 0.0, 0.0, // fifth point
      -1.0, 1.0,  -1.0, 1.0, 0.0, 0.0, // seventh point

      1.0,  1.0,  -1.0, 0.0, 1.0, 1.0, // fifth point
      1.0,  -1.0, -1.0, 0.0, 1.0, 1.0, // sixth point
      -1.0, 1.0,  -1.0, 0.0, 1.0, 1.0, // seventh point
      -1.0, -1.0, -1.0, 0.0, 1.0, 1.0, // eight point

      1.0,  -1.0, 1.0,  1.0, 0.0, 1.0, // second point
      -1.0, -1.0, 1.0,  1.0, 0.0, 1.0, // fourth point
      1.0,  -1.0, -1.0, 1.0, 0.0, 1.0, // sixth point
      -1.0, -1.0, -1.0, 1.0, 0.0, 1.0, // eight point

      1.0,  1.0,  1.0,  0.0, 1.0, 0.0, // first point
      1.0,  -1.0, 1.0,  0.0, 1.0, 0.0, // second point
      1.0,  1.0,  -1.0, 0.0, 1.0, 0.0, // fifth point
      1.0,  -1.0, -1.0, 0.0, 1.0, 0.0, // sixth point

      -1.0, 1.0,  1.0,  1.0, 1.0, 1.0, // third point
      -1.0, -1.0, 1.0,  1.0, 1.0, 1.0, // fourth point
      -1.0, 1.0,  -1.0, 1.0, 1.0, 1.0, // seventh point
      -1.0, -1.0, -1.0, 1.0, 1.0, 1.0  // eight point
  };

  unsigned int indices[] = {
      0,  2,  1,  // first triangle
      3,  1,  2,  // second triangle
      6,  7,  4,  // third triangle
      5,  4,  7,  // fourth triangle
      9,  8,  11, // fifth triangle
      10, 11, 8,  // sixth triangle
      12, 13, 14, // seventh triangle
      15, 14, 13, // eigth triangle
      16, 17, 18, // ninth triangle
      19, 18, 17, // tenth triangle
      21, 20, 23, // eleventh triangle
      22, 23, 20, // twelfth triangle

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
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("ERROR SHADER FAILED %s\n", infoLog);
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

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

  window = glfwCreateWindow(900, 700, "stars-triangles", NULL, NULL);
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

  glViewport(0, 0, 900, 700);
  //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  //  glEnable(GL_CULL_FACE);
  //  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  setupShaders();
  setupData();

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f),
                        glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

    glm::mat4 projection = glm::mat4(1.0f);
    projection =
        glm::perspective(glm::radians(60.0f), 900.0f / 700.0f, 0.1f, 100.0f);

    unsigned int model_loc = glGetUniformLocation(shaderProgram, "model");

    unsigned int view_loc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projection_loc =
        glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    //     glDrawArrays(GL_TRIANGLES, 0, 4);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
