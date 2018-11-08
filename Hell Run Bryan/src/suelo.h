#ifndef SUELO_H
#define SUELO_H

using namespace std;

class suelo{
    private:
        GLuint vao, vbo;
        int numvertices;
        glm::vec3 pos;
	glm::mat4 model;

        char* filename;
	int model_location;
	int shaderprog;


    public:
        suelo(char *filename, int shaderprog);

        // gets
        GLuint getvao();
        int getnumvertices();
        glm::vec3 getpos();
        char* getfilename();
	int get_model_location();

        // sets
        void setvao(GLuint vao);
        void setnumvertices(int num);
        void setpos(glm::vec3 p);
        void setfilename(char *f);
	void draw();
};

#endif
