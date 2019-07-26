/*******************************************************************************
Model viewer

Uses model viewer code from course examples, stb_image, tinyobjloader
to load *.obj and draw using data provided by obj format

'1' - camera rotates about camera axes
'2' - camera rotates about world axes

cycle lighting modes using "S"
cycle debug modes using "B"
toggle debug and lighting modes "D"

Jack Arney
1685823

30/4/18
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"

#include "inputstate.h"
#include "viewer.h"
#include "table.h"
#include "shader.hpp"

#include "class_object.h"

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


////////////////////////////////
// swap shaders and load new uniforms
void shader_swap() {

  int current_shader;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader);

  if (current_shader == light_shading_id) {

    // use debug shader
    glUseProgram(debug_inspect_id);
    if (debug_mode == 0) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  }
  else if (current_shader == debug_inspect_id) {

    // use light shader
    glUseProgram(light_shading_id);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
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
      case 'D':
        shader_swap();
        break;
      case 'S':
        if (current_shader_id == light_shading_id) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          light_mode = (light_mode + 1) % 4;
        }
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
  world.projection = glm::perspective(fov, (float) winX / winY, 1.0f, 30.0f );

  // load it to the shader program
  int current_shader;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader);

  int projection_handle = glGetUniformLocation(current_shader, "projection");
  if (projection_handle == -1) {
    std::cout << "Uniform: projection_matrix is not an active uniform label\n";
  }
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

  if (argc != 2) {
    printf("Incorrent number of parameters\n");
    return -1;
  }

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

  object * house = new object(argv[1]);
  world.objects.push_back(house);

  // load shaders
  debug_inspect_id = LoadShaders("debug_inspect.vert", "debug_inspect.frag");
  if (debug_inspect_id == 0) {
    printf("Debug_inspect shaders could not be found.\n");
    exit(1);
  }

  light_shading_id = LoadShaders("light_shading.vert", "light_shading.frag");
  if (light_shading_id == 0) {
    printf("Light_shading shaders could not be found.\n");
    exit(1);
  }

  // Set OpenGL state we need for this application.
  glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
  glEnable(GL_DEPTH_TEST);

  glUseProgram(debug_inspect_id);
  projection_set();

  glUseProgram(light_shading_id);
  projection_set();


  // set cameras
  WorldCam = new WorldObjectViewer( cameraPos );
  ObjCam = new ObjectViewer( cameraPos );
  Camera = ObjCam;

  // Define callback functions and start main loop
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_pos_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetFramebufferSizeCallback(window, reshape_callback);

  float time_start = glfwGetTime();

  while (!glfwWindowShouldClose(window))
  {

    // Update the camera, and draw the scene.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Camera->update( Input );

    // glUseProgram(debug_inspect_id);

    // First load the viewing matrix from the current camera
    glm::mat4 viewMatrix;
    viewMatrix = Camera->getViewMtx();

    // Load it to the shader program
    int current_shader_id;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader_id);
    int viewHandle = glGetUniformLocation(current_shader_id, "view");
    if (viewHandle == -1) {
      std::cout << "Uniform: view is not an active uniform label\n";
    }
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMatrix));

    int view_position_handle = glGetUniformLocation(light_shading_id, "view_position");

    glm::vec4 view_position = glm::vec4(0, 0, 0, 1) * glm::transpose(viewMatrix);
    glUniform4fv(view_position_handle, 1, glm::value_ptr(view_position));

    int debug_mode_handle = glGetUniformLocation(debug_inspect_id, "debug_mode");
    if(debug_mode_handle == -1) {
      printf("Debug_mode is not an active uniform label\n");
    }
    glUniform1i(debug_mode_handle, debug_mode);


    int light_mode_handle = glGetUniformLocation(light_shading_id, "light_mode");
    if(light_mode_handle == -1) {
      printf("Debug_mode is not an active uniform label\n");
    }
    glUniform1i(light_mode_handle, light_mode);

    float time_now = glfwGetTime();

    for (size_t i = 0; i < world.objects.size(); i++) {
    	world.objects.at(i)->render(current_shader_id, time_now - time_start);
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
