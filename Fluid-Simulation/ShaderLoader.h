namespace Fluid {
    class ShaderLoader {
    private:

    public:
        std::string ReadFile(char *filename) {
            std::string shaderCode;
            std::ifstream file(filename, std::ios::in);

            if (!file.good()) {
                printf("ERR: Couldn't read shader file: %s\n", filename);
                return NULL;
            }

            file.seekg(0, std::ios::end);
            shaderCode.resize((unsigned int)file.tellg());
            file.seekg(0, std::ios::beg);
            file.read(&shaderCode[0], shaderCode.size());
            file.close();
            return shaderCode;
        }

        GLuint CreateShader(GLenum shaderType, std::string source, char *shaderName) {
            int result = 0;

            GLuint shader = glCreateShader(shaderType);
            const char *shaderCode = source.c_str();
            const int shaderCodeSize = source.size();

            glShaderSource(shader, 1, &shaderCode, &shaderCodeSize);
            glCompileShader(shader);
            glGetShaderiv(shader, GL_COMPILE_STATUS, &result);


            if (result == GL_FALSE) {
                int logLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<char> shaderLog(logLength);
                glGetShaderInfoLog(shader, logLength, NULL, &shaderLog[0]);
                printf("ERR: Shader compilation failed: %s\n%s\n", shaderName, &shaderLog[0]);
                return 0;
            }

            return shader;
        }

        GLuint CreateProgram(char *vertexShaderFilename, char *fragmentShaderFilename) {
            std::string vertexShaderCode = ReadFile(vertexShaderFilename);
            std::string fragmentShaderCode = ReadFile(fragmentShaderFilename);

            GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertexShaderCode, "vertex shader");
            GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderCode, "fragment shader");

            int result = 0;

            GLuint program = glCreateProgram();
            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);

            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &result);

            if (result == GL_FALSE) {
                int logLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<char> programLog(logLength);
                glGetProgramInfoLog(program, logLength, NULL, &programLog[0]);
                printf("ERR: Program Link Failed: %s\n", &programLog[0]);
                return 0;
            }

            return program;
        }

    };
}
