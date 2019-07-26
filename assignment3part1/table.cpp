
#include "table.h"

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define CUBE_NUM_VERTICES 8
#define CUBE_NUM_TRIS 12
#define VALS_PER_VERT 3

unsigned int vaoHandle;

/**
 * Creates a new vertex array object for a cube
 * and loads in data into a vertex attribute buffer
 */
int createVAO(int programID)
{
    // Cube vertex data
    float Vertices[ CUBE_NUM_VERTICES * VALS_PER_VERT ] = {
        -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0 };

    // 12 triangles - 2 per face of the cube
    unsigned int Indices[CUBE_NUM_TRIS*3] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };

    glUseProgram(programID);
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	unsigned int buffer[2];
	glGenBuffers(2, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

 	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vaoHandle;
}



/**
 Constructor - calculates the table's dimensions based on the
 desired height and width.
*/
Table::Table( float width, float length, int id)
{
    tableX = width;
    tableY = 0.25;
    tableZ = length;

    legX = width / 8;
    legY = width / 2;
    legZ = width / 8;

    vaoHandle = createVAO(id);
}

/**
 Draw the table as a set of scaled blocks.
*/
void Table::render(int programID)
{
	// glUseProgram(programID);

	int modelUniformHandle = glGetUniformLocation(programID, "model");
	if (modelUniformHandle == -1)
		exit(1);

 	glBindVertexArray(vaoHandle);

    // We have separated modelling from viewing.
    // Viewing (i.e. placing the camera relative to the entire table)
    // is handled in the viewer class.
    glm::mat4 model;

    // now apply the scales to each cube forming the table
    // First the table top
	model = glm::scale( model, glm::vec3(tableX, tableY, tableZ) );
	glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

    // leg 1
    model = glm::mat4(1.0f);
    model = glm::translate( model, glm::vec3(tableX-legX, -tableY-legY, tableZ-legZ) );
    model = glm::scale( model, glm::vec3( legX, legY, legZ ) );
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

    // leg 2
    model = glm::mat4(1.0f);
    model = glm::translate( model, glm::vec3(-(tableX-legX), -tableY-legY, tableZ-legZ) );
    model = glm::scale( model, glm::vec3( legX, legY, legZ ) );
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

    // leg 3
    model = glm::mat4(1.0f);
    model = glm::translate( model, glm::vec3(tableX-legX, -tableY-legY, -(tableZ-legZ) ) );
    model = glm::scale( model, glm::vec3( legX, legY, legZ ) );
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

    // leg 4
    model = glm::mat4(1.0f);
    model = glm::translate( model, glm::vec3(-(tableX-legX), -tableY-legY, -(tableZ-legZ) ));
    model = glm::scale( model, glm::vec3(legX, legY, legZ ));
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glFlush();

}
