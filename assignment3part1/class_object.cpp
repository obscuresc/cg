#include "class_object.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define VALS_PER_VERT 3
#define VALS_PER_ELEMENT 3
#define VALS_PER_NORM 3
#define VALS_PER_TEX 2

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define NUM_TEXTURES 3
#define TEX_SIZE 64


// constructor
object::object(std::string obj_path) {

  printf("#########################\n");
  std::string name = obj_path.substr(obj_path.find_last_of("\\/") + 1);
  std::string dir_path = obj_path.substr(0, obj_path.find_last_of("\\/")) + "/";

  // load object
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;


  int loadflag = LoadObj(shapes, materials, err,
                         obj_path.c_str(), dir_path.c_str());
  if (loadflag) {
    printf("%s imported successfully\n", name.c_str());
  }
  else {
    printf("%s\n", err.c_str());
    exit(0);
  }

  // set normals to zero if not in file
  for (int i = 0; i < shapes.size(); i++)
  {
    if (shapes.at(i).mesh.normals.size() == 0) {
      shapes.at(i).mesh.normals.resize(shapes.at(i).mesh.positions.size(), 0);
    }
  }

  // for (size_t i = 0; i < materials.size(); i++) {
  //
  //   // printf("------------------\n");
  //   // printf("%s\n", materials.at(i).name.c_str());
  //   // printf("ambient colour (%f, %f, %f)\n", materials.at(i).ambient[0],
  //   //                                         materials.at(i).ambient[1],
  //   //                                       materials.at(i).ambient[2]);
  //   // printf("diffuse colour (%f, %f, %f)\n", materials.at(i).diffuse[0],
  //   //                                         materials.at(i).diffuse[1],
  //   //                                       materials.at(i).diffuse[2]);
  //   // printf("%s\n", materials.at(i).ambient_texname.c_str());
  //   // printf("%s\n", materials.at(i).diffuse_texname.c_str());
  //   // printf("%s\n", materials.at(i).specular_texname.c_str());
  //   // printf("%s\n", materials.at(i).specular_highlight_texname.c_str()); // map_Ns
  //   // printf("%s\n", materials.at(i).bump_texname.c_str());               // map_bump, bump
  //   // printf("%s\n", materials.at(i).displacement_texname.c_str());       // disp
  //   // printf("%s\n", materials.at(i).alpha_texname.c_str());
  // }
  // printf("\n");

  ////////////////////////////////
  // normalise factor and centering
  float maxX;
  float minX;
  float maxY;
  float minY;
  float maxZ;
  float minZ;

  for (size_t i = 0; i < shapes.size(); i++) {
    for (size_t j = 0; j < shapes.at(i).mesh.positions.size()/3; j++) {
      if (i != 0 && j != 0) {
        if (maxX < shapes.at(i).mesh.positions.at(3*j)) {
          maxX = shapes.at(i).mesh.positions.at(3*j);
        }
        if (minX > shapes.at(i).mesh.positions.at(3*j)) {
          minX = shapes.at(i).mesh.positions.at(3*j);
        }
        if (maxY < shapes.at(i).mesh.positions.at(3*j+1)) {
          maxY = shapes.at(i).mesh.positions.at(3*j+1);
        }
        if (minY > shapes.at(i).mesh.positions.at(3*j+1)) {
          minY = shapes.at(i).mesh.positions.at(3*j+1);
        }
        if (maxZ < shapes.at(i).mesh.positions.at(3*j+2)) {
          maxZ = shapes.at(i).mesh.positions.at(3*j+2);
        }
        if (minZ > shapes.at(i).mesh.positions.at(3*j+2)) {
          minZ = shapes.at(i).mesh.positions.at(3*j+2);
        }
      }
      else {
        maxX = shapes.at(i).mesh.positions.at(3*j);
        minX = shapes.at(i).mesh.positions.at(3*j);
        maxY = shapes.at(i).mesh.positions.at(3*j+1);
        minY = shapes.at(i).mesh.positions.at(3*j+1);
        maxZ = shapes.at(i).mesh.positions.at(3*j+2);
        minZ = shapes.at(i).mesh.positions.at(3*j+2);
      }
    }
  }

