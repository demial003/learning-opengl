/*
* GlGeomTeapot.cpp - Version 1.1 - November 13, 2020
*
* C++ class for rendering a version of the Utah teapot in Modern OpenGL.
*   A GlGeomTeapot object encapsulates a VAO, a VBO, and an EBO,
*   which can be used to render the teapot.
*   The u- and v-mesh resolution of the patches can be varied.
*   It is build on top of GlGeomBezier
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
* Freeglut code is also "as-is" without any warranty.
* Bug reports: Sam Buss, sbuss@ucsd.edu.
* Web page: http://math.ucsd.edu/~sbuss/MathCG2
*/

#pragma once

#ifndef GLGEOM_TEAPOT_H
#define GLGEOM_TEAPOT_H

#include <assert.h>

#include "GlGeomBezier.h"

// GlGeomTeapot
//     Builds and renders a version of the Utah teapot, including rim and base.
//     This uses a total of 32 degree 4 Bezier patches.
//     Teapot is rendered with y-axis as upward direction, and
//          spout pointing in the direction of the z-axis.
// Supports:
//    (1) Allocating and loading a VAO, VBO, and EBO
//    (2) Rendering the teapot with OpenGL.
// How to use:
//     * First call the constructor GlGeomTeapot() and possibly ReMesh()
//             to set the numbers of mesh resolutions.
//             These numbers can be changed by calling ReMesh().
//     * Then call InitializeAttribLocations() to specify whether to use
//          normals and texture coordinates and to specify
//          the locations in the VBO buffer for the shader program.
//          The also allocates and loads the VAO, VBO and EBO.
//     * Call Render() to render all the entire teapot
//     * The routines Render(...) issues the the glDrawElements commands 
//       for the Bezier patch(es) using the VAO, VBO and EBO.

class GlGeomTeapot : public GlGeomBezier
{
public:
    GlGeomTeapot() : GlGeomTeapot(8, 8) {}
    GlGeomTeapot(int uMeshResolution, int vMeshResolution);
    ~GlGeomTeapot();

    // Disable all copy and assignment operators for a GlGeomTeapot object.
    //     If you need to pass it to/from a function, use references or pointers
    //     and be sure that there are no implicit copy or assignment operations!
    GlGeomTeapot(const GlGeomTeapot&) = delete;
    GlGeomTeapot& operator=(const GlGeomTeapot&) = delete;
    GlGeomTeapot(GlGeomTeapot&&) = delete;
    GlGeomTeapot& operator=(GlGeomTeapot&&) = delete;

    // Remesh: re-mesh to change the mesh resolution.
    // Can be called either before or after InitializeAttribLocations(), but it is
    //    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution;
    //    and InitializeAttribLocations() is called afterwards.
    void Remesh(int uMeshResolution, int vMeshResolution) { GlGeomBezier::Remesh(uMeshResolution,vMeshResolution); }

    // Allocate the VAO, VBO, and EBO.
    // Set up info about the Vertex Attribute Locations
    // This must be called before Render() or other render function.
    // First parameter is the location for the vertex position vector in the shader program.
    // Second parameter is the location for the vertex normal vector in the shader program.
    // Third parameter is the location for the vertex 2D texture coordinates in the shader program.
    // The second and third parameters are optional (use UINT_MAX to omit).
    void InitializeAttribLocations(
        unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

    // Render the entire teapot.  Must call InitializeAttribLocations first.
    void Render() {  GlGeomBezier::Render(); }
    void RenderBody() { GlGeomBezier::RenderPatches(0, 16); }
    void RenderLid() { GlGeomBezier::RenderPatches(16, 8); }
    void RenderHandle() { GlGeomBezier::RenderPatches(24, 4); }
    void RenderSpout() { GlGeomBezier::RenderPatches(28, 4); }

private:
    void LoadTeapotControlPoints();
    void CalcTeapotControlPoints();

    static double* teapotControlPts;

};

inline GlGeomTeapot::GlGeomTeapot(int uMeshResolution, int vMeshResolution)
{
    Remesh(uMeshResolution, vMeshResolution);
}


inline GlGeomTeapot::~GlGeomTeapot() { delete teapotControlPts; }

#endif // GLGEOM_TEAPOT_H

