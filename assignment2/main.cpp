#include <cstdlib>          // atoi
#include <fstream>
#include <GL/glew.h>        // helps with opengl extensions
#include <GLFW/glfw3.h>     // windowing api

#define GLM_SWIZZLE

#include <glm/glm.hpp>      // maths
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glu.h>
#include <stdio.h>          // printf
#include <vector>

#include "class_object.h"

int ** maze_matrix;

#define GLFW_INCLUDE_NONE
#define CUBE_NUM_TRIS 12
#define CUBE_NUM_VERTICES 8
#define VALS_PER_VERT 3
#define VALS_PER_TEX 2

static double FOV = glm::pi<float>()/4;
#define winX 640
#define winY 640

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define NUM_TEXTURES 3
#define TEX_SIZE 64

int texture[NUM_TEXTURES];
int texture_toggle = true;

float yRot = 0;

//// world initialisation
// world structure
struct world_t {
  std::vector<object *> objects;

  glm::mat4 projection_matrix;
  glm::vec3 eye;
  glm::vec3 at;
  glm::vec3 up;

  glm::vec2 maze_position;
  glm::vec3 spawn;
  glm::vec3 marker;

  size_t x;
  size_t z;
  size_t maze_size;

} world;


// create scene from world settings
void render(float delta_time)
{

	// clear and draw objects
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view_matrix = glm::lookAt(world.eye, world.at, world.up);

  for (size_t i = 0; i < world.objects.size(); i++) {
    world.objects.at(i)->draw(delta_time, texture_toggle,
      view_matrix, world.projection_matrix,
      world.at-world.eye);
  }

}

//////////////////////////////////
// camera functions
void move_camera_forward() {

  glm::vec3 increment = glm::vec3(0);

  float anglez = glm::acos(glm::dot(world.at - world.eye, glm::vec3(0, 0, 1))/glm::length(world.at - world.eye));
  float anglex = glm::acos(glm::dot(world.at - world.eye, glm::vec3(1, 0, 0))/glm::length(world.at - world.eye));
  if (anglez >= 0 && anglez <= glm::pi<float>()/4
                  && (maze_matrix[(world.z+1) % world.maze_size][world.x] != 1
                  && world.z != world.maze_size - 1)) {

    increment = glm::vec3(0, 0, 2);
    world.z++;
  }
  else if (anglex >= 0 && anglex <= glm::pi<float>()/4
                       && (maze_matrix[world.z][(world.x+1) % world.maze_size] != 1
                       && world.x != world.maze_size - 1)) {
    increment = glm::vec3(2, 0, 0);
    world.x++;
  }
  else if (anglex >= 3*glm::pi<float>()/4 && world.x != 0) {

    if (maze_matrix[world.z][(world.x-1) % world.maze_size] != 1) {

      world.x--;
      increment = glm::vec3(-2, 0, 0);
    }
  }
  else if (anglez >= 3*glm::pi<float>()/4 && (maze_matrix[(world.z-1) % world.maze_size][world.x] != 1
                                          && world.z != 0)) {

    world.z--;
    increment = glm::vec3(0, 0, -2);
  }

  world.eye += increment;
  world.at += increment;

  // check if under marker
  if (world.eye.xz() == world.marker.xz()) {
    world.eye = world.spawn;
    world.at = world.eye + glm::vec3(0, 0, 1);
    world.x = 0;
    world.z = 0;
  }

}


// camera rotation
void move_camera_rotate(float angle) {

  world.at = glm::rotate(world.at - world.eye, angle, glm::vec3(0.0, 1.0, 0.0))
             + world.eye;
}


//////////////////////////////////
// load texture
int load_texture(std::string path) {

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
      printf("Failed to load texture");
  }

  stbi_image_free(data);

  return texture;
}

//////////////////////////////////
// glfw callbacks
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

  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    texture_toggle = !texture_toggle;
  }

  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    move_camera_forward();
  }

  if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    move_camera_rotate(glm::pi<float>() / 90);
  }

  if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    move_camera_rotate(-glm::pi<float>() / 90);
  }

}