  // pythagoras normalisation
  normalise_factor = 0;
  for (size_t i = 0; i < shapes.size(); i++) {
    for (size_t j = 0; j < shapes.at(i).mesh.positions.size()/3; j++) {

      float normalise_factor_temp = pow(shapes.at(i).mesh.positions.at(3*j), 2) +
        pow(shapes.at(i).mesh.positions.at(3*j+1), 2) +
        pow(shapes.at(i).mesh.positions.at(3*j+2), 2);

      if (normalise_factor_temp > normalise_factor) {
        normalise_factor = normalise_factor_temp;
      }
    }
  }

  // scaling and translation
  normalise_factor = sqrt(normalise_factor);
  obj_centre = glm::vec3(-1*(maxX+minX)/2, -1*(maxY+minY)/2, -1*(maxZ+minZ)/2);


  printf("minX: %f\tmaxX: %f\n", minX, maxX);
  printf("minY: %f\tmaxY: %f\n", minY, maxY);
  printf("minZ: %f\tmaxZ: %f\n", minZ, maxZ);
  printf("normalise_factor: %f\n\n", normalise_factor);

  ////////////////////////////////
  // parameters used for rendering
  shapes_buffer = new object_shape [shapes.size()];
  shapes_size = shapes.size();

  ////////////////////////////////
  // load to hardware and object
  for (size_t i = 0; i < shapes.size(); i++) {

    shapes_buffer[i].indices_size = shapes.at(i).mesh.indices.size();

    // assumes that each shape has only one material
    shapes_buffer[i].material_id = shapes.at(i).mesh.material_ids.at(0);

    glGenVertexArrays(1, &shapes_buffer[i].vao);
    glBindVertexArray(shapes_buffer[i].vao);

    glGenBuffers(4, shapes_buffer[i].vbo);

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, shapes_buffer[i].vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
                  shapes.at(i).mesh.positions.size() * sizeof(GL_FLOAT),
                  &shapes.at(i).mesh.positions.at(0),
                  GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // element indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes_buffer[i].vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                  shapes.at(i).mesh.indices.size() * sizeof(GL_UNSIGNED_INT),
                  &shapes.at(i).mesh.indices.at(0),
                  GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);

    // normals
    glBindBuffer(GL_ARRAY_BUFFER, shapes_buffer[i].vbo[2]);
    glBufferData(GL_ARRAY_BUFFER,
                  shapes.at(i).mesh.normals.size() * sizeof(GL_FLOAT),
                  &shapes.at(i).mesh.normals.at(0),
                  GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    // testing normalising of normals
    glVertexAttribPointer(2, VALS_PER_NORM, GL_FLOAT, GL_TRUE, 0, 0);
    // glVertexAttribPointer(2, VALS_PER_NORM, GL_FLOAT, GL_FALSE, 0, 0);

    // texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, shapes_buffer[i].vbo[3]);
    glBufferData(GL_ARRAY_BUFFER,
                  shapes.at(i).mesh.texcoords.size() * sizeof(GL_FLOAT),
                  &shapes.at(i).mesh.texcoords.at(0),
                  GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

  }

  // load textures
  materials_size = materials.size();
  materials_buffer = new object_material[materials_size];
  for (size_t i = 0; i < materials_size; i++) {

    materials_buffer[i].diffuse_colour.x = materials.at(i).diffuse[0];
    materials_buffer[i].diffuse_colour.y = materials.at(i).diffuse[1];
    materials_buffer[i].diffuse_colour.z = materials.at(i).diffuse[2];

    materials_buffer[i].ambient[0] = materials.at(i).ambient[0];
    materials_buffer[i].ambient[1] = materials.at(i).ambient[1];
    materials_buffer[i].ambient[2] = materials.at(i).ambient[2];
    materials_buffer[i].diffuse[0] = materials.at(i).diffuse[0];
    materials_buffer[i].diffuse[1] = materials.at(i).diffuse[1];
    materials_buffer[i].diffuse[2] = materials.at(i).diffuse[2];
    materials_buffer[i].specular[0] = materials.at(i).specular[0];
    materials_buffer[i].specular[1] = materials.at(i).specular[1];
    materials_buffer[i].specular[2] = materials.at(i).specular[2];
    materials_buffer[i].shininess = materials.at(i).shininess;

    glGenTextures(1, &materials_buffer[i].diffuse_texture_id);
    glBindTexture(GL_TEXTURE_2D, materials_buffer[i].diffuse_texture_id);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load((dir_path + materials.at(i).diffuse_texname).c_str(),
                                    &width, &height, &nrChannels, 0);

    if (data) {
      if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
      }
      else if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
      }
    }
    else {
        printf("Failed to load texture");
    }

