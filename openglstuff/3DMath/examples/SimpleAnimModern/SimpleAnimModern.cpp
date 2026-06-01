/*
 * SimpleAnimModern.cpp - Version 1.4 - September 3, 2020
 *
 * Example program illustrating a simple use
 * of Modern OpenGL to render and animate a triangle strip,
 * a triangle fan and overlapping triangles in 3D.
 *
 * Author: Sam Buss
 *
 * Software accompanying POSSIBLE SECOND EDITION TO the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG2
 */

// These libraries are needed to link the program.
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")

// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "ShaderMgrSAM.h"
#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
bool check_for_opengl_errors();     // Function prototype (should really go in a header file)

// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>

// ********************
// Animation controls and state infornation
// ********************
int FlatSmoothMode = 0;	    // ==0 for rendering in smooth mode, ==1 for rendering in flat mode
double currentAngle = 0;	// Amount of clockwise rotation (in radians)

// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************

const int NumObjects = 3;
const int iTriangleFan = 0;
const int iTriangleStrip = 1;
const int iTriangles = 2;

unsigned int myVBO[NumObjects];  // a Vertex Buffer Object holds an array of data
unsigned int myVAO[NumObjects];  // a Vertex Array Object - holds info about an array of vertex data;

// We create two shader programs: each consists of a vertex shader and a fragment shader
//     One program uses smooth shading, the other flat shading.
unsigned int shaderProgramSmooth;
unsigned int shaderProgramFlat;
const unsigned int vertPos_loc = 0;   // Corresponds to "location = 0" in the verter shader definitions
const unsigned int vertColor_loc = 1; // Corresponds to "location = 1" in the verter shader definitions
const char* projMatName = "projectionMatrix";		// Name of the uniform variable projectionMatrix
int projMatLocation_smooth;				// Location of the projectionMatrix in the "smooth" shader program.
int projMatLocation_flat;					// Location of the projectionMatrix in the "flat" shader program.
const char* modelviewMatName = "modelviewMatrix";	// Name of the uniform variable modelviewMatrix
int modelviewMatLocation_smooth;			// Location of the modelviewMatrix in the "smooth" shader program.
int modelviewMatLocation_flat;				// Location of the modelviewMatrix in the "flat" shader program.

//  The Projection matrix: Controls the "camera view/field-of-view" transformation
//     Generally is the same for all objects in the scene.
LinearMapR4 theProjectionMatrix;		//  The Projection matrix: Controls the "camera/view" transformation

// A ModelView matrix controls the placement of a particular object in 3-space.
//     It is generally different for each object.
LinearMapR4 theModelViewMatrixTriFan;
LinearMapR4 theModelViewMatrixTriStrip;
LinearMapR4 theModelViewMatrixThreeTriangles;

// *****************************
// These variables set the dimensions of the rectanglar region we wish to view.
// They are used to help form the projection matrix.
const double Xmin = -2.0, Xmax = 2.0;
const double Ymin = -1.0, Ymax = 2.0;
const double Zmin = -1.0, Zmax = 1.0;

