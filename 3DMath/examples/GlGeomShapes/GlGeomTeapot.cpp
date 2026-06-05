/*
* GlGeomTeapot.cpp - Version 1.1 - November 13, 2020
*
* C++ class for rendering a version of the Utah teapot in Modern OpenGL.
*   A GlGeomTeapot object encapsulates a VAO, a VBO, and an EBO,
*   which can be used to render the teapot.
*   The u- and v-mesh resolution of the patches can be varied.
*   GlGeomTeapot is built on top of GlGeomBezier
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

#include "GlGeomTeapot.h"

double* GlGeomTeapot::teapotControlPts = 0;  // Stores the entire set of Bezier patch control points

// The data arrays below are from fg_teaput_data.h (Freeglut-3.2.1)

/*
 * Rim, body, lid, and bottom data must be rotated along all four quadrants;
 * handle and spout data is flipped across the x-y plane (negate z values) only.
 * Vertex order must be adjusted when reflecting across the x-y plane
 * to have consistent outward face directions.
 */
#define GLUT_TEAPOT_N_INPUT_PATCHES 10
static int patchdata_teapot[GLUT_TEAPOT_N_INPUT_PATCHES][16] =
{
    {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15, }, /* rim    */
    { 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27, }, /* body   */
    { 24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39, },
    { 64,  64,  64,  64,  65,  66,  67,  68,  69,  70,  71,  72,  39,  38,  37,  36, }, /* bottom */
    { 40,  41,  42,  40,  43,  44,  45,  46,  47,  47,  47,  47,  48,  49,  50,  51, }, /* lid    */
    { 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63, },
    { 73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88, }, /* handle */
    { 85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, },
    {101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, }, /* spout  */
    {113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128  }
};