    stbi_image_free(data);
  }

  // unbind vbos/vao
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("%s fully loaded to graphics hardware.\n", name.c_str());
  printf("#########################\n");
}

void object::render(int current_shader_id, float time_delta) {

  // model transformation matrix
  int model_handle = glGetUniformLocation(current_shader_id, "model");
  if (model_handle == -1) {
    printf("Model matrix couldn't be located in shader program\n");
    exit(1);
  }

  glm::mat4 model;
  model = glm::scale(model, glm::vec3(1.0/normalise_factor));
  model = glm::translate(model, obj_centre);
  glUniformMatrix4fv(model_handle, 1, false, glm::value_ptr(model));

  int time_handle = glGetUniformLocation(current_shader_id, "time");
  glUniform1f(time_handle, time_delta);

  // diffuse colour
  int diffuse_colour_handle = glGetUniformLocation(current_shader_id, "diffuse_colour");

  // draw each shape
	for (size_t i = 0; i < shapes_size; i++) {

    glBindVertexArray(shapes_buffer[i].vao);
		glBindBuffer(GL_ARRAY_BUFFER, shapes_buffer[i].vbo[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes_buffer[i].vbo[1]);

    glBindTexture(GL_TEXTURE_2D,
        materials_buffer[shapes_buffer[i].material_id].diffuse_texture_id);

    // load material data
    int ambient_handle = glGetUniformLocation(current_shader_id, "ambient");
    int diffuse_handle = glGetUniformLocation(current_shader_id, "diffuse");
    int specular_handle = glGetUniformLocation(current_shader_id, "specular");
    int shininess_handle = glGetUniformLocation(current_shader_id, "shininess");

    glUniform3fv(ambient_handle, 1, materials_buffer[shapes_buffer[i].material_id].ambient);
    glUniform3fv(diffuse_handle, 1, materials_buffer[shapes_buffer[i].material_id].diffuse);
    glUniform3fv(specular_handle, 1, materials_buffer[shapes_buffer[i].material_id].specular);
    glUniform1i(shininess_handle, materials_buffer[shapes_buffer[i].material_id].shininess);

    if (diffuse_colour_handle != -1) {
      glUniform3fv(diffuse_colour_handle, 1,
        glm::value_ptr(materials_buffer[shapes_buffer[i].material_id].diffuse_colour));
    }

		// draw elements comprising shape.at(i)
		glDrawElements(GL_TRIANGLES, shapes_buffer[i].indices_size, GL_UNSIGNED_INT, 0);

	}

  glBindVertexArray(0);
  glFlush();
}

void object::set_model(glm::vec3 in_rotation, glm::vec3 in_scale, glm::vec3 in_translation) {

  rotation = in_rotation;
  scale = in_scale;
  translation = in_translation;
}
