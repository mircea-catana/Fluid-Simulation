namespace Fluid {
    class FluidEngine {
    private:
        bool running;
        SDL_Window *window;

        ShaderLoader *shaderLoader;
        GLuint currentShaderProgram;

        Mesh *mesh;
        Fluid *fluid;

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

        void update_colors() {
            Vertex *bufferData = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

            int width = mesh->get_mesh_width();
            int height = mesh->get_mesh_height();
            std::vector<float> *alpha = fluid->fluid_density(width, height);
            for (unsigned y = 0; y < height; ++y) {
                for (unsigned x = 0; x < width; ++x) {
                    // Edge vertex
                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        Vec4 vertexColor = Vec4(fluid->get_rgb(), alpha->at(XY(y, x, width)));
                        bufferData[XY(y, x, width)].set_color(vertexColor);
                    // Interior vertex
                    } else {
                        // Need to store this because of macro
                        int ny = y - 1; int sy = y + 1; int ex = x + 1; int wx = x - 1;

                        Vec4 northColor = Vec4(fluid->get_rgb(), alpha->at(XY(ny, x, width)));
                        Vec4 eastColor = Vec4(fluid->get_rgb(), alpha->at(XY(y, ex, width)));
                        Vec4 southColor = Vec4(fluid->get_rgb(), alpha->at(XY(sy, x, width)));
                        Vec4 westColor = Vec4(fluid->get_rgb(), alpha->at(XY(y, wx, width)));
                        Vec4 vertexColor = (northColor + eastColor + southColor + westColor) / 4.0f;
                        bufferData[XY(y, x, width)].set_color(vertexColor);
                    }
                }
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        void draw() {
            glClearDepth(1.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(currentShaderProgram);

            update_colors();

            glBindVertexArray(mesh->get_vao());
            glDrawElements(GL_TRIANGLES, mesh->get_indices()->size(), GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);

            SDL_GL_SwapWindow(window);
        }

        void create_scene() {
            mesh = new Mesh();
            mesh->CreateGrid(100, 100);

            fluid = new Fluid(100, 100);
           fluid->random_fill();
			//fluid->update();
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
			int i = 0;
            while (running) {
                handle_input();
				fluid->updateStam(i);
				i++;
                draw();
            }
        }

        void cleanup() {
            delete shaderLoader;
            SDL_Quit();
        }
    };
}
