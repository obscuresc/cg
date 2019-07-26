// Polygon class defines initialisation methods for polygon mesh
//  and loading procedure to graphics memory

#ifndef POLYGON_CLASS_H
#define POLYGON_CLASS_H

#include <cstdio>
#include <sys/types.h>
#include <GL/glew.h>        // helps with opengl extensions
#include <GLFW/glfw3.h>     // windowing api
#include <glm/glm.hpp>      // maths
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include <string>

// class will need capability for multi component handling
//	ie multiple vao and vbos within, shaders for each vbo
// 	eg car, wheels shader for tyre, shader for rim
// 	wheel is contained within vao, each aspect in vbo
// class constructor defines vertex positions at unit maximal radius
class polygon {

  private:

		// mesh
    int sides;
    float * vertices;
    unsigned int * indices;

    size_t vertices_size;
    size_t indices_size;

		// mesh memory allocation
    unsigned int vao;
    unsigned int buffer[2];
    unsigned int default_shader;

    // position
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 translation;

    // dynamic motion axis
    glm::vec3 spin_axis;

		// shader programs
		std::string vertex_shader;
		std::string fragment_shader;


  public:

    polygon(int, std::string, std::string);
    void draw(float);

    void set_position(glm::vec3, glm::vec3, glm::vec3);

    unsigned int get_shader();

    void set_spin_axis(glm::vec3);

};


#endif