// Teapot data is in an unexpected order. Coordinates are:
//     First: Towards the spout (would be expected to be "z")
//     Second: Out the side (would be expected to be "x")
//     Third: Central axis (would be expected to be "y")
// The code permutes the teapot axes so that "y" is up, and "z" is the spout direction.
static float cpdata_teapot[][3] =
{
    { 1.40000f,  0.00000f,  2.40000f}, { 1.40000f, -0.78400f,  2.40000f},
    { 0.78400f, -1.40000f,  2.40000f}, { 0.00000f, -1.40000f,  2.40000f},
    { 1.33750f,  0.00000f,  2.53125f}, { 1.33750f, -0.74900f,  2.53125f},
    { 0.74900f, -1.33750f,  2.53125f}, { 0.00000f, -1.33750f,  2.53125f},
    { 1.43750f,  0.00000f,  2.53125f}, { 1.43750f, -0.80500f,  2.53125f},
    { 0.80500f, -1.43750f,  2.53125f}, { 0.00000f, -1.43750f,  2.53125f},
    { 1.50000f,  0.00000f,  2.40000f}, { 1.50000f, -0.84000f,  2.40000f},
    { 0.84000f, -1.50000f,  2.40000f}, { 0.00000f, -1.50000f,  2.40000f},
    { 1.75000f,  0.00000f,  1.87500f}, { 1.75000f, -0.98000f,  1.87500f},
    { 0.98000f, -1.75000f,  1.87500f}, { 0.00000f, -1.75000f,  1.87500f},
    { 2.00000f,  0.00000f,  1.35000f}, { 2.00000f, -1.12000f,  1.35000f},
    { 1.12000f, -2.00000f,  1.35000f}, { 0.00000f, -2.00000f,  1.35000f},
    { 2.00000f,  0.00000f,  0.90000f}, { 2.00000f, -1.12000f,  0.90000f},
    { 1.12000f, -2.00000f,  0.90000f}, { 0.00000f, -2.00000f,  0.90000f},
    { 2.00000f,  0.00000f,  0.45000f}, { 2.00000f, -1.12000f,  0.45000f},
    { 1.12000f, -2.00000f,  0.45000f}, { 0.00000f, -2.00000f,  0.45000f},
    { 1.50000f,  0.00000f,  0.22500f}, { 1.50000f, -0.84000f,  0.22500f},
    { 0.84000f, -1.50000f,  0.22500f}, { 0.00000f, -1.50000f,  0.22500f},
    { 1.50000f,  0.00000f,  0.15000f}, { 1.50000f, -0.84000f,  0.15000f},
    { 0.84000f, -1.50000f,  0.15000f}, { 0.00000f, -1.50000f,  0.15000f},
    { 0.00000f,  0.00000f,  3.15000f}, { 0.00000f, -0.00200f,  3.15000f},
    { 0.00200f,  0.00000f,  3.15000f}, { 0.80000f,  0.00000f,  3.15000f},
    { 0.80000f, -0.45000f,  3.15000f}, { 0.45000f, -0.80000f,  3.15000f},
    { 0.00000f, -0.80000f,  3.15000f}, { 0.00000f,  0.00000f,  2.85000f},
    { 0.20000f,  0.00000f,  2.70000f}, { 0.20000f, -0.11200f,  2.70000f},
    { 0.11200f, -0.20000f,  2.70000f}, { 0.00000f, -0.20000f,  2.70000f},
    { 0.40000f,  0.00000f,  2.55000f}, { 0.40000f, -0.22400f,  2.55000f},
    { 0.22400f, -0.40000f,  2.55000f}, { 0.00000f, -0.40000f,  2.55000f},
    { 1.30000f,  0.00000f,  2.55000f}, { 1.30000f, -0.72800f,  2.55000f},
    { 0.72800f, -1.30000f,  2.55000f}, { 0.00000f, -1.30000f,  2.55000f},
    { 1.30000f,  0.00000f,  2.40000f}, { 1.30000f, -0.72800f,  2.40000f},
    { 0.72800f, -1.30000f,  2.40000f}, { 0.00000f, -1.30000f,  2.40000f},
    { 0.00000f,  0.00000f,  0.00000f}, { 0.00000f, -1.42500f,  0.00000f},
    { 0.79800f, -1.42500f,  0.00000f}, { 1.42500f, -0.79800f,  0.00000f},
    { 1.42500f,  0.00000f,  0.00000f}, { 0.00000f, -1.50000f,  0.07500f},
    { 0.84000f, -1.50000f,  0.07500f}, { 1.50000f, -0.84000f,  0.07500f},
    { 1.50000f,  0.00000f,  0.07500f}, {-1.60000f,  0.00000f,  2.02500f},
    {-1.60000f, -0.30000f,  2.02500f}, {-1.50000f, -0.30000f,  2.25000f},
    {-1.50000f,  0.00000f,  2.25000f}, {-2.30000f,  0.00000f,  2.02500f},
    {-2.30000f, -0.30000f,  2.02500f}, {-2.50000f, -0.30000f,  2.25000f},
    {-2.50000f,  0.00000f,  2.25000f}, {-2.70000f,  0.00000f,  2.02500f},
    {-2.70000f, -0.30000f,  2.02500f}, {-3.00000f, -0.30000f,  2.25000f},
    {-3.00000f,  0.00000f,  2.25000f}, {-2.70000f,  0.00000f,  1.80000f},
    {-2.70000f, -0.30000f,  1.80000f}, {-3.00000f, -0.30000f,  1.80000f},
    {-3.00000f,  0.00000f,  1.80000f}, {-2.70000f,  0.00000f,  1.57500f},
    {-2.70000f, -0.30000f,  1.57500f}, {-3.00000f, -0.30000f,  1.35000f},
    {-3.00000f,  0.00000f,  1.35000f}, {-2.50000f,  0.00000f,  1.12500f},
    {-2.50000f, -0.30000f,  1.12500f}, {-2.65000f, -0.30000f,  0.93750f},
    {-2.65000f,  0.00000f,  0.93750f}, {-2.00000f,  0.00000f,  0.90000f},
    {-2.00000f, -0.30000f,  0.90000f}, {-1.90000f, -0.30000f,  0.60000f},
    {-1.90000f,  0.00000f,  0.60000f}, { 1.70000f,  0.00000f,  1.42500f},
    { 1.70000f, -0.66000f,  1.42500f}, { 1.70000f, -0.66000f,  0.60000f},
    { 1.70000f,  0.00000f,  0.60000f}, { 2.60000f,  0.00000f,  1.42500f},
    { 2.60000f, -0.66000f,  1.42500f}, { 3.10000f, -0.66000f,  0.82500f},
    { 3.10000f,  0.00000f,  0.82500f}, { 2.30000f,  0.00000f,  2.10000f},
    { 2.30000f, -0.25000f,  2.10000f}, { 2.40000f, -0.25000f,  2.02500f},
    { 2.40000f,  0.00000f,  2.02500f}, { 2.70000f,  0.00000f,  2.40000f},
    { 2.70000f, -0.25000f,  2.40000f}, { 3.30000f, -0.25000f,  2.40000f},
    { 3.30000f,  0.00000f,  2.40000f}, { 2.80000f,  0.00000f,  2.47500f},
    { 2.80000f, -0.25000f,  2.47500f}, { 3.52500f, -0.25000f,  2.49375f},
    { 3.52500f,  0.00000f,  2.49375f}, { 2.90000f,  0.00000f,  2.47500f},
    { 2.90000f, -0.15000f,  2.47500f}, { 3.45000f, -0.15000f,  2.51250f},
    { 3.45000f,  0.00000f,  2.51250f}, { 2.80000f,  0.00000f,  2.40000f},
    { 2.80000f, -0.15000f,  2.40000f}, { 3.20000f, -0.15000f,  2.40000f},
    { 3.20000f,  0.00000f,  2.40000f}
};


