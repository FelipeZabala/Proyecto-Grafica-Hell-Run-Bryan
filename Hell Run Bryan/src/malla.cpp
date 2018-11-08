#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tools.h"
#include "malla.h"

using namespace std;
malla::malla(char* filename, int shaderprog){
    	this->setpos(glm::vec3(0,0,0));
	this->filename = filename;
	this->shaderprog = shaderprog;
	assert(load_mesh(filename, &vao, &numvertices));
	model_location = glGetUniformLocation(shaderprog, "model");

}

GLuint malla::getvao(){
    return this->vao;
}

int malla::getnumvertices(){
    return this->numvertices;
}

glm::vec3 malla::getpos(){
	return this->pos;
}

char* malla::getfilename(){
    	return this->filename;
}

void malla::setpos(glm::vec3 p){
   	model = glm::translate(glm::mat4(1.0f), p); 
}

void malla::draw(){
	glUseProgram(shaderprog);
        glBindVertexArray(this->getvao());
        glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES,0,this->getnumvertices());
        glBindVertexArray(0);
}
