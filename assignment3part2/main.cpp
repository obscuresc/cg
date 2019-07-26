/*******************************************************************************
Assignment 3 part 2
  Scene renderer

Creates a generic scene to exhibit some of the techniques covered
  in Computer Graphics


cycle lighting modes using "S"
cycle debug modes using "B"
toggle debug and lighting modes "D"

Jack Arney
1685823

24/5/18
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"

#include "external_files/inputstate.h"
#include "external_files/viewer.h"
#include "external_files/shader.hpp"

#include "external_files/stb_image.h"

#include "class_object.h"
// #include "class_skybox.h"


// insert type handling for skybox to modify render method
// localise shader handling for each object



////////////////////////////////
// world initialisation
// world structure
struct world_t {

  std::vector<object *> objects;
  std::vector<int> textures;

  // transforms
  glm::mat4 projection;
  glm::vec3 eye;
  glm::vec3 at;
  glm::vec3 up;

} world;

int debug_mode = 0;
int light_mode = 0;

int winX = 500;
int winY = 500;


// cameras
WorldObjectViewer *WorldCam;
ObjectViewer *ObjCam;
Viewer *Camera;

// eye postion
glm::vec3 cameraPos(0.0f, 0.0f, -2.0f);
float fov = (float)M_PI/3.0f;

// Data structure storing mouse input info
InputState Input;

int debug_inspect_id;
int light_shading_id;

void projection_set();

void terrain() {

  // n*n size grid
  size_t n = 10;
  std::vector<float> terrain_vertices(3*n*n);
  printf("terrain size: %lu\n", terrain_vertices.size());

  // row
  for (int i = 0; i < n; i++)
  {
    // columns
    for (int j = 0; j < n; j++)
    {
      terrain_vertices.at(3*(n*i+j)) = (float) j;
      terrain_vertices.at(3*(n*i+j)+1) = (float) 0;
      terrain_vertices.at(3*(n*i+j)+2) = (float) i;
    }
  }

  std::vector<int> terrain_indices(6*(n-1)*(n-1));
  printf("size of indices: %lu\n", terrain_indices.size());

  // for all the squares comprising base
  for (int k = 0; k < (n-1)*(n-1); k++)
  {
    int j = k % (n-1);
    int i = (k - j)/(n-1);
    printf("i: %i\tj: %i\n", i, j);

    terrain_indices.at(6*k) = i*n+j;
    terrain_indices.at(6*k+1) = i*n + 1 + j;
    terrain_indices.at(6*k+2) = i*n + n + j;
    terrain_indices.at(6*k+3) = i*n + 1 + j;
    terrain_indices.at(6*k+4) = i*n + n + 1 + j;
    terrain_indices.at(6*k+5) = i*n + n + j;
  }


  for (int i = 0; i < terrain_indices.size()/3; ++i)
  {
    printf("%i ", terrain_indices.at(3*i));
    printf("%i ", terrain_indices.at(3*i+1));
    printf("%i ", terrain_indices.at(3*i+2));
    printf("\n");
  }

  std::vector<float> terrain_normals;
  std::vector<float> terrain_texture_coords;


  object * terrain = new object("terrain", terrain_vertices, terrain_indices, terrain_normals, terrain_texture_coords);
  world.objects.push_back(terrain);
}

////////////////////////////////
// callbacks
void key_callback(GLFWwindow* window,
  int key, int scancode, int action, int mods) {

  int current_shader_id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader_id);

  if (action == GLFW_PRESS) {
    switch(key)
    {
      case GLFW_KEY_ESCAPE: // escape key pressed
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
      case '1':
        Camera = ObjCam;
        break;
      case '2':
        Camera = WorldCam;
        break;
      case 'B':
        if (current_shader_id == debug_inspect_id) {
          debug_mode = (debug_mode + 1) % 3;
          if (debug_mode == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          }
          else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          }
        }
        break;
      case 'S':
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          light_mode = (light_mode + 1) % 4;
        break;
      default:
        break;
    }
  }
}
////////////////////////////////
// set global projection
void projection_set()
{
  world.projection = glm::perspective(fov, (float) winX / winY, 0.5f, 30.0f );

  // load it to the shader program
  int current_shader;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader);

  int projection_handle = glGetUniformLocation(current_shader, "projection");
  // if (projection_handle == -1) {
  //   std::cout << "Uniform: projection_matrix is not an active uniform label\n";
  // }
  glUniformMatrix4fv(projection_handle, 1, false, glm::value_ptr(world.projection));
}

////////////////////////////////
// call backs
// reshape window
void reshape_callback( GLFWwindow *window, int x, int y )
{
  winX = x;
  winY = y;
  projection_set();
  glViewport( 0, 0, x, y );
}

// update mouse position
void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
  // Use a global data structure to store mouse info
  // We can then use it however we want
  Input.update((float)x, (float)y);
  if (Input.rMousePressed) {

    float xRot, yRot;
    Input.readDeltaAndReset( &yRot, &xRot );
    fov = fov - xRot/100;
    projection_set();
  }
}

// update mouse buttons
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    Input.rMousePressed = true;
  }
  else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    Input.rMousePressed = false;
  }
  else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    Input.lMousePressed = true;
  }
  else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    Input.lMousePressed = false;
  }
}

// error callback
static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}


////////////////////////////////
int main (int argc, char **argv)
{
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(1);
  }

  // Specify that we want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create the window and OpenGL context
  window = glfwCreateWindow(winX, winY, "Modelling and viewing", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(1);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    exit(1);
  }

  // load objects
  object * house = new object("house/House2.obj");
  world.objects.push_back(house);

  object * pine = new object("pine/PineTransp.obj");
  pine->set_translation(glm::vec3(-6.0, 0.0, 0.0));
  world.objects.push_back(pine);

  object * car = new object("car/car-n.obj");
  car->set_translation(glm::vec3(3.0, 0.0, 0.0));
  world.objects.push_back(car);

  // std::vector<std::string> face_textures (6);
  // face_textures = {"siege_rt.tga",
  //    "siege_lf.tga",
  //    "siege_up.tga",
  //    "siege_dn.tga",
  //    "siege_ft.tga",
  //    "siege_bk.tga"};
  // object * skybox = new object(face_textures);
  // world.objects.push_back(skybox);

  // terrain();





  // Set OpenGL state we need for this application.
  glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
  glEnable(GL_DEPTH_TEST);

  // set cameras
  WorldCam = new WorldObjectViewer( cameraPos );
  ObjCam = new ObjectViewer( cameraPos );
  Camera = ObjCam;

  // Define callback functions and start main loop
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_pos_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetFramebufferSizeCallback(window, reshape_callback);

  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);

  float time_start = glfwGetTime();

  while (!glfwWindowShouldClose(window))
  {
    // Update the camera, and draw the scene.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Camera->update( Input );

    glm::mat4 viewMatrix;
    viewMatrix = Camera->getViewMtx();

    float time_now = glfwGetTime();

    for (size_t i = 0; i < world.objects.size(); i++) {

      // load uniforms
      world.objects.at(i)->load_shader();
      projection_set();
      int current_shader_id;
      glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader_id);

      if (world.objects.at(i)->get_type() == 1) {

        int viewHandle = glGetUniformLocation(current_shader_id, "view");
        if (viewHandle == -1) {
          std::cout << "Uniform: view is not an active uniform label\n";
        }
        glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMatrix));

        int view_position_handle = glGetUniformLocation(light_shading_id, "view_position");

        glm::vec4 view_position = glm::vec4(0, 0, 0, 1) * glm::transpose(viewMatrix);
        glUniform4fv(view_position_handle, 1, glm::value_ptr(view_position));

        int light_mode_handle = glGetUniformLocation(current_shader_id, "light_mode");
        if (light_mode_handle == -1) {
          printf("Light_mode is not an active uniform label\n");
        }
        glUniform1i(light_mode_handle, light_mode);
      }

      // draw objects
    	world.objects.at(i)->render(time_now - time_start);
    }


    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // clean up
  for (size_t i = 0; i < world.objects.size(); i++) {
    delete world.objects[i];
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(0);

  return 0;
}
