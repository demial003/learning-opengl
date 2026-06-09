#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char *vertexShaderSource = "#version 400 core\n"
                                 "layout (location = 0) in vec2 aPos;\n"
                                 "layout (location = 1) in vec3 vecColor;\n"
                                 "layout (location = 2) in vec2 aTexCoord;\n"
                                 "out vec3 theColor;\n"
                                 "out vec2 TexCoord;\n"
                                 "void main()\n"
                                 "{\n"
                                 "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                 "  theColor = vecColor;\n"
                                 "	TexCoord = aTexCoord;\n"
                                 "}\0";

const char *fragmentShaderSource =
    "#version 400 core\n"
    "in vec3 theColor;\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "uniform float textureMix;\n"
    "void main()\n"
    "{\n"
    "FragColor = mix(texture(texture1, "
    "TexCoord), texture(texture2, vec2(-TexCoord.x, TexCoord.y)), "
    "textureMix);\n"
    "}\0";

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int shaderProgram;
unsigned int texture1;
unsigned int texture2;
unsigned int v_posLoc = 0;
unsigned int v_colorLoc = 1;
unsigned int v_texLoc = 2;

void setupGeometry() {
  float vertices[] = {
      -0.5f, -0.5f, 1.0f, 0.3f, 0.8f, 0.0f, 0.0f, //
      -0.5f, 0.5f,  1.0f, 0.0f, 0.5f, 0.0f, 1.0f, //
      0.5f,  0.5f,  1.0f, 0.9f, 0.8f, 1.0f, 1.0f, //
      0.5f,  -0.5f, 0.0f, 1.0f, 0.8f, 1.0f, 0.0f, //
  };

  unsigned int indices[] = {
      0, 2, 1, //
      0, 3, 2  //
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

  glVertexAttribPointer(v_posLoc, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void *)0);
  glVertexAttribPointer(v_colorLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glVertexAttribPointer(v_texLoc, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void *)(5 * sizeof(float)));

  glEnableVertexAttribArray(v_posLoc);
  glEnableVertexAttribArray(v_colorLoc);
  glEnableVertexAttribArray(v_texLoc);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void setupTextures() {
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("container.jpg", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fprintf(stderr, "failed to load image xdd ");
    exit(EXIT_FAILURE);
  }
  stbi_image_free(data);

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(true);
  data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    fprintf(stderr, "failed to load image xdd ");
    exit(EXIT_FAILURE);
  }

  stbi_image_free(data);
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

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
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

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  GLint tex_mix_loc = glGetUniformLocation(shaderProgram, "textureMix");
  float mix = 0.0f;

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    mix += 0.1f;
    if (mix >= 1.0)
      mix = 1.0;
    glUniform1f(tex_mix_loc, mix);
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    mix -= 0.1f;
    if (mix <= 0.0f)
      mix = 0.0f;
    glUniform1f(tex_mix_loc, mix);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
int main(void) {
  GLFWwindow *window;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(900, 900, "textures", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "failed to create glfw window");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("failed to initialize GLAD");
    exit(EXIT_FAILURE);
  }

  glViewport(0, 0, 900, 900);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  setupShaders();
  setupTextures();
  setupGeometry();

  glUseProgram(shaderProgram);
  GLint tex_uniform_loc1 = glGetUniformLocation(shaderProgram, "texture1");
  GLint tex_uniform_loc2 = glGetUniformLocation(shaderProgram, "texture2");
  glUniform1i(tex_uniform_loc1, 0);
  glUniform1i(tex_uniform_loc2, 1);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.6f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
