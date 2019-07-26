#include "polygon_class.h"

#define POINTS_PER_ELEMENT 3
#define ELEMENT_DUPLICATES 2
#define VALS_PER_VERT 3
#define THICKNESS_RATIO 0.7


// constructor, takes number of sides as parameter
polygon::polygon(int x, std::string vertex_shader, std::string fragment_shader) {

  sides = x;
  vertices_size = POINTS_PER_ELEMENT * ELEMENT_DUPLICATES * sides * sizeof(float);
  vertices = new float[POINTS_PER_ELEMENT * ELEMENT_DUPLICATES * sides]();

  // create vertices
  float base_angle = 2*glm::pi<float>()/sides;
  for (size_t i = 0; i < sides; i++) {
    vertices[3*i] = glm::cos((i % sides)*base_angle);
    vertices[3*i + 1] = glm::sin((i % sides)*base_angle);
    vertices[3*i + 2] = 0;
  }

  for (size_t i = sides; i < 2*sides; i++) {
    vertices[3*i] = THICKNESS_RATIO* glm::cos((i % sides)*base_angle);
    vertices[3*i + 1] = THICKNESS_RATIO * glm::sin((i % sides)*base_angle);
    vertices[3*i + 2] = 0;
  }

  for (size_t i = 0; i < 2*sides; i++) {
    printf("%f\t%f\t%f\n", vertices[3*i], vertices[3*i+1], vertices[3*i+2]);
  }
  printf("%s\n", "-------");

  // indices to define elements
  size_t indices_size = 3 * 2 * sides * sizeof(unsigned int);
  indices = new unsigned int[3*2*sides]();

  // first index of element
  indices[0] = 0;
  for (size_t i = 1; i < sides; i++) {
    indices[6*i - 3] = i;
    indices[6*i] = i;
  }

  // iterate through elements in mesh
  for (size_t i = 0; i < 2*sides; i++) {

    if (!(i % 2)) {
      indices[3*i + 1] = (indices[3*i] + 1) % sides;    // second
    }
    else {
      indices[3*i + 1] = indices[3*i] + sides - 1;
    }
    indices[3*i+2] = indices[3*i] + sides;              // last
  }

  indices[3*2*sides - 2] += sides;                       // account for wrap around

  for (size_t i = 0; i < 2*sides; i++) {
    printf("%i\t%i\t%i\n", indices[3*i], indices[3*i+1], indices[3*i+2]);
  }
  printf("%s\n", "-------");

  // create vao and load with buffers
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // buffer[0] vertices, buffer[1] indices
  glGenBuffers(2, buffer);

  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

  // tidy up buffers
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // load shaders
  default_shader = LoadShaders(vertex_shader.c_str(), fragment_shader.c_str());
  if (default_shader == 0)
  exit(1);

  // clean up dynamic memory
  delete [] vertices;
  delete [] indices;
}

// draw method for object
void polygon::draw(float delta_time) {

  // specify shader to use
  glUseProgram(default_shader);

  // use vao
  glBindVertexArray(vao);

  int time_handle = glGetUniformLocation(default_shader, "time");
  if (time_handle != -1) {
    glUniform1f(time_handle, delta_time);
  }

  // model transformation matrix
  int model_handle = glGetUniformLocation(default_shader, "model");
  if (model_handle == -1) {
    exit(1);
  }

  // apply transformations to object
  glm::mat4 transform_matrix;

  float rotation = delta_time * glm::pi<float>()/2;
  transform_matrix = glm::translate(transform_matrix, translation);
  transform_matrix = glm::rotate(transform_matrix, rotation, spin_axis);
  transform_matrix = glm::scale(transform_matrix, scale);

  // send transformations to shaders
  glUniformMatrix4fv(model_handle, 1, false, glm::value_ptr(transform_matrix));
  transform_matrix = glm::mat4();

  // Send command to GPU to draw the data in the current VAO as triangles
  glDrawElements(GL_TRIANGLES, VALS_PER_VERT * ELEMENT_DUPLICATES * sides, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);	// Un-bind the VAO

  glFlush();	// Guarantees previous commands have been completed before continuing
}

// location definition of object
void polygon::set_position(glm::vec3 in_rotation, glm::vec3 in_scale, glm::vec3 in_translation) {

  rotation = in_rotation;
  scale = in_scale;
  translation = in_translation;
}

unsigned int polygon::get_shader() {

  return default_shader;
}

void polygon::set_spin_axis(glm::vec3 in_spin_axis) {

  spin_axis = in_spin_axis;
}
