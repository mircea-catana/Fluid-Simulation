namespace Fluid {
    class Fluid3D {
    private:
        int gridSize;

        std::vector<float> vx;
        std::vector<float> prev_vx;

        std::vector<float> vy;
        std::vector<float> prev_vy;

        std::vector<float> vz;
        std::vector<float> prev_vz;

        std::vector<float> density;
        std::vector<float> prev_density;

        Vec3 rgb;
        std::vector<float> alphas;

        Solver3D solver;

        int clamp(int value, int min, int max) {
            int result = (value < min) ? min : value;
            result = (value > max) ? max : value;
            return result;
        }

    public:
        Fluid3D(int size) {
            gridSize = size;

            vx.resize(gridSize * gridSize * gridSize);
            vy.resize(gridSize * gridSize * gridSize);
            vz.resize(gridSize * gridSize * gridSize);

            prev_vx.resize(gridSize * gridSize * gridSize);
            prev_vy.resize(gridSize * gridSize * gridSize);
            prev_vz.resize(gridSize * gridSize * gridSize);

            density.resize(gridSize * gridSize * gridSize);
            prev_density.resize(gridSize * gridSize * gridSize);

            rgb = Vec3(0.6f, 0.7f, 0.9f);

            solver = Solver3D();
        }

        void clear() {
            for (unsigned i = 0; i < gridSize*gridSize*gridSize; ++i) {
                density[i] = prev_density[i] = 0.0f;
                vx[i] = vy[i] = vz[i] = 0.0f;
                prev_vx[i] = prev_vy[i] = prev_vz[0] = 0.0f;
            }
        }

        std::vector<float>* fluid_density(int meshWidth, int meshHeight) {
            if (alphas.size() != meshWidth * meshHeight) {
                alphas = std::vector<float>(meshWidth * meshHeight);
            }

            float wFactor = (float)gridSize / (meshWidth + 1);
            float hFactor = (float)gridSize / (meshHeight + 1);
            float wIndex = 0.0f;
            float hIndex = 0.0f;

            for (unsigned y = 0; y < meshHeight; ++y) {
                int closestY = clamp(round(hIndex), 0, gridSize - 1);
                for (unsigned x = 0; x < meshWidth; ++x) {
                    int closestX = clamp(round(wIndex), 0, gridSize - 1);

                    // Add up along Z
                    float sum = 0.0f;
                    for (unsigned z = 0; z < gridSize; ++z) {
                        sum += density[XYZ(z, closestY, closestX, gridSize)];
                    }
                    sum /= gridSize;

                    alphas[XY(y, x, meshWidth)] = sum;
                    wIndex += wFactor;
                }
                wIndex = 0.0f;
                hIndex += hFactor;
            }

            return &alphas;
        }

        void update_stam(float frameNumber) {
            // Assume 30 fps for now
            float dt = 1.0f / 30.0f;

            int N = gridSize - 2;

            float *x = vx.data(), *y = vy.data(), *z = vz.data();
            float *x0 = prev_vx.data(), *y0 = prev_vy.data(), *z0 = prev_vz.data();
            float *dens = density.data(), *dens0 = prev_density.data();

            float visc = 0.0f;
            float diff = 0.0f;

            std::fill(prev_vx.begin(), prev_vx.end(), 0.0f);
            std::fill(prev_vy.begin(), prev_vy.end(), 0.0f);
            std::fill(prev_vz.begin(), prev_vz.end(), 0.0f);
            std::fill(prev_density.begin(), prev_density.end(), 0.0f);

            float s = sin(frameNumber * 0.01f);
            float c = cos(frameNumber * 0.01f);

            int hSize = gridSize / 2;

            // Sources
            density[XYZ(hSize, hSize, hSize, gridSize)] += gridSize * dt * 3.0f;
            x[XYZ(hSize, hSize, hSize, gridSize)] += s * gridSize * dt;
            y[XYZ(hSize, hSize, hSize, gridSize)] += c * gridSize * dt;
            z[XYZ(hSize, hSize, hSize, gridSize)] += (1.0f - (c + 1.0f) / 2.0f) * gridSize * dt;
            // End Sources
            
           // solver.vorticity(N, x, y, z, dt, 150);
            solver.velocity_step(N, x, y, z, x0, y0, z0, visc, dt);
            solver.density_step(N, x, y, z, dens, dens0, diff, dt);
           // solver.decay(N, dens, dt, 0.1); 
        }

        Vec3 get_rgb() {
            return rgb;
        }

        void set_rgb(Vec3 rgb) {
            this->rgb = rgb;
        }
    };
}

