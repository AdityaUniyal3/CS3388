#define GLM_ENABLE_EXPERIMENTAL

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

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

#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <string>
#include <sstream>


#include "CamControls.hpp"
#include "TriTable.hpp"
#include "shaderSource.hpp"

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
};

GLuint VAO;
GLuint VBO;
GLuint VBO_positions;
GLuint VBO_indices;
GLuint shaderProgram;



void readPLY(const std::string& filename, std::vector<Vertex>& vertices, std::vector<int>& indices) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    // Skip header
    while (std::getline(file, line)) {
        if (line == "end_header") {
            break;
        }
    }

    // Read vertices
    int vertexCount = 0;
    while (std::getline(file, line) && vertexCount < vertices.size()) {
        std::istringstream iss(line);
        Vertex vertex;
        if (!(iss >> vertex.x >> vertex.y >> vertex.z >> vertex.nx >> vertex.ny >> vertex.nz)) {
            std::cerr << "Error: Invalid vertex data format in file " << filename << std::endl;
            return;
        }
        vertices.push_back(vertex);
        ++vertexCount;
    }

    // Read faces
    int faceCount = 0;
    while (std::getline(file, line) && faceCount < indices.size() / 3) {
        std::istringstream iss(line);
        int count;
        iss >> count;  // Read the number of vertices in the face
        if (count != 3) {
            std::cerr << "Error: Only triangular faces are supported in file " << filename << std::endl;
            return;
        }
        int index1, index2, index3;
        iss >> index1 >> index2 >> index3;
        indices.push_back(index1);
        indices.push_back(index2);
        indices.push_back(index3);
        ++faceCount;
    }

    file.close();
}









GLuint compileShaderProgram() {
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
    }

  

    // Create shader program
    GLuint shaderProgram = glCreateProgram();

      // Bind attribute locations
    glBindAttribLocation(shaderProgram, 0, "aPos");

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }


    return shaderProgram;
}

void bindVBOs(std::vector<Vertex> vertices, std::vector<int> indices){

    
    // Generate VBOs for positions, texture coordinates, and indices
        glGenBuffers(1, &VBO_positions);
        glGenBuffers(1, &VBO_indices);

        // Bind VBO for positions and upload data
        glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, x));
        glEnableVertexAttribArray(0);

      
        // Bind VBO for indices and upload data (you're already doing this correctly)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);


       // Unbind VAO
        glBindVertexArray(0);


}

std::vector<float> marching_cubes(float (*f)(float, float, float), float isovalue,float min, float max, float stepsize) {
    std::vector<float> vertices;

    // Iterate over the grid
    for (float x = min; x < max; x += stepsize) {
        for (float y = min; y < max; y += stepsize) {
            for (float z = min; z < max; z += stepsize) {
                // Calculate scalar field values at cube vertices
                float values[8];
                values[0] = f(x, y, z);
                values[1] = f(x + stepsize, y, z);
                values[2] = f(x + stepsize, y, z + stepsize);
                values[3] = f(x, y, z + stepsize);
                values[4] = f(x, y + stepsize, z);
                values[5] = f(x + stepsize, y + stepsize, z);
                values[6] = f(x + stepsize, y + stepsize, z + stepsize);
                values[7] = f(x, y + stepsize, z + stepsize);

                // Determine the cube configuration
                int cubeIndex = 0;
                if (values[0] < isovalue) cubeIndex |= 1;
                if (values[1] < isovalue) cubeIndex |= 2;
                if (values[2] < isovalue) cubeIndex |= 4;
                if (values[3] < isovalue) cubeIndex |= 8;
                if (values[4] < isovalue) cubeIndex |= 16;
                if (values[5] < isovalue) cubeIndex |= 32;
                if (values[6] < isovalue) cubeIndex |= 64;
                if (values[7] < isovalue) cubeIndex |= 128;

                // Use the lookup table to determine the vertices and generate triangles
                int* edgeTable = marching_cubes_lut[cubeIndex];
                if (edgeTable[0] == -1) continue;  // No triangles for this configuration

                for (int i = 0; edgeTable[i] != -1; i += 3) {
                    // Generate vertices along the edges of the cube
                    float v1[3] = {x + stepsize * vertexOffset[edgeTable[i]][0],
                                   y + stepsize * vertexOffset[edgeTable[i]][1],
                                   z + stepsize * vertexOffset[edgeTable[i]][2]};
                    float v2[3] = {x + stepsize * vertexOffset[edgeTable[i + 1]][0],
                                   y + stepsize * vertexOffset[edgeTable[i + 1]][1],
                                   z + stepsize * vertexOffset[edgeTable[i + 1]][2]};
                    float v3[3] = {x + stepsize * vertexOffset[edgeTable[i + 2]][0],
                                   y + stepsize * vertexOffset[edgeTable[i + 2]][1],
                                   z + stepsize * vertexOffset[edgeTable[i + 2]][2]};

                    // Add vertices to the list
                    vertices.push_back(v1[0]);
                    vertices.push_back(v1[1]);
                    vertices.push_back(v1[2]);
                    
                    vertices.push_back(v2[0]);
                    vertices.push_back(v2[1]);
                    vertices.push_back(v2[2]);
                    
                    vertices.push_back(v3[0]);
                    vertices.push_back(v3[1]);
                    vertices.push_back(v3[2]);
                }
            }
        }
    }

    return vertices;
}