void GlGeomTeapot::InitializeAttribLocations(
    unsigned int pos_loc, unsigned int normal_loc, unsigned int texcoords_loc)
{
    // Load vertices into the GlGeomBezier if not already done.
    if (teapotControlPts == 0) {
        LoadTeapotControlPoints();
    }
    GlGeomBezier::InitializeAttribLocations(pos_loc, normal_loc, texcoords_loc);
}

void GlGeomTeapot::LoadTeapotControlPoints() {
    CalcTeapotControlPoints();
    GlGeomBezier::LoadControlPts(4, 4, 3, 32, teapotControlPts);
}

void GlGeomTeapot::CalcTeapotControlPoints() {
    if (teapotControlPts != 0) {
        return;
    }
    teapotControlPts = new double[4 * 4 * 3 * 32 * 4];    // Control points for 32 patches
    double* toPtr = teapotControlPts;

    // Calculate 24 patches from the the six patches with 4-fold rotational symmetry
    for (int i = 0; i < 6; i++) {
        for (int sym = 0; sym < 4; sym++) {
            for (int k = 0; k < 16; k++) {
                int index = patchdata_teapot[i][k];
                double z = cpdata_teapot[index][0];
                double x = cpdata_teapot[index][1];
                double y = cpdata_teapot[index][2];
                if ((sym & 0x02) != 0) {
                    // Rotate 180 degrees around y axis.
                    x = -x;
                    z = -z;
                }
                if ((sym & 0x01) != 0) {
                    // Rotate 90 degrees around y axis
                    double temp = x;
                    x = z;
                    z = -temp;
                }
                *(toPtr++) = x;
                *(toPtr++) = y;
                *(toPtr++) = z;
            }
        }
    }

    // Calculate 8 patches from the the four patches with 2-fold mirror (xy) symmetry
    for (int i = 0; i < 4; i++) {
        // Patch in given position
        for (int k = 0; k < 16; k++) {
            int index = patchdata_teapot[i+6][k];
            *(toPtr++) = cpdata_teapot[index][1];
            *(toPtr++) = cpdata_teapot[index][2];
            *(toPtr++) = cpdata_teapot[index][0];
        }
        // Patch reflected across x-y plane.
        for (int k = 0; k < 16; k++) {
            int kk = (k & 0x0c) + (3 - (k & 0x03));
            int index = patchdata_teapot[i+6][kk];
            *(toPtr++) = -cpdata_teapot[index][1]; // Negate to reflect across y-z plane
            *(toPtr++) = cpdata_teapot[index][2];
            *(toPtr++) = cpdata_teapot[index][0]; 
        }
    }
}