// *************************
// mySetupGeometries defines the scene data, especially vertex  positions and colors.
//    - It also loads all the data into the VAO's (Vertex Array Objects) and
//      into the VBO's (Vertex Buffer Objects).
// This routine is only called once to initialize the data.
// *************************
void mySetupGeometries() {

	// Allocate the needed Vertex Array Objects (VAO's) and Vertex Buffer Objects (VBO')
	glGenVertexArrays(NumObjects, &myVAO[0]);
	glGenBuffers(NumObjects, &myVBO[0]);

	// First Geometry : a Triangle Fan
	// Specify vertices for the triangles rendered with GL_TRIANGLE_FAN
	float triangleFanVerts[] = {
		// Positions			// Colors
		0.0f, 0.0f, 0.0f,		0.8f, 0.8f, 0.8f,   // Light grey
		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,	// Red
		0.5f, 0.866f, 0.0f,     1.0f, 1.0f, 0.0f,   // Yellow
		-0.5f, 0.866f, 0.0f,	0.0f, 1.0f, 0.0f,   // Green
		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 1.0f,	// Cyan
		-0.5f, -0.866f, 0.0f,	0.0f, 0.0f, 1.0f,	// Blue
		.5f, -0.866f, 0.0f,		1.0f, 0.0f, 1.0f,	// Magenta
		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f	// Duplicate vert and color
	};

	// Bind (and initialize) the Vertex Array Object and the Vertex Buffer Object
	//     the triangle fan.  The Vertex Buffer Object is stored on the GPU
	//     and holds data from the triangleFanVerts above.
	glBindVertexArray(myVAO[iTriangleFan]);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iTriangleFan]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleFanVerts), triangleFanVerts, GL_STATIC_DRAW);

	// Bind the Vertex Array Object and then configure vertex attributes(s).
	//   The vertices consist of three coordinates (x, y, z values)
	//   This information is stored in the VertexArrayObject
	//   vertPos_loc corresponds to the "location = 0" in the vertex shader.
	//          These positions were just loaded into the VBO as part of the above array, one per vertex.
	//  vertColor_loc corresponds to the "location = 1" in the vertex shader.
	//			These colors were also just loaded into the VBO, one color per vertex.
	// The next four commands tell the VAO where the vertex positions and color values
	//          are stored in the VBO.
	// The value "6 * sizeof(float)" is the "stride" (distance between buffer locations where values are stored.
	// The value "3 * sizeof(float)" is the offset in the VBO buffer where the first color start.
	glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);	// Vertex data is in the VBO
	glEnableVertexAttribArray(vertPos_loc);									// Enable use of the vertex data
	glVertexAttribPointer(vertColor_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vertColor_loc);

	// The model view matrix for triangle fan resizes and repositions it
	theModelViewMatrixTriFan.Set_glTranslate(1.2, 1.2, 0.0);		// Initialize to translation by (1.2, 1.2, 0.0)
	theModelViewMatrixTriFan.Mult_glScale(0.5);						// Shrink using scale factor 1/2 (multiplies on the right)

	// Second Geometry: A Triangle Strip
	// Specify eight vertices that will be rendered with GL_TRIANGLE_STRIP
	float triangleStripVerts[] = {
		// Positions			// Colors
		0.0f, 0.0f, 0.0f,		0.9f, 0.9f, 0.9f,   // Light grey
		0.5f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,	// Red
		-0.5f, 1.0f, 0.0f,		1.0f, 1.0f, 0.0f,   // Yellow
		0.5f, 2.0f, 0.0f,		0.0f, 1.0f, 0.0f,   // Green
		-0.5f, 2.0f, 0.0f,		0.0f, 1.0f, 1.0f,	// Cyan
		0.5f, 3.0f, 0.0f,		0.0f, 0.0f, 1.0f,	// Blue
		-0.5f, 3.0f, 0.0f,		1.0f, 0.0f, 1.0f,	// Magenta
		0.0f, 4.0f, 0.0f,		0.4f, 0.4f, 0.4f	// Gray
	};

	// Do the same as above, now for the vertices that form a triangle strip.
	glBindVertexArray(myVAO[iTriangleStrip]);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iTriangleStrip]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleStripVerts), triangleStripVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vertPos_loc);
	glVertexAttribPointer(vertColor_loc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vertColor_loc);

	// The model view matrix for triangle strip resizes and repositions it
	theModelViewMatrixTriStrip.Set_glTranslate(-1.5, 0.5, 0.0);			// Initialize to translation by (-1.5, 0.5, 0.0)
	theModelViewMatrixTriStrip.Mult_glScale(0.4);						// Shrink by scaling factor 0.4
	theModelViewMatrixTriStrip.Mult_glRotate(-0.785, 0.0, 0.0, 1.0);	// Rotate 45 degrees (0.785 radians) clockwise


	// Third Geometry: A collection of three triangles.
	// Specify nine vertices that will be used to form triangles.
	// The vertices are listed in counterclockwise order.
	// Will render with GL_TRIANGLES
	float trianglesVerts[] = {
		// x,y,z coordinates	// R,G,B colors
		0.7f, -0.42f, 0.0f,		1.0f, 0.8f, 0.8f, // First triangle
		0.7f, -0.18f, 0.0f,		1.0f, 0.8f, 0.8f,
		-0.7f, -0.3f, 0.5f,		1.0f, 0.0f, 0.0f,
	
		-0.25f, 0.7f, 0.0f,		0.8f, 1.0f, 0.8f, // Second triangle
		-0.40f, 0.55f, 0.0f,	0.8f, 1.0f, 0.8f,
		0.5f, -0.6f, 0.5f,		0.0f, 1.0f, 0.0f,
	
		-0.57f, -0.53f, 0.0f,	0.8f,  0.8f, 1.0f,	// Third triangle
		-0.43f, -0.67f, 0.0f,	0.8f,  0.8f, 1.0f,
		0.32f, 0.62f, 0.5f,		0.0f,  0.0f, 1.0f,
	};

	// Do the same as above, now for the three triangles.
	glBindVertexArray(myVAO[iTriangles]);
	glBindBuffer(GL_ARRAY_BUFFER, myVBO[iTriangles]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trianglesVerts), trianglesVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(vertPos_loc);
	glVertexAttribPointer(vertColor_loc, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(vertColor_loc);

	// The model view matrix for triangle strip resizes and repositions it
	theModelViewMatrixThreeTriangles.SetIdentity();

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}
// *************************************
// Main routine for rendering the scene
// myRenderScene() is called every time the scene needs to be redrawn.
// mySetupGeometries() has already created the vertex and buffer objects
//    and the model view matrices.
// setup_shaders() has already created the shader programs.
// *************************************
void myRenderScene() {

	// Clear the rendering window
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const float clearDepth = 1.0f;
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, &clearDepth);	// Must pass in a pointer to the depth value!

	// Choose the shader program to use
    int modelviewMatLocation;
	if (FlatSmoothMode == 0) {
		glUseProgram(shaderProgramSmooth);
        modelviewMatLocation = modelviewMatLocation_smooth;
	} 
	else {
		glUseProgram(shaderProgramFlat);
        modelviewMatLocation = modelviewMatLocation_flat;
    }

	// Draw Triangle Fan
	float matEntries[16];
	theModelViewMatrixTriFan.DumpByColumns(matEntries);
	glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	glBindVertexArray(myVAO[iTriangleFan]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
	
	// Draw Triangle Strip
	theModelViewMatrixTriStrip.DumpByColumns(matEntries);
	glUniformMatrix4fv(modelviewMatLocation, 1, false, matEntries);
	glBindVertexArray(myVAO[iTriangleStrip]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

	// Draw three overlapping triangles - Rotate them slightly in each re-rendering pass.
	currentAngle += 0.005;		// Increment current angle by 0.005 radians.
	if (currentAngle > 2.0*3.1415926535897932) {
		currentAngle -= 2.0*3.1415926535897932;
	}
	LinearMapR4 mat = theModelViewMatrixThreeTriangles;
	mat.Mult_glRotate(currentAngle, 0.0, 0.0, -1.0);		// Rotate around negative z-axis (clockwise for viewer)
	mat.DumpByColumns(matEntries);
	glUniformMatrix4fv(modelviewMatLocation_smooth, 1, false, matEntries);
	glBindVertexArray(myVAO[iTriangles]);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_SceneData() {
	mySetupGeometries();
	setup_shaders();

	// Get the locations of the projection and model view matrices in the shader programs.
	projMatLocation_smooth = glGetUniformLocation(shaderProgramSmooth, projMatName);
	projMatLocation_flat = glGetUniformLocation(shaderProgramFlat, projMatName);
	modelviewMatLocation_smooth = glGetUniformLocation(shaderProgramSmooth, modelviewMatName);
	modelviewMatLocation_flat = glGetUniformLocation(shaderProgramFlat, modelviewMatName);

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

// *******************************************************
// Process all key press events.
// This routine is called each time a key is pressed or released.
// *******************************************************
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		return;			// Ignore key up (key release) events
	}
	if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_X) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_SPACE) {
		FlatSmoothMode = 1 - FlatSmoothMode;	// Toggle between 0 and 1 for flat and smooth shading
	}
}


