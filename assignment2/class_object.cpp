#include "class_object.h"

#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define THICKNESS_RATIO 0.7
#define NUMBER_OF_PRIMITIVE_POINTS 4


// constructor
object::object(std::string vertex_shader, std::string fragment_shader,
  unsigned int in_texture,
  float * in_texture_coords, size_t in_texture_size,
  float * in_vertices, size_t in_vertices_size,
  unsigned int * in_indices, size_t in_indices_size,
  float * normals, size_t normals_size) {

  // grab vertices
  vertices_size = in_vertices_size;
  vertices = new float[vertices_size]();

  for (size_t i = 0; i < in_vertices_size; i++) {
    vertices[i] = in_vertices[i];
  }

  // indices to define elements
  indices_size = in_indices_size;
  indices = new unsigned int[indices_size]();

  for (size_t i = 0; i < indices_size; i++) {
    indices[i] = in_indices[i];
  }

  // grab in_texture
  texture = in_texture;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // buffer[0] vertices, buffer[1] indices
  // buffer[2] texture_coords, buffer[3] light
  glGenBuffers(4, buffer);

  glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, vertices_size * sizeof(float), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(unsigned int), indices, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
  glBufferData(GL_ARRAY_BUFFER, in_texture_size * sizeof(float), in_texture_coords, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
  glBufferData(GL_ARRAY_BUFFER, normals_size * sizeof(float), normals, GL_STATIC_DRAW);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  default_shader = LoadShaders(vertex_shader.c_str(), fragment_shader.c_str());
  if (default_shader == 0)
  exit(1);

  delete [] vertices;
  delete [] indices;
}

void object::draw(float delta_time, unsigned int texture_toggle,
  glm::mat4 view_matrix, glm::mat4 projection_matrix,
  glm::vec3 light_source) {

  // specify shader to use
  glUseProgram(default_shader);

  // use vao
  glBindVertexArray(vao);

  int light_source_handle = glGetUniformLocation(default_shader, "light_position");
  glUniform3fv(light_source_handle, 3, glm::value_ptr(light_source));


  int texture_toggle_handle = glGetUniformLocation(default_shader, "texture_toggle");

  glUniform1ui(texture_toggle_handle, texture_toggle);

  // model transformation matrix
  int model_handle = glGetUniformLocation(default_shader, "model");
  if (model_handle == -1) {
    printf("Model matrix couldn't be located in shader program\n");
    exit(1);
  }

  // apply transformations to object
  glm::mat4 transform_matrix = glm::mat4(1.0);


  // transform_matrix = glm::scale(transform_matrix, scale);
  // transform_matrix = glm::rotate(transform_matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  // transform_matrix = glm::rotate(transform_matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  // transform_matrix = glm::rotate(transform_matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  transform_matrix = glm::translate(transform_matrix, translation);

  // send transformations to shaders
  glUniformMatrix4fv(model_handle, 1, false, glm::value_ptr(transform_matrix));
  transform_matrix = glm::mat4();

  // view transformation matrix
  int view_handle = glGetUniformLocation(default_shader, "view");
  if (view_handle == -1) {
    printf("View matrix couldn't be located in shader program\n");
    exit(1);
  }

  // send view to shaders
  glUniformMatrix4fv(view_handle, 1, false, glm::value_ptr(view_matrix));

  // projection transformation matrix
  int projection_handle = glGetUniformLocation(default_shader, "projection");
  if (projection_handle == -1) {
    printf("Projection matrix couldn't be located in shader program\n");
    exit(1);
  }

  // send view to shaders
  glUniformMatrix4fv(projection_handle, 1, false, glm::value_ptr(projection_matrix));



  int texture_handle = glGetUniformLocation(default_shader, "texture_map");
  if (texture_handle == -1) {
    printf("Texture map couldn't be located in shader program\n");
    exit(1);
  }

  glBindTexture(GL_TEXTURE_2D, texture);


  // Send command to GPU to draw the data in the current VAO as triangles
  glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);	// Un-bind the VAO

  glFlush();	// Guarantees previous commands have been completed before continuing
}

void object::set_model(glm::vec3 in_rotation, glm::vec3 in_scale, glm::vec3 in_translation) {

  rotation = in_rotation;
  scale = in_scale;
  translation = in_translation;
}

unsigned int object::get_shader() {

  return default_shader;
}
