/*
* GlGeomCube.h - Version 1.4 - July 24, 2023
*
* Principal Author: Derrick Yao
* Updated by: Sam Buss
* Written in the GlGeomShape environment provided by Sam Buss
*
* C++ class for rendering cubes in Modern OpenGL.
*   A GlGeomCube object encapsulates a VAO, a VBO, and an EBO,
*   which can be used to render a cube.
*   The resolution of the faces can be varied.
*
* Software accompanying POSSIBLE SECOND EDITION TO the book
*		3D Computer Graphics: A Mathematical Introduction with OpenGL,
*		by S. Buss, Cambridge University Press, 2003.
*
* Software is "as-is" and carries no warranty.  It may be used without
*   restriction, but if you modify it, please change the filenames to
*   prevent confusion between different versions.
* Bug reports: Sam Buss, sbuss@ucsd.edu and Derrick Yao
* Web page: http://math.ucsd.edu/~sbuss/MathCG2
*/

#pragma once
#ifndef GLGEOM_CUBE_H
#define GLGEOM_CUBE_H

#include "GlGeomBase.h"

// GlGeomCube
//     Generates vertices, normals, and texture coodinates for a cube.
//     Cube formed of six faces, with each face a grid of "squares"
//     Cube has side length 1 and is centered at the origin
//	       Vertices are located at +-0.5 on each axis
//	   Textures range from (0,0) to (1,1) on each face
//     Textures are upright on the side faces
//	   Texture on top points away from viewer, and on bottom towards viewer
// Supports:
//    (1) Allocating and loading a VAO, VBO, and EBO
//    (2) Rendering cube  with OpenGL
//    (3) Specialty routine to render only one face
// How to use:
//     * First call either the constructor or Remesh()
//             to set the dimension of the grid of squares.
//             These numbers can be changed later by calling Remesh().
//    * Then call InitializeAttribLocations() to
//            give locations in the VBO buffer for the shader program.
//            This loads all the vertex data into the VBO and EBO.
//    * Call Render() - to render the cube.  This gives the glDrawElements 
//            commands for the cone using the VAO, VBO and EBO.

class GlGeomCube : public GlGeomBase
{
public:
	GlGeomCube() : GlGeomCube(1) {}
	GlGeomCube(int meshResolution);

	// Remesh() - Re-mesh to change the number of squares.
	// Can be called either before or after InitializeAttribLocations(), but it is
	//    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution.
	void Remesh(int meshResolution);

	// Allocate the VAO, VBO, and EBO.
	// Set up info about the Vertex Attribute Locations
	// This must be called before Render() is first called.
	// First parameter is the location for the vertex position vector in the shader program.
	// Second parameter is the location for the vertex normal vector in the shader program.
	// Third parameter is the location for the vertex 2D texture coordinates in the shader program.
	// The second and third parameters are optional.
	void InitializeAttribLocations(
		unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

	void Render();		// Render: renders entire cube

	void RenderFace(int face);	// RenderFace: renders one face
								// face #: 0-front, 1-right, 2-back, 3-left, 4-top, 5-bottom
								// "front" means +z; "right" means +x; "top" means +y

	int GetMeshResolution() const { return meshRes; }

	// Use GetNumElements() and GetNumVerticesTexCoords() and GetNumVerticesNoTexCoords()
	//    to determine the amount of data that will returned by CalcVboAndEbo.
	//    For cubes, inclusion of texture coordinates does not affect number of vertices.
	int GetNumElements() const { return GetNumElementsSide() * 6; }
	int GetNumVerticesTexCoords() const { return GetNumVerticesSideTexCoords() * 6; }
	int GetNumVerticesNoTexCoords() const { return GetNumVerticesTexCoords(); }

	// Get the amount of data items for a single face
	int GetNumElementsSide() const { return meshRes * meshRes * 6; }
	int GetNumVerticesSideTexCoords() const { return (meshRes + 1) * (meshRes + 1); }
	int GetNumVerticesSideNoTexCoords() const { return GetNumVerticesSideTexCoords(); }


	// CalcVboAndEbo- return all VBO vertex information, and EBO elements for GL_TRIANGLES drawing.
	// See GlGeomBase.h for additional information
	void CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
		int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset,
		unsigned int stride);


private:
	// Disable all copy and assignment operators.
	// A GlGeomCube can be allocated as a global or static variable, or with new.
	//     If you need to pass it to/from a function, use references or pointers
	//     and be sure that there are no implicit copy or assignment operations!
	GlGeomCube(const GlGeomCube&) = delete;
	GlGeomCube& operator=(const GlGeomCube&) = delete;
	GlGeomCube(GlGeomCube&&) = delete;
	GlGeomCube& operator=(GlGeomCube&&) = delete;
private:
	int meshRes; // The number squares along a side

private:
	bool VboEboLoaded = false;

	void PreRender();
	void InsertCoords(float coords_x, float coords_y, int face, float* posPtr);
};

// Constructor
inline GlGeomCube::GlGeomCube(int meshResolution)
{
	meshRes = meshResolution;
}
#endif // GLGEOM_CUBE_H