// *************************************************
// This function is called with the graphics window is first created,
//    and again whenever it is resized.
// The Projection View Matrix is typically set here.
// *************************************************
void window_size_callback(GLFWwindow* window, int width, int height) {
	// Define the portion of the window used for OpenGL rendering.
	glViewport(0, 0, width, height);

	// Setup the projection matrix as an orthographic view.
	// Determine the min and max values for x and y that should appear in the window.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	double w = (width == 0) ? 1.0 : (double)width;
	double h = (height == 0) ? 1.0 : (double)height;

    // Center point and one-half of width and height
    double centerX = (Xmax + Xmin) / 2.0;
    double centerY = (Ymax + Ymin) / 2.0;
    double halfWidth = (Xmax - Xmin) / 2.0;
    double halfHeight = (Ymax - Ymin) / 2.0;

    // Scale (increase) either width or height up to match window aspect ration
    double scale = (halfHeight * w ) / (halfWidth * h);
    if (scale > 1.0) {
        halfWidth *= scale;
    }
    else {
        halfHeight /= scale;
    }
    double windowXmin = centerX - halfWidth;
    double windowXmax = centerX + halfWidth;
    double windowYmin = centerX - halfHeight;
    double windowYmax = centerY + halfHeight;
 
	// Using the max & min values for x & y & z that should be visible in the window,
	//		we set up the orthographic projection.
	theProjectionMatrix.Set_glOrtho(windowXmin, windowXmax, windowYmin, windowYmax, Zmin, Zmax);

	float matEntries[16];
	theProjectionMatrix.DumpByColumns(matEntries);
	if (glIsProgram(shaderProgramSmooth)) {
		glUseProgram(shaderProgramSmooth);
		glUniformMatrix4fv(projMatLocation_smooth, 1, false, matEntries);
	}
	if (glIsProgram(shaderProgramFlat)) {
		glUseProgram(shaderProgramFlat);
		glUniformMatrix4fv(projMatLocation_flat, 1, false, matEntries);
	}
	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_OpenGL() {
	
	glEnable(GL_DEPTH_TEST);	// Enable depth buffering
	glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders

	// Set polygon drawing mode for front and back of each triangle
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable backface culling to render both sides of triangles
	// glDisable(GL_CULL_FACE);
	
	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks(GLFWwindow* window) {
	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	// Set callback for key up/down/repeat events
	glfwSetKeyCallback(window, key_callback);

	// Set callbacks for mouse movement (cursor position) and mouse botton up/down events.
	// glfwSetCursorPosCallback(window, cursor_pos_callback);
	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}

int main() {
	glfwSetErrorCallback(error_callback);	// Supposed to be called in event of errors. (doesn't work?)
	glfwInit();
#if defined(__APPLE__) || defined(__linux__)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);        // Invoke Multisample Antialiasing (MSAA)
    glEnable(GL_MULTISAMPLE);               // Usually is enabled by default, but call just in case.

	const int initWidth = 800;
	const int initHeight = 600;
	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, "SimpleAnimModern", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return -1;
	}

	// Print info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
#ifdef GL_SHADING_LANGUAGE_VERSION
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
    printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));

	printf("------------------------------\n");
	printf("Press space bar to toggle between flat shading and smooth shading.\n");
	printf("Press ESCAPE or 'X' or 'x' to exit.\n");
	
    setup_callbacks(window);
   
	// Initialize OpenGL, the scene and the shaders
    my_setup_OpenGL();
	my_setup_SceneData();
 	window_size_callback(window, initWidth, initHeight);

    // Loop while program is not terminated.
	while (!glfwWindowShouldClose(window)) {
	
		myRenderScene();				// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		// Poll events (key presses, mouse events)
		glfwWaitEventsTimeout(1.0/60.0);	    // Use this to animate at 60 frames/sec (timing is NOT reliable)
		// glfwWaitEvents();					// Use this if no animation.
		// glfwPollEvents();					// Use this version when animating as fast as possible
	}

	glfwTerminate();
	return 0;
}

