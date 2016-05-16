namespace Fluid {
    class Fluid {
    private:
        int gridWidth;
        int gridHeight;

        std::vector<float> density;
        std::vector<Vec3> forces;

        // to test our mesh
        std::vector<float> prev_density;
        std::vector<float> prev_vx;
        std::vector<float> prev_vy;
        //std::vector<float> density;
        std::vector<float> vx;
        std::vector<float> vy;

        std::vector<float> temp;
        std::vector<float> prev_temp;

        Vec3 rgb;
        std::vector<float> alphas;

        Solver solver;

    public:
        Fluid(int width, int height) {
            gridWidth = width;
            gridHeight = height;
            // density = std::vector<float>(width*height);
            forces = std::vector<Vec3>(width*height);

            //to test our mesh
            density.resize((width)*(height));
            vx.resize((width)*(height));
            vy.resize((width)*(height));

            prev_density.resize((width)*(height));
            prev_vx.resize((width)*(height));
            prev_vy.resize((width)*(height));

            temp.resize(width*height);
            prev_temp.resize(width*height);

            rgb = Vec3(0.6f, 0.7f, 0.9f);
            solver = Solver();
        }

        void clear() {
            for (unsigned y = 0; y < gridHeight; ++y) {
                for (unsigned x = 0; x < gridWidth; ++x) {
                    density[XY(y, x, gridWidth)] = 0.0;
                    // Equivalent of 27 deg Celsius on Kelvin scale
                    temp[XY(y, x, gridWidth)] = 300;
                }
            }
        }

        std::vector<float>* fluid_density(int meshWidth, int meshHeight) {
            if (alphas.size() != meshWidth*meshHeight) {
                alphas = std::vector<float>(meshWidth*meshHeight);
            }

            // Mapping Fluid Grid to Mesh Grid
            float wFactor = (float)gridWidth / (meshWidth + 1);
            float hFactor = (float)gridHeight / (meshHeight + 1);
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

        void updateStam(float frame_number) {
            float dt = 1.0f / 30;
            int N = gridWidth - 2;
            float *u = vx.data(), *v = vy.data(), *u_prev = prev_vx.data(), *v_prev = prev_vy.data();
            float *dens = density.data(), *dens_prev = prev_density.data();
            //float *tmp = temp.data(), *prev_tmp = prev_temp.data();
            float visc = 0.0f;
            float diff = 0.0f;

            std::fill(prev_vx.begin(), prev_vx.end(), 0.0f);
            std::fill(prev_vy.begin(), prev_vy.end(), 0.0f);
            std::fill(prev_density.begin(), prev_density.end(), 0.0f);
            //  std::fill(prev_temp.begin(), prev_temp.end(), 0.0f);

            float c = sin(frame_number*0.001f);
            float s = cos(frame_number*0.001f);

            int hw = gridWidth / 2;
            int hh = gridHeight / 2;

            density[XY(hh, hw, gridWidth)] += gridWidth * dt;
            u[XY(hh, hw, gridWidth)] += c * (gridWidth * dt);
            v[XY(hh, hw, gridWidth)] += s * (gridWidth * dt);

            // Adding temperature influence to verical velocity
   /*         float alpha = 1.0, beta = 1.0;
            for (unsigned i = 0; i < gridWidth*gridHeight; ++i) {
                float f = (alpha * density[i] - beta * (temp[i] - 273)) * -9.81;
                v[i] += f * dt * 0.001;
            }
*/

            solver.someVorticity(N, u, v, dt, 0.5);


            solver.velocity_step(N, u, v, u_prev, v_prev, visc, dt);
            solver.density_step(N, dens, dens_prev, u, v, diff, dt);
            //solver.density_step(N, tmp, prev_tmp, u, v, diff, dt);
        }

        Vec3 get_rgb() {
            return rgb;
        }

        void set_rgb(Vec3 rgb) {
            this->rgb = rgb;
        }
    };
}
