#ifndef FLUID_SIMULATOR_H
#define FLUID_SIMULATOR_H

auto XY = [](int a, int b, int w) { return a*w + b; };

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
#include "Fluid.h"
#include "Vertex.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "FluidEngine.h"

#endif;
