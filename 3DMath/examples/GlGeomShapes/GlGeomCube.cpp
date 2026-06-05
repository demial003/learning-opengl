/*
* GlGeomCube.cpp - Version 1.4 - July 24, 2023
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
#define CUBE_DEBUG false

 // Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "GlGeomCube.h"
#include "MathMisc.h"
#include "assert.h"

#if CUBE_DEBUG
#include <stdio.h>
#endif

void GlGeomCube::Remesh(int meshResolution) {
    if (meshRes == meshResolution) {
        return;
    }
    meshRes = ClampRange(meshResolution, 1, 255);

    VboEboLoaded = false;
}

void GlGeomCube::CalcVboAndEbo(float* VBOdataBuffer, unsigned int* EBOdataBuffer,
	int vertPosOffset, int vertNormalOffset, int vertTexCoordsOffset, unsigned int stride)
{
    assert(vertPosOffset >= 0 && stride > 0);
    bool calcNormals = (vertNormalOffset >= 0);       // Should normals be calculated?
    bool calcTexCoords = (vertTexCoordsOffset >= 0);  // Should texture coordinates be calculated?

    /* 
     * ================
     * faces are numbered 0-3, with face 0 starting at the front and faces 1-3 going
     * counterclockwise from above; then 4 on top, 5 on bottom
     * 
     * with 0 as the front face,
     *     +---+
     *     | 4 |
     * +---+---+---+---+
     * | 3 | 0 | 1 | 2 |
     * +---+---+---+---+
     *     | 5 |
     *     +---+
     * 
     * textures are all upright in the net above
     *
     * vertices are inserted into the VBOdataBuffer starting from the bottom left corner of
     * each face, first in a row left-to-right, then across multiple rows bottom-to-top.
     *      e.g. for meshRes = 2, the 2x2 grid of squares on face 0 uses a 3x3 grid of vertices:
     *          6 7 8
     *          3 4 5
     *          0 1 2
     *      and the VBOdataBuffer would be filled from 0-8 in this order.
     *
     * vertices are inserted into the EBOdataBuffer in two-triangle squares, starting from the
     * botom left corner of each face and proceeding as per VBOdataBuffer above.
     *      e.g. for meshRes = 2 as above, it handles the squares in this order:
     *          0,1,3,4
     *          1,2,4,5
     *          3,4,6,7
     *          4,5,7,8
     * for each square, it first writes the upper left triangle, then the lower right triangle,
     * both starting from the greatest vertex
     *      e.g. for the 1,2,4,5 square above, it would give the triangles (5,4,1),(5,1,2)
     * 
     *      e.g. for this face, the EBOdataBuffer would contain
     *          4,3,0,4,0,1,5,4,1,5,1,2,7,6,3,7,3,4,8,7,4,8,4,5,
     * 
     * ================
     */

    // ========
    // Setup the data used to fill the buffers
    // ========

    float cubeNorms[] = { // the normal vectors for each face
         0.0,  0.0,  1.0,   // face 0
         1.0,  0.0,  0.0,   // face 1
         0.0,  0.0, -1.0,   // face 2
        -1.0,  0.0,  0.0,   // face 3
         0.0,  1.0,  0.0,   // face 4
         0.0, -1.0,  0.0,   // face 5
    };

    // Generate an array of coordinates, from -0.5 to 0.5
    // (This plus 0.5 will also be the texture coords)
    float* coords = new float[meshRes+1];
    // Manually assign the endpoints to avoid floating point error in pos
    coords[0] = -0.5f;
    // and generate all other points
    int i;
    for (i = 0; 2*i <= meshRes; i++) {
        coords[i] = (float)Lerp(-0.5, 0.5, (double)i / meshRes);
    }
    // Second half of coordinates are set this way to avoid roundoff errors.
    // They are the negative of the first half of the coordinates.
    for (; i <= meshRes; i++) {
        coords[i] = -coords[meshRes - i];
    }

    // Generate the vertex element order for the EBO
    // faceOrder is a template of the vertex order on the first face,
    //   and can be offset by a constant amount to be used on other faces.
    int* faceOrder = new int[GetNumElementsSide()];
    int squareOrder[] = {
        // The two triangles inside each square on the face
        //   These are the vertex elements for the bottom left square of a face.
        meshRes + 2,   meshRes + 1,    0,      // top left triangle
        meshRes + 2,   0,              1,      // bottom right triangle
    };
    int* faceOrderPtr = faceOrder;
    for (int row = 0; row < meshRes; row++) { // for each row of squares
        // for example, a row of squares might contain the two rows of vertices if meshRes=3
        //      4 5 6 7
        //      0 1 2 3
        for (int col = 0; col < meshRes; col++) { // for each square (each column) in the row
            // one square might be
            //      6 7
            //      2 3
            for (int i = 0; i < 6; i++) {         // for each entry in squareOrder
                *(faceOrderPtr++) = squareOrder[i] + (row*(meshRes+1)) + col;  // adjust for row and column
            }
        }
    }

    // ========
    // Fill the VBOdataBuffer and EBOdataBuffer face by face
    // ========

    // Initialize pointers to the start of their respective buffers
    float* vboPtr = VBOdataBuffer;
    unsigned int* eboPtr = EBOdataBuffer;

    for (int face = 0; face < 6; face++) {

        // Fills the VBOdataBuffer from face 0 to face 5, with the vertex order within each
        // face described above

        // iterate over each vertex on the face
        // x, y correspond to the row, column; it'll get the value from coords[]
        for (int y = 0; y <= meshRes; y++) {
            for (int x = 0; x <= meshRes; x++) {

                // assign the position values
                float* posPtr = vboPtr + vertPosOffset;
                InsertCoords(coords[x], coords[y], face, posPtr);

                // assign the normal values
                if (calcNormals) {
                    float* normPtr = vboPtr + vertNormalOffset;
                    for (int dim = 0; dim < 3; dim++) {
                        *(normPtr++) = cubeNorms[face * 3 + dim]; // constant across the entire face
                    }
                }

                // assign the texture values
                if (calcTexCoords) {
                    float* texPtr = vboPtr + vertTexCoordsOffset;
                    *(texPtr++) = coords[x] + 0.5f;
                    *(texPtr++) = coords[y] + 0.5f;
                }

                // increment to the next vertex
                vboPtr += stride;

            }
        }

        // Fills the EBOdataBuffer from face 0 to face 5, as rows from y=0 to y=meshRes
        // as used in filling the VBO
        faceOrderPtr = faceOrder;
        unsigned int faceOffset;
        if (calcTexCoords) {
            faceOffset = face * GetNumVerticesSideTexCoords();
        }
        else {
            faceOffset = face * GetNumVerticesSideNoTexCoords();
        }

        for (int i = 0; i < GetNumElementsSide(); i++) {
            // Add num faces to the value stored
            // E.g. for meshRes=2, there are 9 vertices to a face, so for the second
            // face (face=1), the first triangle 4,3,0 is offset by 1*9=9 to 13,12,9
            *(eboPtr++) = *(faceOrderPtr++) + faceOffset;
        }
    }