// If an error is found, it could have been caused by any command since the
//   previous call to check_for_opengl_errors()
// To find what generated the error, you can try adding more calls to
//   check_for_opengl_errors().
char errNames[9][36] = {
    "Unknown OpenGL error",
    "GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION",
    "GL_INVALID_FRAMEBUFFER_OPERATION", "GL_OUT_OF_MEMORY",
    "GL_STACK_UNDERFLOW", "GL_STACK_OVERFLOW", "GL_CONTEXT_LOST" };
bool check_for_opengl_errors() {
    int numErrors = 0;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        numErrors++;
        int errNum = 0;
        switch (err) {
        case GL_INVALID_ENUM:
            errNum = 1;
            break;
        case GL_INVALID_VALUE:
            errNum = 2;
            break;
        case GL_INVALID_OPERATION:
            errNum = 3;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errNum = 4;
            break;
        case GL_OUT_OF_MEMORY:
            errNum = 5;
            break;
        case GL_STACK_UNDERFLOW:
            errNum = 6;
            break;
        case GL_STACK_OVERFLOW:
            errNum = 7;
            break;
        case GL_CONTEXT_LOST:
            errNum = 8;
            break;
        }
        printf("OpenGL ERROR: %s.\n", errNames[errNum]);
    }
    return (numErrors != 0);
}