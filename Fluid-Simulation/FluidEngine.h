namespace Fluid {
    class FluidEngine {
    private:
        bool running;
        SDL_Window *window;

        ShaderLoader *shaderLoader;
        GLuint currentShaderProgram;

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

        void draw() {
            glClearDepth(1.0f);
            glClearColor(0.8f, 0.2f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(currentShaderProgram);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            SDL_GL_SwapWindow(window);
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

            shaderLoader = new ShaderLoader();
            currentShaderProgram = shaderLoader->CreateProgram("Shaders\\SmokeVS.glsl", "Shaders\\SmokeFS.glsl");
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            running = true;
        }

        void game_loop() {
            while (running) {
                handle_input();
                draw();
            }
        }

        void cleanup() {
            delete shaderLoader;
            SDL_Quit();
        }
    };
}
