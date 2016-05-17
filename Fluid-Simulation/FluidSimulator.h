#ifndef FLUID_SIMULATOR_H
#define FLUID_SIMULATOR_H

auto XY = [](int y, int x, int w) { return y*w + x; };
auto XYZ = [](int z, int y, int x, int N) { return z*N*N + y*N + x; };
namespace Fluid {

}

// DPENDENCIES
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <ppl.h>
#include <vector>
#include <valarray>
#include <memory>
#include <SDL/SDL.h>
#include <GL/glew.h>

// LOCAL
#include "Vec3.h"
#include "Vec4.h"
#include "Solver.h"
#include "Solver3D.h"
#include "Fluid.h"
#include "Fluid3D.h"
#include "Vertex.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "FluidEngine.h"

#endif;
