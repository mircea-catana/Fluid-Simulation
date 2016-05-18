#ifndef FLUID_SIMULATOR_H
#define FLUID_SIMULATOR_H

auto YX = [](int y, int x, int N) { return y*N + x; };
auto ZYX = [](int z, int y, int x, int N) { return z*N*N + y*N + x; };

namespace Fluid {
    enum FluidMode { TWO_DIM, THREE_DIM };
    enum RenderMode { SCREEN, IMAGE };
}

// DPENDENCIES
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <ppl.h>
#include <string>
#include <vector>
#include <valarray>
#include <memory>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include "lodepng.h"

// LOCAL
#include "Vec3.h"
#include "Vec4.h"
#include "Solver.h"
#include "Solver3D.h"
#include "Fluid.h"
#include "Fluid3D.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Mesh3D.h"
#include "ShaderLoader.h"
#include "FluidEngine.h"

#endif;
