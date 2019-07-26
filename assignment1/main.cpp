#include <GL/glew.h>        // helps with opengl extensions
#include <GLFW/glfw3.h>     // windowing api
#include <glm/glm.hpp>      // maths
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>          // printf
#include <cstdlib>          // atoi
#include <vector>

#include "polygon_class.h"

#define GLFW_INCLUDE_NONE
#define VALS_PER_VERT 3
#define NUM_VERTS 3
#define TRIANGLE_SEPERATION 0.25

struct world_t {
  std::vector<polygon> objects;
} world;


// create back frame
void render(float delta_time)
{

	// clear and draw objects
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (size_t i = 0; i < world.objects.size(); i++) {
    world.objects.at(i).draw(delta_time);
  }

}

// error callback - output to stderr
static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

// key callback - for polled event handling
static void key_callback(GLFWwindow* window,
  int key, int scancode, int action, int mods) {

  // close application
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  // swap to line mode
  static bool line_state = true;
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    if (line_state) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    line_state = !line_state;
  }
}

// main program, takes a number of sides as parameter
int main(int argc, char **argv)
{

  if (atoi(argv[1]) < 3) {
    printf("%s\n", "Incorrect Input, sides must be greater than 2");
    exit(1);
  }

  int sides = atoi(argv[1]);
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(1);
  }

  // openGL v3.3 and compatibility
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window
  window = glfwCreateWindow(640, 640, "Assignment 1 - polygons", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, key_callback);

  // initialise glew
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    exit(1);
  }

  // create polygons
  polygon first(sides, "big_triangle.vert", "big_triangle.frag");
	polygon second(sides, "minimal.vert", "minimal.frag");
	polygon third(sides, "minimal.vert", "minimal.frag");
	polygon fourth(sides, "minimal.vert", "minimal.frag");

  // rotation, scale, translation
  first.set_position(glm::vec3(0), glm::vec3(1), glm::vec3(0));

  // space inner polygons
  float rotation;
  rotation = glm::pi<float>() / 2;
  second.set_position(glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.25, 0.25, 0.25),
    glm::vec3(TRIANGLE_SEPERATION*glm::cos(rotation), TRIANGLE_SEPERATION*glm::sin(rotation), 0));

  rotation = rotation + glm::pi<float>() * 2 / 3;
  third.set_position(glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.25, 0.25, 0.25),
    glm::vec3(TRIANGLE_SEPERATION*glm::cos(rotation), TRIANGLE_SEPERATION*glm::sin(rotation), 0));

  rotation = rotation + glm::pi<float>() * 2 / 3;
  fourth.set_position(glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.25, 0.25, 0.25),
    glm::vec3(TRIANGLE_SEPERATION*glm::cos(rotation), TRIANGLE_SEPERATION*glm::sin(rotation), 0));

  // assign varying rotation axis
  first.set_spin_axis(glm::vec3(0, 0, 1));
  second.set_spin_axis(glm::vec3(1, 0, 0));
  third.set_spin_axis(glm::vec3(0, 1, 0));
  fourth.set_spin_axis(glm::vec3(0, 0, 1));

  // load objects to word list
  world.objects.push_back(first);
  world.objects.push_back(second);
  world.objects.push_back(third);
  world.objects.push_back(fourth);

  // default background colour
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // run window and drawing/polling cycle
  float time_start = glfwGetTime();
  float time_now;
  while (!glfwWindowShouldClose(window))
  {

    time_now = glfwGetTime();
    float delta_time = time_now - time_start;
    render(delta_time);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(0);

  return 0;
}
