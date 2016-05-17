namespace Fluid {
    class FluidEngine {
    private:
        enum FluidMode { _2D, _3D };

        FluidMode fluidMode;
        bool running;
        SDL_Window *window;

        ShaderLoader *shaderLoader;
        GLuint currentShaderProgram;

        Mesh *mesh;
        Fluid *fluid;
        Fluid3D *fluid3D;

        void handle_input() {
            SDL_Event evnt;

            while (SDL_PollEvent(&evnt)) {
                switch (evnt.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
                }
            }
        }

        void update_colors_2D() {
            Vertex *bufferData = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

            int width = mesh->get_mesh_width();
            int height = mesh->get_mesh_height();
            std::vector<float> *alpha = fluid->fluid_density(width, height);
            Vec4 vertexColor;

            for (unsigned y = 0; y < height; ++y) {
                for (unsigned x = 0; x < width; ++x) {
                    // Edge vertex
                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        vertexColor = Vec4(fluid->get_rgb(), alpha->at(XY(y, x, width)));
                        bufferData[XY(y, x, width)].set_color(vertexColor);

                        // Interior vertex
                    } else {
                        // Need to store this because of macro
                        int ny = y - 1; int sy = y + 1; int ex = x + 1; int wx = x - 1;

                        float a1 = (alpha->at(XY(ny, x, width)) + alpha->at(XY(y, ex, width)) +
                            alpha->at(XY(sy, x, width)) + alpha->at(XY(y, wx, width))) / 4.0f;
                        float a2 = (alpha->at(XY(ny, ex, width)) + alpha->at(XY(sy, ex, width)) +
                            alpha->at(XY(sy, wx, width)) + alpha->at(XY(ny, wx, width))) / 4.0f;

                        vertexColor = Vec4(fluid->get_rgb(), (a1 + a2) / 2.0f);
                        bufferData[XY(y, x, width)].set_color(vertexColor);
                    }
                }
            }


            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void update_colors_3D() {
            Vertex *bufferData = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

            int width = mesh->get_mesh_width();
            int height = mesh->get_mesh_height();
            std::vector<float> *alpha = fluid3D->fluid_density(width, height);
            Vec4 vertexColor;

            for (unsigned y = 0; y < height; ++y) {
                for (unsigned x = 0; x < width; ++x) {
                    // Edge vertex
                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        vertexColor = Vec4(fluid3D->get_rgb(), alpha->at(XY(y, x, width)));
                        bufferData[XY(y, x, width)].set_color(vertexColor);

                        // Interior vertex
                    } else {
                        // Need to store this because of macro
                        int ny = y - 1; int sy = y + 1; int ex = x + 1; int wx = x - 1;

                        float a1 = (alpha->at(XY(ny, x, width)) + alpha->at(XY(y, ex, width)) +
                                    alpha->at(XY(sy, x, width)) + alpha->at(XY(y, wx, width))) / 4.0f;
                        float a2 = (alpha->at(XY(ny, ex, width)) + alpha->at(XY(sy, ex, width)) +
                                    alpha->at(XY(sy, wx, width)) + alpha->at(XY(ny, wx, width))) / 4.0f;

                        vertexColor = Vec4(fluid3D->get_rgb(), (a1 + a2) / 2.0f);
                        bufferData[XY(y, x, width)].set_color(vertexColor);
                    }
                }
            }


            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void draw() {
            glClearDepth(1.0f);
            glClearColor(0.08f, 0.18f, 0.35f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(currentShaderProgram);

            float globalTime = (float)SDL_GetTicks() / 1000.0f;
            GLuint globalTimePosition = glGetUniformLocation(currentShaderProgram, "globalTime");
            glUniform1f(globalTimePosition, globalTime);

            if (fluidMode == _2D) {
                update_colors_2D();
            } else if (fluidMode == _3D) {
                update_colors_3D();
            }

            glBindVertexArray(mesh->get_vao());
            glDrawElements(GL_TRIANGLES, mesh->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);

            SDL_GL_SwapWindow(window);
        }

        void create_scene() {
            mesh = new Mesh();

            fluidMode = _3D;

            if (fluidMode == _2D) {
                mesh->CreateGrid(150, 150);
                fluid = new Fluid(150, 150);
                fluid->clear();
            } else if (fluidMode == _3D) {
                mesh->CreateGrid(25, 25);
                fluid3D = new Fluid3D(25);
                fluid3D->clear();
            }
        }

    public:
        static FluidEngine& instance() {
            static FluidEngine *inst = nullptr;
            if (inst == nullptr) {
                inst = new FluidEngine();
            }
            return *inst;
        }

        void init(int windowWidth, int windowHeight) {
            SDL_Init(SDL_INIT_EVERYTHING);

            window = SDL_CreateWindow("Fluid Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
            if (window == nullptr) {
                printf("ERR: SDL_Window failed to initialize.\n");
                SDL_Quit();
                exit(1);
            }

            SDL_GLContext gl_context = SDL_GL_CreateContext(window);
            if (gl_context == nullptr) {
                printf("ERR: SDL_GLContext failed to initialize.\n");
                SDL_Quit();
                exit(1);
            }

            if (glewInit() != GLEW_OK) {
                printf("ERR: GLEW failed to initialize.\n");
                SDL_Quit();
                exit(1);
            }

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            create_scene();

            shaderLoader = new ShaderLoader();
            currentShaderProgram = shaderLoader->CreateProgram("Shaders\\SmokeVS.glsl", "Shaders\\SmokeFS.glsl");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            running = true;
        }

        void game_loop() {
            float frameNumber = 0;
            while (running) {
                handle_input();

                if (fluidMode == _2D) {
                    fluid->updateStam(frameNumber);
                } else if (fluidMode == _3D) {
                    fluid3D->update_stam(frameNumber);
                }
                
                frameNumber += 1.0f;
                draw();
            }
        }

        void cleanup() {
            delete shaderLoader;
            SDL_Quit();
        }
    };
}
