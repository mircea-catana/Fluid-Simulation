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
        Mesh3D *mesh3D;

        Fluid *fluid;
        Fluid3D *fluid3D;

        float modelToWorld[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, -4.0f, 1.0f };

        float worldToView[16] = { 1.0f, 0.0f, 0.0f, 0.0f, 
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  0.0f, 0.0f, 0.0f, 1.0f };

        float viewToProjection[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f,
                                       0.0f, 0.0f, 0.0f, 1.0f };

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
                        vertexColor = Vec4(fluid->get_rgb(), alpha->at(YX(y, x, width)));
                        bufferData[YX(y, x, width)].set_color(vertexColor);

                        // Interior vertex
                    } else {
                        // Need to store this because of macro
                        int ny = y - 1; int sy = y + 1; int ex = x + 1; int wx = x - 1;

                        float a1 = (alpha->at(YX(ny, x, width)) + alpha->at(YX(y, ex, width)) +
                            alpha->at(YX(sy, x, width)) + alpha->at(YX(y, wx, width))) / 4.0f;
                        float a2 = (alpha->at(YX(ny, ex, width)) + alpha->at(YX(sy, ex, width)) +
                            alpha->at(YX(sy, wx, width)) + alpha->at(YX(ny, wx, width))) / 4.0f;

                        vertexColor = Vec4(fluid->get_rgb(), (a1 + a2) / 2.0f);
                        bufferData[YX(y, x, width)].set_color(vertexColor);
                    }
                }
            }


            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void update_colors_3D() {
            Vertex *bufferData = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

            int size = mesh3D->get_mesh_size();
            std::vector<float> *alpha = fluid3D->fluid_density(size);
            Vec4 vertexColor;

            for (unsigned z = 0; z < size; ++z) {
                for (unsigned y = 0; y < size; ++y) {
                    for (unsigned x = 0; x < size; ++x) {
                        // Edge vertex
                        //if (x == 0 || x == size - 1 || y == 0 || y == size - 1 || z == 0 || z == size - 1) {
                            vertexColor = Vec4(fluid3D->get_rgb(), alpha->at(ZYX(z, y, x, size)));
                            bufferData[ZYX(z, y, x, size)].set_color(vertexColor);

                            // Interior vertex
                        /*} else {

                            float a1 = (alpha->at(YX(ny, x, width)) + alpha->at(YX(y, ex, width)) +
                                        alpha->at(YX(sy, x, width)) + alpha->at(YX(y, wx, width))) / 4.0f;
                            float a2 = (alpha->at(YX(ny, ex, width)) + alpha->at(YX(sy, ex, width)) +
                                        alpha->at(YX(sy, wx, width)) + alpha->at(YX(ny, wx, width))) / 4.0f;

                            vertexColor = Vec4(fluid3D->get_rgb(), (a1 + a2) / 2.0f);
                            bufferData[YX(y, x, width)].set_color(vertexColor);
                        }*/
                    }
                }
            }
            
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void draw() {
            //modelToWorld[14] -= 0.002f;

            glClearDepth(1.0f);
            glClearColor(0.08f, 0.18f, 0.35f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(currentShaderProgram);

            float globalTime = (float)SDL_GetTicks() / 1000.0f;
            GLuint globalTimePosition = glGetUniformLocation(currentShaderProgram, "globalTime");
            glUniform1f(globalTimePosition, globalTime);

            GLuint mtwPosition = glGetUniformLocation(currentShaderProgram, "modelToWorld");
            glUniformMatrix4fv(mtwPosition, 1, GL_FALSE, modelToWorld);

            GLuint wtvPosition = glGetUniformLocation(currentShaderProgram, "worldToView");
            glUniformMatrix4fv(wtvPosition, 1, GL_FALSE, worldToView);

            GLuint vtpPosition = glGetUniformLocation(currentShaderProgram, "viewToProjection");
            glUniformMatrix4fv(vtpPosition, 1, GL_FALSE, viewToProjection);

            if (fluidMode == _2D) {
                update_colors_2D();
                glBindVertexArray(mesh->get_vao());
                glDrawElements(GL_TRIANGLES, mesh->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);
                glBindVertexArray(0);
            } else if (fluidMode == _3D) {
                update_colors_3D();
                glBindVertexArray(mesh3D->get_vao());
                glDrawElements(GL_TRIANGLES, mesh3D->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);
                glBindVertexArray(0);
            }

            

            SDL_GL_SwapWindow(window);
        }

        void create_scene() {
            fluidMode = _3D;
            set_perspective_camera();
            if (fluidMode == _2D) {
                mesh = new Mesh();
                mesh->CreateGrid(150, 150);
                fluid = new Fluid(150, 150);
                fluid->clear();
            } else if (fluidMode == _3D) {
                mesh3D = new Mesh3D();
                mesh3D->CreateCube(20);
                fluid3D = new Fluid3D(20);
                fluid3D->clear();
            }
        }

        void set_perspective_camera() {
            float fovy = 60.0f;
            float aspect = (float)1024 / 768;
            float zNear = 0.1f;
            float zFar = 500.0f;

            float deltaZ = zFar - zNear;
            float radians = fovy / 2.0f * 3.1415f / 180.0f;
            float s = sin(radians);
            float ctg = cos(radians) / s;

            viewToProjection[0] = ctg / aspect;
            viewToProjection[5] = ctg;
            viewToProjection[10] = -(zFar + zNear) / deltaZ;
            viewToProjection[11] = -1.0f;
            viewToProjection[14] = -2.0f * zNear * zFar / deltaZ;
            viewToProjection[15] = 0.0f;
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
