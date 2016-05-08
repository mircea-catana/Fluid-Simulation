#ifndef FLUID_SIMULATOR_H
#define FLUID_SIMULATOR_H

#define XY(I, J, W) I*W+J

namespace Fluid {

}

// DPENDENCIES
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <SDL/SDL.h>
#include <GL/glew.h>

// LOCAL
#include "Vec3.h"
#include "Vec4.h"
#include "Fluid.h"
#include "Vertex.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "FluidEngine.h"
#include "Solver.h"

#endif;
