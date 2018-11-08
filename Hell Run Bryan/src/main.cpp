#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "gl_utils.h"
#include "tools.h"
#include "malla.h"
#include "suelo.h"


#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void input(GLFWwindow *window);

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 5.0f, 22.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

//variables
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  45.0f;
float x=0.0f;
float t=0.0f;
float z=-96.0f;
float z1=0.0f; 
float z2=144.0f;
float ztrampa=-4.0f;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int tramp=0;//funciona como booleano
malla *brayan;

void init(){
	restart_gl_log ();
	start_gl ();
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor (0.2, 0.8, 0.8, 1.0); // grey background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height);
	glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
	
}


void dibujar(malla *e1){
	e1->draw();
}

void MovimientoPista(malla *pista,float posicion){
	pista->setpos(glm::vec3(0.0f, -1.0f, posicion));
}
void MovimientoTrampa(malla *trampa,float posicion){
	trampa->setpos(glm::vec3(0.0f, -2.0f, posicion));
}
float cambioPista(float posicion){
	if(posicion<-144.0f){ //cambio de la pista 1 a posicion inicial
		posicion=144.0f;
  	}
	return posicion;
}

void moverBryan(){
	brayan->setpos(glm::vec3(x, -1.0f, 0.0f));
}

int main(){
	init();

/*-------------------------------CREATE SHADERS-------------------------------*/
	GLuint shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);
	brayan = new malla((char*)"mallas/personaje.obj", shader_programme);
	brayan->setpos(glm::vec3(0.0f, -1.0f, 0.0f));

	malla *pista1 = new malla((char*)"mallas/piso1.obj", shader_programme);
	pista1->setpos(glm::vec3(0.0f, -1.0f, z1));
	malla *pista2 = new malla((char*)"mallas/piso1.obj", shader_programme);
	pista2->setpos(glm::vec3(0.0f, -1.0f, z2));

	malla *roca1 = new malla((char*)"mallas/roca1.obj", shader_programme);
	roca1->setpos(glm::vec3(-22.0f, -8.0f, -32.0f));
	malla *roca2 = new malla((char*)"mallas/roca2.obj", shader_programme);
	roca2->setpos(glm::vec3(22.0f, -4.0f, -32.0f));
	suelo *suel = new suelo((char*)"mallas/pisoLava.obj", shader_programme);
	suel->setpos(glm::vec3(0.0f, -4.0f, -12.0f));

	malla *godrox = new malla((char*)"mallas/Godrox.obj", shader_programme);
	godrox->setpos(glm::vec3(0.0f, -2.0f, z));
	
	malla *trampa = new malla((char*)"mallas/trampa1.obj", shader_programme);
	trampa->setpos(glm::vec3(0.0f, -2.0f, ztrampa));

	//x=derecha o izquierda,y=altura,z=profundidad

	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	glUseProgram (shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
	glUseProgram (shader_programme);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(g_window)){
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		input(g_window);
		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// activate shader
		glUseProgram (shader_programme);

		// pass projection matrix to shader (note that in this case it could change every frame)
		projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
		glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);

		// camera/view transformation
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
		z+=0.05f; //velocidad en z
		z1-=0.1f; //posicion en z1 pista1
		z2-=0.1f; //posicion en z2 pista2
     		MovimientoPista(pista1,z1);//se mueven las pistas infinitamente
		MovimientoPista(pista2,z2);
		z1=cambioPista(z1);//cambio de la pista 1 a posicion inicial
		z2=cambioPista(z2);//cambio de la pista 2 a posicion inicial
		ztrampa-=0.1f;//se mueve la trampa con la pista
		if(tramp>0){
			MovimientoTrampa(trampa,ztrampa);
			dibujar(trampa);
		}
		if(z<-3.0){ //godrox avanza hasta posicionarse detras del brayan
			if(x<4 && x>-4){
				godrox->setpos(glm::vec3(x+0.5f, -2.0f, z));
				t=x+0.5f; //variable t para que avance godrox cuando este en los bordes de la pista
			}
			else{
				godrox->setpos(glm::vec3(t, -2.0f, z));
			}			
			//hoz(){
				//} metodo a implementar para que godrox realice un ataque con la hoz
		}
		
		suel->draw();
		dibujar(brayan);
		dibujar(pista1);
		dibujar(pista2);
		dibujar(roca1);
		dibujar(roca2);
		dibujar(godrox);
		
		//dibujar agujeros y caja de trampas y trampas por implementar

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}




// metodo input que interacciona con el teclado
// ---------------------------------------------------------------------------------------------------------
void input(GLFWwindow *window){
	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey (g_window, GLFW_KEY_A) && x>-7 && z<-3.0) { //permite mover a brayan a la izquierda hasta que godrox lo alcance
		moverBryan();
		x-=0.05;
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;//para que la camara se quede con brayan
	}
	if (glfwGetKey (g_window, GLFW_KEY_D) && x<7 && z<-3.0) { //permite mover a brayan a la izquierda hasta que godrox lo alcance
		moverBryan();
		x+=0.05;
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
    	if (glfwGetKey (g_window, GLFW_KEY_S)) { //activa trampa
		tramp=1;				
		ztrampa=z+4;
	}
    	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
       	 	glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

