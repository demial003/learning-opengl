#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.7f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