int main(int argc, char **argv)
{

  // grab file path
  char * maze_path = argv[1];

  //////////////////////////////////
  // create window and initialise helpers
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
  window = glfwCreateWindow(winX, winY, "Assignment 2 - maze", NULL, NULL);
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

  //////////////////////////////////
	// read in file
	std::ifstream inFile;
  inFile.open(maze_path);

  // exit if file failed to open
  if (!inFile) {
    printf("Unable to open %s\n", maze_path);
    exit(1);
  }

  // read maze size from file
  std::string line;
  std::getline(inFile, line);
  size_t maze_size = atoi(line.c_str());

  //////////////////////////////////
  // copy maze to better readable data structure
  // n by n matrix tiles as 0, wall block as 1, X as -1

  maze_matrix = new int * [maze_size]();
  for (size_t i = 0; i < maze_size; i++) {

    maze_matrix[i] = new int[maze_size]();
  }

  size_t u = 0;
  while (!inFile.eof()) {

    std::getline(inFile, line);
    std::string::iterator i;
    size_t v = 0;
    for (i = line.begin(); i < line.end(); i++) {

      if (*i == '*') {
        maze_matrix[u][v] = 1;
      }
      else if (*i == 'X') {
        maze_matrix[u][v] = -1;
      }
      else {
        maze_matrix[u][v] = 0;
      }

      v++;
    }

    u++;
  }

  inFile.close();

  // check if maze_matrix defined correctly
  for (size_t rows = 0; rows < maze_size; rows++) {
    for (size_t columns = 0; columns < maze_size; columns++) {

      printf("%2d ", maze_matrix[rows][columns]);
    }

    printf("\n");
  }

  //////////////////////////////////
  // define objects
  // marker object
  float marker_vertices[CUBE_NUM_VERTICES * VALS_PER_VERT] = {
       -1.0f, -1.0f, 1.0f ,
        1.0f, -1.0f, 1.0f ,
        1.0f,  1.0f, 1.0f ,
       -1.0f,  1.0f, 1.0f ,
       -1.0f, -1.0f, -1.0f ,
        1.0f, -1.0f, -1.0f ,
        1.0f,  1.0f, -1.0f ,
       -1.0f,  1.0f, -1.0f
  };

  unsigned int marker_indices[CUBE_NUM_TRIS * VALS_PER_VERT] = {
      0,1,2, 2,3,0,
      1,5,6, 6,2,1,
      5,4,7, 7,6,5,
      4,0,3, 3,7,4,
      3,2,6, 6,7,3,
      4,5,1, 1,0,4
  };

  float marker_texture_coords[8 * VALS_PER_TEX] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
        1, 0,
        0, 0,
        0, 1,
        1, 1
  };

  float marker_normals[8 * VALS_PER_VERT] = {
        -sqrt(2)/2, 0, sqrt(2)/2,
        sqrt(2)/2, 0, sqrt(2)/2,
        sqrt(2)/2, 0, sqrt(2)/2,
        -sqrt(2)/2, 0, sqrt(2)/2,
        -sqrt(2)/2, 0, -sqrt(2)/2,
        sqrt(2)/2, 0, -sqrt(2)/2,
        sqrt(2)/2, 0, -sqrt(2)/2,
        -sqrt(2)/2, 0, -sqrt(2)/2,
  };


  // define base
  float base_vertices[4 * VALS_PER_VERT] = {
        -1.0f, 0, 1.0f,
        1.0f, 0, 1.0f,
        -1.0f, 0, -1.0f,
        1.0f, 0, -1.0f
  };

  float base_texture_coords[8] = {
        0, 0,
        1, 0,
        0, 1,
        1, 1
  };

  unsigned int base_indices[2 * VALS_PER_VERT] = {
        0,2,1, 1,2,3
  };

  float base_normals[4 * VALS_PER_VERT] = {
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0
  };


  // define wall cube
  float wall_vertices[8 * VALS_PER_VERT] = {
       -1.0f, -1.0f, 1.0f ,
        1.0f, -1.0f, 1.0f ,
        1.0f,  1.0f, 1.0f ,
       -1.0f,  1.0f, 1.0f ,
       -1.0f, -1.0f, -1.0f ,
        1.0f, -1.0f, -1.0f ,
        1.0f,  1.0f, -1.0f ,
       -1.0f,  1.0f, -1.0f
  };

  float wall_texture_coords[8 * VALS_PER_TEX] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
        1, 0,
        0, 0,
        0, 1,
        1, 1
  };

  unsigned int wall_indices[24] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
  };

  float wall_normals[8 * VALS_PER_VERT] = {
        -sqrt(2)/2, 0, sqrt(2)/2,
        sqrt(2)/2, 0, sqrt(2)/2,
        sqrt(2)/2, 0, sqrt(2)/2,
        -sqrt(2)/2, 0, sqrt(2)/2,
        -sqrt(2)/2, 0, -sqrt(2)/2,
        sqrt(2)/2, 0, -sqrt(2)/2,
        sqrt(2)/2, 0, -sqrt(2)/2,
        -sqrt(2)/2, 0, -sqrt(2)/2,
  };


  //////////////////////////////////
  // load textures
  texture[0] = load_texture("floor.jpg");
  texture[1] = load_texture("wall.jpg");
  texture[2] = load_texture("marker.jpg");


  //////////////////////////////////
  // build maze
  for (size_t rows = 0; rows < maze_size; rows++) {
    for (size_t columns = 0; columns < maze_size; columns++) {

      if (maze_matrix[rows][columns] == 0 || maze_matrix[rows][columns] == -1) {
        object * base = new object ("default.vert", "default.frag",
          texture[0],
          base_texture_coords, 8,
          base_vertices, 12,
          base_indices, 6,
          base_normals, 4 * VALS_PER_VERT);
        base->set_model(glm::vec3(0, 0, 0), glm::vec3(0.5), glm::vec3(2*columns, 0, 2*rows));
        world.objects.push_back(base);
      }

      else if (maze_matrix[rows][columns] == 1) {
        object * wall = new object ("default.vert", "default.frag",
          texture[1],
          wall_texture_coords, 8 * VALS_PER_TEX,
          wall_vertices, 8 * VALS_PER_VERT,
          wall_indices, 24,
          wall_normals, 8 * VALS_PER_VERT);
        wall->set_model(glm::vec3(0, 0, 0), glm::vec3(0.3), glm::vec3(2*columns, 1, 2*rows));
        world.objects.push_back(wall);
      }

      if (maze_matrix[rows][columns] == -1) {
        object * marker = new object ("marker.vert", "marker.frag",
          texture[2],
          marker_texture_coords, 8,
          marker_vertices, CUBE_NUM_VERTICES * VALS_PER_VERT,
          marker_indices, CUBE_NUM_TRIS * 3,
          marker_normals, CUBE_NUM_TRIS * VALS_PER_VERT);
        marker->set_model(glm::vec3(0, 0, 0), glm::vec3(0.15), glm::vec3(2*columns, 4, 2*rows));
        world.marker = glm::vec3(2*columns, 1.5, 2*rows);

        world.objects.push_back(marker);
      }
    }
  }

  //////////////////////////////////
  // set camera
  world.eye = glm::vec3(0, 1.5, 0);
  world.at = glm::vec3(0, 1.5, 1);
  world.up = glm::vec3(0, 1, 0);
  world.spawn = world.eye;
  world.z = 0;
  world.x = 0;
  world.maze_size = maze_size;

  world.projection_matrix = glm::perspective(FOV, double(winX) / double(winY), 0.2, 100.0);

  //////////////////////////////////
  // set draw settings
  // default background colour
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  // set depth testing
  glEnable(GL_DEPTH_TEST);

  //////////////////////////////////
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

  //////////////////////////////////
  // clean up
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(0);

  for (size_t i = 0; i < world.objects.size(); i++) {
    delete world.objects[i];
  }

  return 0;
}
