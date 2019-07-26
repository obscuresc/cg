// Polygon class defines initialisation methods for polygon mesh
//  and loading procedure to graphics memory

#ifndef CLASS_OBJECT_H
#define CLASS_OBJECT_H

#include <cstdio>
#include <sys/types.h>
#include <fstream>
#include <GL/glew.h>        // helps with opengl extensions
#include <GLFW/glfw3.h>     // windowing api
#include <glm/glm.hpp>      // maths
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include <string>
#include <vector>



struct object_shape {

    unsigned int vao;
    unsigned int vbo[4];
    unsigned int indices_size;

    unsigned int material_id;
};

struct object_material {

    unsigned int diffuse_texture_id;
    glm::vec3 diffuse_colour;

    float ambient[3];
    float diffuse[3];
    float specular[3];
    float transmittance[3];
    float emission[3];
    float shininess;
};

class object {

  private:

    // mesh
    unsigned int shapes_size;
    object_shape * shapes_buffer;
    unsigned int default_shader;

    // materials
    unsigned int materials_size;
    object_material * materials_buffer;


    // normalise scale and centre
    float normalise_factor;
    glm::vec3 obj_centre;

    // transformations
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 translation;

  public:

    object(std::string);
    void render(int, float);
    void set_model(glm::vec3, glm::vec3, glm::vec3);
};


#endif
