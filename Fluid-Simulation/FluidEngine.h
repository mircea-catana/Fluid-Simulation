namespace Fluid {
    
    class FluidEngine {
    private:
        const int MAX_IMAGES_RENDERED = 1250;

        FluidMode fluidMode;
        RenderMode renderMode;

        bool running;
        int windowWidth;
        int windowHeight;
        int frameCounter;

        SDL_Window *window;

        ShaderLoader *shaderLoader;
        GLuint currentShaderProgram;

        Mesh *mesh;
        Mesh3D *mesh3D;
        Mesh3D *containingCube;

        Fluid *fluid;
        Fluid3D *fluid3D;

        std::vector<unsigned char>* imagePixels;

        float modelToWorld[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f };

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

        void render_image(int frameNumber) {
            printf("Rendering image number: %d\n", frameNumber);
            std::string filename = "../Release/Images/frame_" + std::to_string(frameNumber) + ".png";
            glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, imagePixels->data());

            for (unsigned x = 0; x < windowWidth; ++x) {
                for (unsigned y = 0; y <= windowHeight / 2; ++y) {
                    std::swap(imagePixels->at(4*y*windowWidth + 4*x), imagePixels->at(4*(windowHeight - 1 - y)*windowWidth + 4*x));
                    std::swap(imagePixels->at(4 * y*windowWidth + 4 * x + 1), imagePixels->at(4 * (windowHeight - 1 - y)*windowWidth + 4 * x + 1));
                    std::swap(imagePixels->at(4 * y*windowWidth + 4 * x + 2), imagePixels->at(4 * (windowHeight - 1 - y)*windowWidth + 4 * x + 2));
                    std::swap(imagePixels->at(4 * y*windowWidth + 4 * x + 3), imagePixels->at(4 * (windowHeight - 1 - y)*windowWidth + 4 * x + 3));
                }
            }

            unsigned int error = lodepng::encode(filename, *imagePixels, windowWidth, windowHeight);
            if (error) {
                printf("Image encoding error:\n%s\n", lodepng_error_text(error));
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

                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        vertexColor = Vec4(fluid->get_rgb(), alpha->at(YX(y, x, width)));
                    } else {

                        float a1 = (alpha->at(YX(y - 1, x, width)) + alpha->at(YX(y, x + 1, width)) +
                            alpha->at(YX(y + 1, x, width)) + alpha->at(YX(y, x - 1, width))) / 4.0f;
                        float a2 = (alpha->at(YX(y - 1, x + 1, width)) + alpha->at(YX(y + 1, x + 1, width)) +
                            alpha->at(YX(y + 1, x - 1, width)) + alpha->at(YX(y - 1, x - 1, width))) / 4.0f;

                        vertexColor = Vec4(fluid->get_rgb(), (a1 + a2) / 2.0f);
                    }

                    bufferData[YX(y, x, width)].set_color(vertexColor);
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

                        if (x == 0 || x == size - 1 || y == 0 || y == size - 1 || z == 0 || z == size - 1) {
                            vertexColor = Vec4(fluid3D->get_rgb(), alpha->at(ZYX(z, y, x, size)));
                        } else {

                            float a1 = alpha->at(ZYX(z, y, x - 1, size)) + alpha->at(ZYX(z, y, x + 1, size)) +
                                       alpha->at(ZYX(z, y - 1, x, size)) + alpha->at(ZYX(z, y + 1, x, size));
                            float a2 = alpha->at(ZYX(z - 1, y, x - 1, size)) + alpha->at(ZYX(z - 1, y, x + 1, size)) +
                                       alpha->at(ZYX(z - 1, y - 1, x, size)) + alpha->at(ZYX(z  -1, y + 1, x, size));

                            vertexColor = Vec4(fluid3D->get_rgb(), (a1 + a2) / 2.0f);
                        }

                        bufferData[ZYX(z, y, x, size)].set_color(vertexColor);
                    }
                }
            }
            
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void draw() {
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

            GLuint frameNumberPosition = glGetUniformLocation(currentShaderProgram, "frameNumber");
            if (fluidMode == THREE_DIM) {
                glUniform1f(frameNumberPosition, frameCounter / 150.0f);
            } else {
                glUniform1f(frameNumberPosition, 0.0f);
            }

            if (fluidMode == TWO_DIM) {
                update_colors_2D();

                glBindVertexArray(mesh->get_vao());
                glEnable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawElements(GL_TRIANGLES, mesh->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);
                
            } else if (fluidMode == THREE_DIM) {
                update_colors_3D();

                glBindVertexArray(mesh3D->get_vao());
                glDisable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDrawElements(GL_TRIANGLES, mesh3D->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);

                glBindVertexArray(containingCube->get_vao());
                glEnable(GL_DEPTH_TEST);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDrawElements(GL_QUADS, containingCube->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);
            }

            glBindVertexArray(0);
            SDL_GL_SwapWindow(window);
        }

        void create_scene() {
            float aspect = (float)windowWidth / windowHeight;
            if (fluidMode == TWO_DIM) {
                set_perspective_camera(60.0f, aspect, 0.1f, 10.0f);
                set_camera_position(Vec3(0.0f, 0.0f, -2.0f));

                mesh = new Mesh();
                mesh->CreateGrid(150, 150);

                fluid = new Fluid(150, 150);
                fluid->clear();

            } else if (fluidMode == THREE_DIM) {
                set_perspective_camera(60.0f, aspect, 0.1f, 100.0f);
                set_camera_position(Vec3(0.0f, 0.0f, -3.0f));

                containingCube = new Mesh3D();
                containingCube->CreateQuadCube(2);

                mesh3D = new Mesh3D();
                mesh3D->CreateCube(30);

                fluid3D = new Fluid3D(30);
                fluid3D->clear();
            }
        }
        
        // Should move to own class
        void set_perspective_camera(float fovy, float aspect, float nearPlane, float farPlane) {
            float deltaZ = farPlane - nearPlane;
            float radians = fovy / 2.0f * 3.1415f / 180.0f;
            float ctg = cos(radians) / sin(radians);

            viewToProjection[0] = ctg / aspect;
            viewToProjection[5] = ctg;
            viewToProjection[10] = -(farPlane + nearPlane) / deltaZ;
            viewToProjection[11] = -1.0f;
            viewToProjection[14] = -2.0f * nearPlane * farPlane / deltaZ;
            viewToProjection[15] = 0.0f;
        }

        void set_mesh_position(Vec3 position) {
            modelToWorld[12] = position.get_x();
            modelToWorld[13] = position.get_y();
            modelToWorld[14] = position.get_z();
        }

        void set_camera_position(Vec3 position) {
            worldToView[12] = position.get_x();
            worldToView[13] = position.get_y();
            worldToView[14] = position.get_z();
        }

    public:
        static FluidEngine& instance() {
            static FluidEngine *inst = nullptr;
            if (inst == nullptr) {
                inst = new FluidEngine();
            }
            return *inst;
        }

        void init(FluidMode fluidMode, RenderMode renderMode, int windowWidth, int windowHeight) {
            this->fluidMode = fluidMode;
            this->renderMode = renderMode;

            this->windowWidth = windowWidth;
            this->windowHeight = windowHeight;

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
            //glEnable(GL_DEPTH_TEST);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glClearDepth(1.0f);
            glClearColor(0.28f, 0.3f, 0.45f, 1.0f);

            create_scene();

            shaderLoader = new ShaderLoader();
            currentShaderProgram = shaderLoader->CreateProgram("Shaders\\SmokeVS.glsl", "Shaders\\SmokeFS.glsl");

            if (renderMode == IMAGE) {
                imagePixels = new std::vector<unsigned char>(windowWidth * windowHeight * 4);
            }

            running = true;
        }

        void game_loop() {
            frameCounter = 0;
            float simulationStep = 0.0f;

            while (running) {
                handle_input();

                if (fluidMode == TWO_DIM) {
                    fluid->updateStam(simulationStep);
                } else if (fluidMode == THREE_DIM) {
                    fluid3D->update_stam(simulationStep);
                }
                
                draw();

                if (renderMode == IMAGE && frameCounter <= MAX_IMAGES_RENDERED && frameCounter > 0) {
                    render_image(frameCounter);
                } else if (renderMode == IMAGE && frameCounter > MAX_IMAGES_RENDERED) {
                    running = false;
                }

                ++frameCounter;
                simulationStep += 1.0f;
            }
        }

        void cleanup() {
            delete shaderLoader;

            delete imagePixels;

            delete fluid;
            delete fluid3D;

            delete mesh;
            delete mesh3D;

            SDL_Quit();
        }
    };
}