// Function to compute normals for each vertex
std::vector<float> compute_normals(const std::vector<float>& vertices) {
    std::vector<float> normals;

    // Iterate over each triangle (every 3 vertices)
    for (size_t i = 0; i < vertices.size(); i += 9) {
        // Get three vertices of the triangle
        float v1x = vertices[i];
        float v1y = vertices[i + 1];
        float v1z = vertices[i + 2];

        float v2x = vertices[i + 3];
        float v2y = vertices[i + 4];
        float v2z = vertices[i + 5];

        float v3x = vertices[i + 6];
        float v3y = vertices[i + 7];
        float v3z = vertices[i + 8];

        // Calculate the edge vectors of the triangle
        float e1x = v2x - v1x;
        float e1y = v2y - v1y;
        float e1z = v2z - v1z;

        float e2x = v3x - v1x;
        float e2y = v3y - v1y;
        float e2z = v3z - v1z;

        // Calculate the cross product of the edge vectors to get the normal vector
        float nx = e1y * e2z - e1z * e2y;
        float ny = e1z * e2x - e1x * e2z;
        float nz = e1x * e2y - e1y * e2x;

        // Normalize the normal vector
        float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        nx /= length;
        ny /= length;
        nz /= length;

        // Repeat the normal vector three times
        for (int j = 0; j < 3; ++j) {
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
    }

    return normals;
}


// Function to write vertices and normals to a PLY file
void writePLY(const std::vector<float>& vertices, const std::vector<float>& normals, const std::string& fileName) {
    std::ofstream outFile(fileName);

    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file " << fileName << " for writing." << std::endl;
        return;
    }

    // Write PLY header
    outFile << "ply" << std::endl;
    outFile << "format ascii 1.0" << std::endl;
    outFile << "element vertex " << vertices.size() / 3 << std::endl;
    outFile << "property float x" << std::endl;
    outFile << "property float y" << std::endl;
    outFile << "property float z" << std::endl;
    outFile << "property float nx" << std::endl;
    outFile << "property float ny" << std::endl;
    outFile << "property float nz" << std::endl;
    outFile << "element face " << vertices.size() / 9 << std::endl;
    outFile << "property list uchar int vertex_indices" << std::endl;
    outFile << "end_header" << std::endl;

    // Write vertices and normals
    for (size_t i = 0; i < vertices.size(); i += 3) {
        outFile << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << " "
                << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << std::endl;
    }

    // Write faces
    for (size_t i = 0; i < vertices.size() / 9; ++i) {
        outFile << "3 " << i * 3 << " " << i * 3 + 1 << " " << i * 3 + 2 << std::endl;
    }

    outFile.close();
}

// Example scalar field function
float example_function(float x, float y, float z) {
    return x * x + y * y + z * z - 1.0; // Sphere equation
}

// Example scalar field function representing a torus
float torus_function(float x, float y, float z) {
    float major_radius = 1.0;  // Major radius of the torus
    float minor_radius = 0.3;  // Minor radius of the torus
    
    float dist_to_ring = sqrt(x * x + y * y) - major_radius;
    return dist_to_ring * dist_to_ring + z * z - minor_radius * minor_radius;
}



// scalar function that represents y − sin(x)cos(z)
float custom_function(float x, float y, float z) {
    return y - sin(x) * cos(z);
}


// scalar function that represents x2−y2−z2−z
float custom_function2(float x, float y, float z) {
    return x * x - y * y - z * z - z;
}


// function to diplay scalae function that represents y − sin(x)cos(z)

void display_function() {


    std::vector<float> vertices = marching_cubes(custom_function, 0, -5.0f, 5.0f, 0.1f);

    glColor3f(0.0, 1.0, 1.0); // Cyan color (lighter blue)


    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();

}


// function to diplay scalae function that represents  x2−y2−z2−z


void  () {


    std::vector<float> vertices = marching_cubes(custom_function2, 0, -5.0f, 5.0f, 0.1f);

    glColor3f(0.0, 1.0, 1.0); // Cyan color (lighter blue)


    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    glEnd();

}

void drawBox() {

    // Define the min and max corners of the marching volume
    float min_corner[3] = {-5, -5, -5};
    float max_corner[3] = {5, 5, 5};

    glLineWidth(2.0);
    glColor3f(1.0, 1.0, 1.0);  // Set color to white
    glBegin(GL_LINES);
    // Bottom rectangle
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], min_corner[1], max_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);

    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);

    // Top rectangle
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], max_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], max_corner[2]);

    glVertex3f(max_corner[0], max_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], max_corner[2]);

    glVertex3f(min_corner[0], max_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);

    // Connect the corresponding corners
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], max_corner[2]);

    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], max_corner[2]);

    glEnd();
}