#if CUBE_DEBUG
    printf("REMESH CUBE: %d SQUARES\n", meshRes);
    printf("VBO offsets: %d %d %d %d\n\n", vertPosOffset, vertNormalOffset, vertTexCoordsOffset, stride);
    float* readVboPtr = VBOdataBuffer;
    unsigned int* readEboPtr = EBOdataBuffer;
    for (int face = 0; face < 6; face++) {
        printf("FACE %d\n", face);
        printf("VBO data:\n");
        for (int vert = 0; vert < GetNumVerticesSideTexCoords(); vert++) {
            for (int num = 0; num < stride; num++) {
                printf("%.2f ", *(readVboPtr++));
            }
            printf("\n");
        }
        printf("EBO data:\n");
        for (int elem = 0; elem < GetNumElementsSide(); elem++) {
            printf("%d ", *(readEboPtr++));
        }
        printf("\n\n");
    }
#endif

    delete[] coords;
    delete[] faceOrder;
    return;
}

void GlGeomCube::InsertCoords(float coords_x, float coords_y, int face, float* posPtr)
{
    switch (face) { // todo: find more elegant way to do this
    case 0: // front face
        *(posPtr++) = coords_x;
        *(posPtr++) = coords_y;
        *(posPtr++) = 0.5f;
        break;
    case 1: // right face
        *(posPtr++) = 0.5f;
        *(posPtr++) = coords_y;
        *(posPtr++) = -coords_x;
        break;
    case 2: // back face
        *(posPtr++) = -coords_x;
        *(posPtr++) = coords_y;
        *(posPtr++) = -0.5f;
        break;
    case 3: // left face
        *(posPtr++) = -0.5f;
        *(posPtr++) = coords_y;
        *(posPtr++) = coords_x;
        break;
    case 4: // top face
        *(posPtr++) = coords_x;
        *(posPtr++) = 0.5f;
        *(posPtr++) = -coords_y;
        break;
    case 5: // bottom face
        *(posPtr++) = coords_x;
        *(posPtr++) = -0.5f;
        *(posPtr++) = coords_y;
        break;
    }
}

void GlGeomCube::InitializeAttribLocations(
    unsigned int pos_loc, unsigned int normal_loc, unsigned int texcoords_loc)
{
    // The call to GlGeomBase::InitializeAttribLocations will further call
    //   GlGeomCube::CalcVboAndEbo()

    GlGeomBase::InitializeAttribLocations(pos_loc, normal_loc, texcoords_loc);
    VboEboLoaded = true;
}

void GlGeomCube::PreRender()
{
    GlGeomBase::PreRender();

    if (!VboEboLoaded) {
        ReInitializeAttribLocations();
    }
}

void GlGeomCube::Render()
{
    PreRender();
    GlGeomBase::Render();
}

void GlGeomCube::RenderFace(int face)
{
    assert(0 <= face && face < 6);
    PreRender();
    GlGeomBase::RenderEBO(GL_TRIANGLES, GetNumElementsSide(), face * GetNumElementsSide());
}