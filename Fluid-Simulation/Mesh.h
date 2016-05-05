namespace Fluid {
    class Mesh {
    private:
        unsigned int vao;
        unsigned int vbo, ivbo;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    public:
        Mesh() {
            vertices = std::vector<Vertex>();
            indices = std::vector<unsigned int>();
            vertices.clear();
            indices.clear();
        }

        ~Mesh() {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ivbo);
        }

        void CreateGrid(int width, int height) {
            vertices.clear();
            for (unsigned y = 0; y < height; ++y) {
                for (unsigned x = 0; x < width; ++x) {
                    // TODO: CHANGE TO ACTUAL COLOR
                    vertices.push_back(Vertex(Vec3((float)x / (width - 1) * 2 - 1.0f, (float)y / (height - 1) * 2 - 1.0f, 0.0f),
                                       Vec4(rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f, 1.0f)));
                }
            }

            indices.clear();
            for (unsigned y = 0; y < height - 1; ++y) {
                for (unsigned x = 0; x < width - 1; ++x) {
                    // Triangle 1
                    indices.push_back(y*width + x);
                    indices.push_back(y*width + x + 1);
                    indices.push_back((y + 1)*width + x);

                    // Triangle 2
                    indices.push_back(y*width + x + 1);
                    indices.push_back((y + 1)*width + x + 1);
                    indices.push_back((y + 1)*width + x);
                }
            }

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ivbo);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vec3)));

           
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
        }

        unsigned int get_vao() {
            return vao;
        }

        unsigned int get_vbo() {
            return vbo;
        }

        unsigned int get_ivbo() {
            return ivbo;
        }

        std::vector<unsigned int>* get_indices() {
            return &indices;
        }

    };
}
