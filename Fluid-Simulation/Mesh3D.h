namespace Fluid {
    class Mesh3D {
    private:
        unsigned int vao;
        unsigned int vbo, ivbo;
        int meshSize;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    public:
        Mesh3D() {
            vertices = std::vector<Vertex>();
            indices = std::vector<unsigned int>();
            vertices.clear();
            indices.clear();
        }

        ~Mesh3D() {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ivbo);
        }

        /*
            Creating a cube with the following front and back faces:
            1 -- 2            5 -- 6
            |    |  - Front   |    |  - Back
            3 -- 4            7 -- 8
            
            Diagonals are considered between lower left and upper right vertices
            as the camera looks at the face, resulting in the following triangles:
            (order is important for normal accuracy)
            F: A) 123 B) 243
            R: A) 264 B) 468
            B: A) 586 C) 578
            L: A) 175 B) 137
            U: A) 156 B) 162
            D: A) 347 B) 487
        */
        void CreateCube(int size) {
            meshSize = size;

            vertices.clear();
            for (unsigned z = 0; z < size; ++z) {
                for (unsigned y = 0; y < size; ++y) {
                    for (unsigned x = 0; x < size; ++x) {
                        Vec3 vPos = Vec3((float)x / (size - 1) * 2 - 1.0f, (float)y / (size - 1) * 2 - 1.0f, (float)z / (size - 1) * 2 - 1.0f);
                        vertices.push_back(Vertex(vPos, Vec4(1.0f, 1.0f, 1.0f, 1.0f)));
                    }
                }
            }

            indices.clear();
            int size2 = size*size;
            for (unsigned z = 0; z < size - 1; ++z) {
                for (unsigned y = 0; y < size - 1; ++y) {
                    for (unsigned x = 0; x < size - 1; ++x) {
                        // Front
                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z)   *size2 + (y)   *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x));

                        indices.push_back((z)   *size2 + (y)   *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x));

                        // Right
                        indices.push_back((z)   *size2 + (y)   *size + (x+1));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));

                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));

                        // Back
                        indices.push_back((z+1) *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));

                        indices.push_back((z+1) *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));

                        // Left
                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x));

                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z)   *size2 + (y+1) *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));

                        // Up
                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));

                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));
                        indices.push_back((z)   *size2 + (y)   *size + (x+1));

                        // Down
                        indices.push_back((z)   *size2 + (y+1) *size + (x));
                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));

                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));
                    }
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
            glBindVertexArray(0);
        }

        void CreateQuadCube(int size) {
            meshSize = size;

            vertices.clear();
            for (unsigned z = 0; z < size; ++z) {
                for (unsigned y = 0; y < size; ++y) {
                    for (unsigned x = 0; x < size; ++x) {
                        Vec3 vPos = Vec3((float)x / (size - 1) * 2 - 1.0f, (float)y / (size - 1) * 2 - 1.0f, (float)z / (size - 1) * 2 - 1.0f);
                        vertices.push_back(Vertex(vPos, Vec4(1.0f, 1.0f, 1.0f, 1.0f)));
                    }
                }
            }

            indices.clear();
            int size2 = size*size;
            for (unsigned z = 0; z < size - 1; ++z) {
                for (unsigned y = 0; y < size - 1; ++y) {
                    for (unsigned x = 0; x < size - 1; ++x) {
                        // Front
                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z)   *size2 + (y)   *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x));

                        // Right
                        indices.push_back((z)   *size2 + (y)   *size + (x+1));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));
                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));

                        // Back
                        indices.push_back((z+1) *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));

                        // Left
                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z)   *size2 + (y+1) *size + (x));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x));

                        // Up
                        indices.push_back((z)   *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x));
                        indices.push_back((z+1) *size2 + (y)   *size + (x+1));
                        indices.push_back((z)   *size2 + (y)   *size + (x+1));

                        // Down
                        indices.push_back((z)   *size2 + (y+1) *size + (x));
                        indices.push_back((z)   *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1) *size2 + (y+1) *size + (x+1));
                        indices.push_back((z+1)   *size2 + (y+1) *size + (x));
                    }
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
            glBindVertexArray(0);
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

        std::vector<Vertex>* get_vertices() {
            return &vertices;
        }

        int get_mesh_size() {
            return meshSize;
        }

    };
}
