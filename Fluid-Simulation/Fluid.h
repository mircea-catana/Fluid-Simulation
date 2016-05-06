namespace Fluid {
    class Fluid {
    private:
        int gridWidth;
        int gridHeight;

        std::vector<float> density;
        std::vector<Vec3> forces;

        Vec3 rgb;
        std::vector<float> alphas;

    public:
        Fluid(int width, int height) {
            gridWidth = width;
            gridHeight = height;
            density = std::vector<float>(width*height);
            forces = std::vector<Vec3>(width*height);
            rgb = Vec3(0.8f, 0.4f, 0.1f);
        }

        void random_fill() {
            for (unsigned y = 0; y < gridHeight; ++y) {
                for (unsigned x = 0; x < gridWidth; ++x) {
                    density[XY(y, x, gridWidth)] = rand() % 100 / 100.0f;
                }
            }
        }

        std::vector<float>* fluid_density(int meshWidth, int meshHeight) {
            alphas = std::vector<float>(meshWidth*meshHeight);

            // Mapping Fluid Grid to Mesh Grid
            float wFactor = (float)gridWidth / (meshWidth+1);
            float hFactor = (float)gridHeight / (meshHeight+1);
            float wIndex = 0.0f;
            float hIndex = 0.0f;

            for (unsigned y = 0; y < meshHeight; ++y) {
                int closestY = round(hIndex);
                closestY = (closestY < 0) ? 0 : closestY;
                closestY = (closestY > gridHeight - 1) ? gridHeight - 1 : closestY;
                for (unsigned x = 0; x < meshWidth; ++x) {
                    int closestX = round(wIndex);
                    closestX = (closestX < 0) ? 0 : closestX;
                    closestX = (closestX > gridWidth - 1) ? gridWidth - 1 : closestX;
                    alphas[XY(y, x, meshWidth)] = density[XY(closestY, closestX, gridWidth)];
                    wIndex += wFactor;
                }
                wIndex = 0.0f;
                hIndex += hFactor;
            }

            return &alphas;
        }

        Vec3 get_rgb() {
            return rgb;
        }

        void set_rgb(Vec3 rgb) {
            this->rgb = rgb;
        }
    };
}
