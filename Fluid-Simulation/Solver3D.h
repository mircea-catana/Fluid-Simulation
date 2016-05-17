namespace Fluid {

auto IJK = [](int i, int j, int k, int N) { return i + (N + 2)*j + (N + 2) * (N + 2) * k; };
#define FOR_CELL for (int x=1; x<=N; ++x) { \
                    for (int y=1; y<=N; ++y) { \
                        for (int z=1; z<=N; ++z) { \
                            int ind = IJK(x, y, z, N);
#define END_FOR_CELL }}}

    class Solver3D {
    private:

        void gauss_seidel(int N, float *d, float *d0, float diff, float divisor) {
            float div = 1.0f / divisor;

            for (unsigned k = 0; k < 20; ++k) {

                FOR_CELL
                    d[ind] = (d0[ind] + diff * (d[IJK(x - 1, y, z, N)] + d[IJK(x + 1, y, z, N)] +
                                                d[IJK(x, y - 1, z, N)] + d[IJK(x, y + 1, z, N)] +
                                                d[IJK(x, y, z - 1, N)] + d[IJK(x, y, z + 1, N)])) * div;
                END_FOR_CELL

            }
        }

        void diffuse(int N, float *d, float *d0, float diff, float dt) {
            float a = diff * dt * (N * N * N);
            gauss_seidel(N, d, d0, a, (1 + 6 * a));
        }

        void advect(int N, float *d, float *d0, float *vx, float *vy, float *vz, float dt) {
            float dt0 = dt * N;

            FOR_CELL
                float i = x - dt0 * vx[IJK(x, y, z, N)];
                float j = y - dt0 * vy[IJK(x, y, z, N)];
                float k = z - dt0 * vz[IJK(x, y, z, N)];

                if (i < 0.5f) i = 0.5f; else if (i>N + 0.5f) i = N + 0.5f;
                if (j < 0.5f) j = 0.5f; else if (j>N + 0.5f) j = N + 0.5f;
                if (k < 0.5f) k = 0.5f; else if (k>N + 0.5f) k = N + 0.5f;

                int i0 = (int)i, i1 = i0 + 1;
                int j0 = (int)j, j1 = j0 + 1;
                int k0 = (int)k, k1 = k0 + 1;

                float a1 = i - i0, a0 = 1 - a1;
                float b1 = j - j0, b0 = 1 - b1;
                float c1 = k - k0, c0 = 1 - c1;

                d[IJK(x, y, z, N)] = a0 * b0 * c0 * d0[IJK(i0, j0, k0, N)] +
                                     a0 * b0 * c1 * d0[IJK(i0, j0, k1, N)] +
                                     a0 * b1 * c0 * d0[IJK(i0, j1, k0, N)] +
                                     a0 * b1 * c1 * d0[IJK(i0, j1, k1, N)] +
                                     a1 * b0 * c0 * d0[IJK(i1, j0, k0, N)] +
                                     a1 * b0 * c1 * d0[IJK(i1, j0, k1, N)] +
                                     a1 * b1 * c0 * d0[IJK(i1, j1, k0, N)] +
                                     a1 * b1 * c1 * d0[IJK(i1, j1, k1, N)];
            END_FOR_CELL

        }

        void project(int N, float *vx, float *vy, float *vz, float *g, float *g0) {
            float h = 1.0f / N;

            FOR_CELL
                g0[ind] = -0.5f * h * (vx[IJK(x + 1, y, z, N)] - vx[IJK(x - 1, y, z, N)] +
                                       vy[IJK(x, y + 1, z, N)] - vy[IJK(x, y - 1, z, N)] +
                                       vz[IJK(x, y, z + 1, N)] - vz[IJK(x, y, z - 1, N)]);
                g[ind] = 0.0f;
            END_FOR_CELL

            gauss_seidel(N, g, g0, 1, 6);

            FOR_CELL
                vx[ind] -= 0.5 * N * (g[IJK(x + 1, y, z, N)] - g[IJK(x - 1, y, z, N)]);
                vy[ind] -= 0.5 * N * (g[IJK(x, y + 1, z, N)] - g[IJK(x, y - 1, z, N)]);
                vz[ind] -= 0.5 * N * (g[IJK(x, y, z + 1, N)] - g[IJK(x, y, z - 1, N)]);
            END_FOR_CELL
        }

    public:
        void density_step(int N, float *vx, float *vy, float *vz, float *d, float *d0, float diff, float dt) {
            std::swap(d0, d);
            diffuse(N, d, d0, diff, dt);

            std::swap(d0, d);
            advect(N, d, d0, vx, vy, vz, dt);
        }

        void velocity_step(int N, float *vx, float *vy, float *vz, float *vx0, float *vy0, float *vz0, float visc, float dt) {
            std::swap(vx0, vx);
            diffuse(N, vx, vx0, visc, dt);
            std::swap(vy0, vy);
            diffuse(N, vy, vy0, visc, dt);
            std::swap(vz0, vz);
            diffuse(N, vz, vz0, visc, dt);

            project(N, vx, vy, vz, vx0, vy0);

            std::swap(vx0, vx);
            std::swap(vy0, vy);
            std::swap(vz0, vz);
            advect(N, vx, vx0, vx0, vy0, vz0, dt);
            advect(N, vy, vy0, vx0, vy0, vz0, dt);
            advect(N, vz, vz0, vx0, vy0, vz0, dt);

            project(N, vx, vy, vz, vx0, vy0);
        }

        void vorticity(int N, float *vx, float *vy, float *vz, float dt, float Epsilon) {

            // PDE to calculate Curl using central differences
            auto Vortx = [=](int i, int j, int k) { return 0.5f*(vz[IJK(i, j+1, k, N)] - vz[IJK(i, j-1, k, N)] + vy[IJK(i, j, k-1, N)] - vy[IJK(i, j, k+1, N)])*dt; };
            auto Vorty = [=](int i, int j, int k) { return 0.5f*(vx[IJK(i, j, k+1, N)] - vx[IJK(i, j, k-1, N)] + vz[IJK(i-1, j, k, N)] - vz[IJK(i+1, j, k, N)])*dt; };
            auto Vortz = [=](int i, int j, int k) { return 0.5f*(vy[IJK(i+1, j, k, N)] - vy[IJK(i-1, j, k, N)] + vx[IJK(i, j-1, k, N)] - vx[IJK(i, j+1, k, N)])*dt; };
           
            //w = curl = gradient x field, nc = directional vector from less vorticity to more vorticity.
            // fb, vorticity confinement force.
            Vec3 w, nc, fb; 
            
            for (int x = 2; x <= N - 1; ++x) {
                for (int y = 2; y <= N - 1; ++y) {
                    for (int z = 2; z <= N - 1; ++z) {
                        int ind = IJK(x, y, z, N);
                        w.set_x( Vortx(x, y, z) );
                        w.set_y( Vorty(x, y, z) );
                        w.set_z( Vortz(x, y, z) );
                        float sigma = std::sqrtf( w.get_x()*w.get_x() + w.get_y()*w.get_y() + w.get_y()*w.get_y() );
                        if (sigma < 0.00000001) {
                            nc.set_x(0);
                            nc.set_y(0);
                            nc.set_z(0);
                        } else {
                            nc.set_x( -0.5*(Vortx(x + 1, y, z) - Vortx(x - 1, y, z))*dt / sigma );
                            nc.set_y( -0.5*(Vortx(x, y + 1, z) - Vortx(x, y - 1, z))*dt / sigma );
                            nc.set_z( -0.5*(Vortx(x, y, z + 1) - Vortx(x, y, z - 1))*dt / sigma );
                        }
                        fb = nc.cross(w)*(-Epsilon);
                        vx[ind] += fb.get_x();
                        vy[ind] += fb.get_y();
                        vz[ind] += fb.get_z();
                    }
                }
            }

        }

        void decay(int N, float *vx, float *vy, float *vz, float *vx0, float *vy0, float *vz0, float visc, float dt) {

        }


    };

#undef FOR_CELL
#undef END_FOR_CELL
}