void drawAxes() {

    // Define the min and max corners of the marching volume
    float min_corner[3] = {-5, -5, -5};
    float max_corner[3] = {5, 5, 5};

    glLineWidth(12.0);
    glBegin(GL_LINES);
    glBegin(GL_LINES);
    // X-axis (Red)
    glColor3f(1.0, 0.0, 0.0); // Red color
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0] + 1, min_corner[1], min_corner[2]);
    // Arrowhead
    glVertex3f(max_corner[0] + 1, min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0] + 0.7, min_corner[1] + 0.3, min_corner[2]);
    glVertex3f(max_corner[0] + 1, min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0] + 0.7, min_corner[1] - 0.3, min_corner[2]);

    // Y-axis (Green)
    glColor3f(0.0, 1.0, 0.0); // Green color
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(min_corner[0], max_corner[1] + 1, min_corner[2]);
    // Arrowhead
    glVertex3f(min_corner[0], max_corner[1] + 1, min_corner[2]);
    glVertex3f(min_corner[0] + 0.3, max_corner[1] + 0.7, min_corner[2]);
    glVertex3f(min_corner[0], max_corner[1] + 1, min_corner[2]);
    glVertex3f(min_corner[0] - 0.3, max_corner[1] + 0.7, min_corner[2]);

    // Z-axis (Blue)
    glColor3f(0.0, 0.0, 1.0); // Blue color
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(min_corner[0], min_corner[1], max_corner[2] + 1);
    // Arrowhead
    glVertex3f(min_corner[0], min_corner[1], max_corner[2] + 1);
    glVertex3f(min_corner[0], min_corner[1] + 0.3, max_corner[2] + 0.7);
    glVertex3f(min_corner[0], min_corner[1], max_corner[2] + 1);
    glVertex3f(min_corner[0], min_corner[1] - 0.3, max_corner[2] + 0.7);

    glEnd();
}


//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[])
{

	///////////////////////////////////////////////////////
	float screenW = 1400;
	float screenH = 900;
	float stepsize = 0.1f;

	
	if (argc > 1 ) {
		screenW = atoi(argv[1]);
	}
	if (argc > 2) {
		screenH = atoi(argv[2]);
	}
	if (argc > 3) {
		stepsize = atof(argv[3]);
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
	window = glfwCreateWindow( screenW, screenH, "Marching Cubes", NULL, NULL);
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
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	// Projection = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW/screenH, 0.001f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(Projection));

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glm::vec3 eye (0.0f, 3.0f, 5.0f);
	// glm::vec3 eye = {-5.0f, 2.0f, -5.0f};
	glm::vec3 up (0.0f, 1.0f, 0.0f);
	glm::vec3 center (0.0f, 0.0f, 0.0f);
	//glm::mat4 V;

	glm::mat4 V = glm::lookAt(eye, center, up);
	glLoadMatrixf(glm::value_ptr(V));


	


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);



    std::vector<float> vertices = marching_cubes(custom_function, 0.0f, -5.0f, 5.0f, 0.1f);
    std::vector<float> normals = compute_normals(vertices);

    writePLY(vertices, normals, "scalar_1.ply");

    std::vector<float> vertices2 = marching_cubes(custom_function2, 0.0f, -5.0f, 5.0f, 0.1f);
    std::vector<float> normals2 = compute_normals(vertices);

    writePLY(vertices2, normals2, "scalar_2.ply");


    std::vector<Vertex> vertices_ply;
    std::vector<int> indices;
    readPLY("example.ply", vertices_ply, indices);

    bindVBOs(vertices_ply, indices);

   // Compile shader program
    shaderProgram = compileShaderProgram();

    // Bind the shader program
    //glUseProgram(shaderProgram);

    
    glEnable(GL_DEPTH_TEST);


	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
     
        drawAxes();
        drawBox();
        display_function2();
       

		// glBegin(GL_TRIANGLES);
		// glColor3f(1.0, 0.0, 0.0);  // Red
		// glVertex2f(-0.5, -0.5);    // Bottom left corner
		// glColor3f(0.0, 1.0, 0.0);  // Green
		// glVertex2f(0.5, -0.5);     // Bottom right corner
		// glColor3f(0.0, 0.0, 1.0);  // Blue
		// glVertex2f(0.0, 0.5);      // Top corner
		// glEnd();

		cameraFirstPerson(window, V, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(V));

        // Bind texture unit 0 before drawing each textured mesh
    //     glActiveTexture(GL_TEXTURE0);

    //       // Bind the shader program
    //     glUseProgram(shaderProgram);

    //     // Set uniform for view matrix
    //     GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    //     glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(V));

    //    // Bind VAO and draw
    //     glBindVertexArray(VAO);
    //     glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //     glBindVertexArray(0);



		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	} 
    
    
    // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );


      // Delete VAO and VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

