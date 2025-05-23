#define GLM_ENABLE_EXPERIMENTAL
// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <vector>
#include <iostream>

//Nope. You'll have to do this yourself for assignment 5
//#include "CamControls.hpp"
void cameraControlsGlobe(glm::mat4& V) {

}

//////////////////////////////////////////////////////////////////////////////
// Vertex Data
//////////////////////////////////////////////////////////////////////////////

static const GLfloat g_vertex_color_buffer_data[] = {
    /* position */      /* color */ 
	0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f, // red
	1.0f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // green
    -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // blue
};

static const GLuint indices[] = {
    0, 1, 2 // Define the triangle using indices of vertices[]
};


//////////////////////////////////////////////////////////////////////////////
// Helper Objects
//////////////////////////////////////////////////////////////////////////////


class Plane {

public:
	enum PLANE_WHICH {
		x,
		y,
		z
	};

private:
	PLANE_WHICH plane = PLANE_WHICH::x;

	glm::vec4 color = glm::vec4(0.9f, 0.9f, 0.9f, 0.1f);

	GLfloat size;

public:



	Plane(GLfloat sz) : size(sz) {} 

	void draw() {

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (plane == PLANE_WHICH::x) {
		glBegin(GL_QUADS);

		glColor4f(color.x, color.y, color.z, color.w);
			glVertex3f(-size, 0.0f, -size);
			glVertex3f(size, 0.0f, -size);
			glVertex3f(size, 0.0f, size);
			glVertex3f(-size, 0.0f, size);
		
		glEnd();

		glBegin(GL_LINES);
		glColor4f(color.x, color.y, color.z, color.w+0.2f);

			for (int i = -size; i < size; ++i) {
				glVertex3f(1.0f*i, 0.0f, -size);				
				glVertex3f(1.0f*i, 0.0f, size);	
				glVertex3f(size, 0.0f, 1.0f*i);				
				glVertex3f(-size, 0.0f, 1.0f*i);				
			}
		glEnd();

		}


		glPopMatrix();
		glDisable(GL_BLEND);
	}

};

class Axes {

	glm::vec3 origin;
	glm::vec3 extents;

	glm::vec3 xcol = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 ycol = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 zcol = glm::vec3(0.0f, 0.0f, 1.0f);

public:

	Axes(glm::vec3 orig, glm::vec3 ex) : origin(orig), extents(ex) {}

	void draw() {

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();


		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glColor3f(xcol.x, xcol.y, xcol.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x + extents.x, origin.y, origin.z);

		glVertex3f(origin.x + extents.x, origin.y, origin.z);
		glVertex3f(origin.x + extents.x, origin.y, origin.z+0.1);
		glVertex3f(origin.x + extents.x, origin.y, origin.z);
		glVertex3f(origin.x + extents.x, origin.y, origin.z-0.1);

		glColor3f(ycol.x, ycol.y, ycol.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y + extents.y, origin.z);

		glVertex3f(origin.x, origin.y + extents.y, origin.z);
		glVertex3f(origin.x, origin.y + extents.y, origin.z+0.1);
		glVertex3f(origin.x, origin.y + extents.y, origin.z);
		glVertex3f(origin.x, origin.y + extents.y, origin.z-0.1);
		
		glColor3f(zcol.x, zcol.y, zcol.z);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x, origin.y, origin.z + extents.z);
		
		glVertex3f(origin.x, origin.y, origin.z + extents.z);
		glVertex3f(origin.x+0.1, origin.y, origin.z + extents.z);

		glVertex3f(origin.x, origin.y, origin.z + extents.z);
		glVertex3f(origin.x-0.1, origin.y, origin.z + extents.z);
		glEnd();


		glPopMatrix();
	}

};




//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[])
{

	///////////////////////////////////////////////////////
	int currentStep = 1;
	int substep = 1;
	if (argc > 1 ) {
		currentStep = atoi(argv[1]);
	}
	if (argc > 2) {
		substep = atoi(argv[2]);
	}
	///////////////////////////////////////////////////////

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	float screenW = 1400;
	float screenH = 900;
	window = glfwCreateWindow( screenW, screenH, "An Example", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.2f, 0.2f, 0.3f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    std::string VertexShaderCode = "\
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 vertexPosition;\n\
		layout(location = 1) in vec3 color;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec3 color_out;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 MVP;\n\
		void main(){ \n\
			// Output position of the vertex, in clip space : MVP * position\n\
			gl_Position =  MVP * vec4(vertexPosition,1);\n\
			// The color will be interpolated to produce the color of each fragment\n\
			color_out = color;\n\
		}\n";

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode = "\
		#version 330 core\n\
		in vec3 color_out; \n\
		void main() {\n\
			gl_FragColor = vec4(color_out, 1.0f);\n\
		}\n";
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Link the program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");

    // === FIX: VAO + VBO + EBO SETUP ===
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);  // Generate Element Buffer Object

    // Bind VAO (stores attribute state)
    glBindVertexArray(VAO);

    // Bind VBO and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_color_buffer_data), g_vertex_color_buffer_data, GL_STATIC_DRAW);

    // Bind EBO and upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position Attribute (Layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute (Layout 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Unbind VAO (Prevents accidental modification)
    glBindVertexArray(0);

	glm::mat4 MVP;

	Axes ax(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f));
	Plane plane(5.0f);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW/screenH, 0.001f, 1000.0f);
		// Projection = glm::mat4(1.0f);
		glLoadMatrixf(glm::value_ptr(Projection));

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glm::vec3 eye = {5.0f, 2.0f, 5.0f};
		// glm::vec3 eye = {-5.0f, 2.0f, -5.0f};
		glm::vec3 up = {0.0f, 1.0f, 0.0f};
		glm::vec3 center = {0.0f, 0.0f, 0.0f};
		//glm::mat4 V;

		glm::mat4 V = glm::lookAt(eye, center, up); 

		glm::mat4 M = glm::mat4(1.0f);

		glm::mat4 MV = V * M;
		glLoadMatrixf(glm::value_ptr(V));

		MVP = Projection * V * M;

		glUseProgram(ProgramID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Enable position attribute (layout location = 0 in shader)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,                   // Attribute index (matches `layout(location = 0)` in shader)
            3,                   // Number of components per vertex (x, y, z)
            GL_FLOAT,            // Type of each component
            GL_FALSE,            // Normalize?
            6 * sizeof(GLfloat), // Stride (each vertex has 6 floats)
            (void*)0             // Offset: starts at the beginning
        );

        // Enable color attribute (layout location = 1 in shader)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,                              // Attribute index (matches `layout(location = 1)` in shader)
            3,                              // Number of components per color (r, g, b)
            GL_FLOAT,                       // Type of each component
            GL_FALSE,                       // Normalize?
            6 * sizeof(GLfloat),            // Stride (each vertex has 6 floats)
            (void*)(3 * sizeof(GLfloat))    // Offset: skip first 3 floats (position)
        );

        glBindVertexArray(VAO);

        // Draw the triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

		ax.draw();
		plane.draw();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    

	// Cleanup shader
	glDeleteProgram(ProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
