#include "FluidSimulator.h"
const int window_width =  1024;
const int window_height =  768;

int main(int argc, char **argv) {

    Fluid::FluidEngine *engine = &(Fluid::FluidEngine::instance());
    engine->init(Fluid::FluidMode::THREE_DIM, Fluid::RenderMode::IMAGE, window_width, window_height);
    engine->game_loop();
    engine->cleanup();

    return 0;
}
