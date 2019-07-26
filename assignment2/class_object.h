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

// class will need capability for multi component handling
//	ie multiple vao and vbos within, shaders for each vbo
// 	eg car, wheels shader for tyre, shader for rim
// 	wheel is contained within vao, each aspect in vbo
// class constructor defines vertex positions at unit maximal radius


class object {

  private:

    // mesh
    int sides;
    float * vertices;
    unsigned int * indices;

    // size in number of elements
    size_t vertices_size;
    size_t indices_size;

		// mesh memory allocation
    unsigned int vao;
    unsigned int buffer[4];
    unsigned int default_shader;

    // position
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 translation;

		// shader programs
		std::string vertex_shader;
		std::string fragment_shader;

    unsigned int texture;


  public:

    object(std::string, std::string,
      unsigned int,
      float *, size_t,
      float *, size_t,
      unsigned int *, size_t,
      float *, size_t);

    void draw(float, unsigned int, glm::mat4, glm::mat4, glm::vec3);

    void set_model(glm::vec3, glm::vec3, glm::vec3);

    unsigned int get_shader();
};


#endif
