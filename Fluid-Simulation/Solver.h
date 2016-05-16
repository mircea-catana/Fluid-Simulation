//from Jos Stam paper to test our fluid engine.

namespace Fluid {
    class Solver {
    private:
        void set_boundary(int N, int b, float * x) {
            auto IX = [=](int i, int j) { return i + (N + 2)*j; };

            for (int i = 1; i <= N; i++) {
                x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
                x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
                x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
                x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
            }

            x[IX(0, 0)] = 0.5f*(x[IX(1, 0)] + x[IX(0, 1)]);
            x[IX(0, N + 1)] = 0.5f*(x[IX(1, N + 1)] + x[IX(0, N)]);
            x[IX(N + 1, 0)] = 0.5f*(x[IX(N, 0)] + x[IX(N + 1, 1)]);
            x[IX(N + 1, N + 1)] = 0.5f*(x[IX(N, N + 1)] + x[IX(N + 1, N)]);
        }

        void gauss_siedel(int N, int b, float * x, float * x0, float a, float c) {
            auto IX = [=](int i, int j) { return i + (N + 2)*j; };

            for (int k = 0; k < 20; k++) {
                for (int i = 1; i <= N; i++) {
                    
            //concurrency::parallel_for(size_t(0), size_t(20), [&](size_t ky) {
                //concurrency::parallel_for(size_t(1), size_t(N+1), [&](size_t i) {
                    for (int j = 1; j <= N; j++) {
                        x[IX(i, j)] = (x0[IX(i, j)] + a*(x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / c;
                    }

                    set_boundary(N, b, x);
                //});
            //});

                }
            }
        }

        void diffusion(int N, int b, float * x, float * x0, float diff, float dt) {
            float a = dt * diff * (N * N);
            gauss_siedel(N, b, x, x0, a, 1 + 4 * a);
        }

        void advection_step(int N, int b, float * d, float * d0, float * u, float * v, float dt) {
            auto IX = [=](int i, int j) { return i + (N + 2)*j; };

            float dt0 = dt*N;
            for (int i = 1; i <= N; i++) {
                for (int j = 1; j <= N; j++) {
                    // (x, y) is the address to copy from
                    // this is Forward Euler method:
                    //(other methods could be: Modified Euler and RK2)
                    float x = i - dt0*u[IX(i, j)], y = j - dt0*v[IX(i, j)];

                    // clamp x and y
                    if (x<0.5f) x = 0.5f; else if (x>N + 0.5f) x = N + 0.5f;
                    if (y<0.5f) y = 0.5f; else if (y>N + 0.5f) y = N + 0.5f;

                    // s1 and s0 are lerp coordinates [0,1) within the source cell
                    // t1 and t0 are lerp coordinates as well
                    // with s and t we got the bilinear interpolation we need (s0 = 1-s1, t0 = 1-t1)
                    int i0 = (int)x, i1 = i0 + 1;
                    int j0 = (int)y, j1 = j0 + 1;
                    float s1 = x - i0, s0 = 1 - s1;
                    float t1 = y - j0, t0 = 1 - t1;

                    // sample the source
                    d[IX(i, j)] = s0*(t0*d0[IX(i0, j0)] + t1*d0[IX(i0, j1)]) + s1*(t0*d0[IX(i1, j0)] + t1*d0[IX(i1, j1)]);
                }
            }

            // copy values out to the boundary.
            set_boundary(N, b, d);
        }

        void project(int N, float * u, float * v, float * p, float * div) {
            auto IX = [=](int i, int j) { return i + (N + 2)*j; };

            // calculate divergence into div
            // set initial value of p
            for (int i = 1; i <= N; i++) {
                for (int j = 1; j <= N; j++) {
                    div[IX(i, j)] = -0.5f*(u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]) / N;
                    p[IX(i, j)] = 0;
                }
            }

            // copy pixels to boundary
            set_boundary(N, 0, div);
            set_boundary(N, 0, p);

            // p += div[x+/-1, y+/-1] * 4;
            gauss_siedel(N, 0, p, div, 1, 4);

            // calculate velocity from pressure-like "p"
            for (int i = 1; i <= N; i++) {
                for (int j = 1; j <= N; j++) {
                    // u from left and right
                    u[IX(i, j)] -= 0.5f*N*(p[IX(i + 1, j)] - p[IX(i - 1, j)]);

                    // v from up and down.
                    v[IX(i, j)] -= 0.5f*N*(p[IX(i, j + 1)] - p[IX(i, j - 1)]);
                }
            }

            // copy velocity to boundary
            set_boundary(N, 1, u);
            set_boundary(N, 2, v);
        }

    public:
        void density_step(int N, float * x, float * x0, float * u, float * v, float diff, float dt) {
            // apply diffusion to density. If there is no velocity, the value will still spread.
            std::swap(x0, x);
            diffusion(N, 1, x, x0, diff, dt);

            // use the velocity field to carry density around.
            std::swap(x0, x);
            advection_step(N, 0, x, x0, u, v, dt);
        }

        void velocity_step(int N, float * u, float * v, float * u0, float * v0, float visc, float dt) {
            // diffuse into neighouring cells
            std::swap(u0, u);
            diffusion(N, 1, u, u0, visc, dt);
            std::swap(v0, v);
            diffusion(N, 2, v, v0, visc, dt);

            // stabilise the system using poisson
            project(N, u, v, u0, v0);
            std::swap(u0, u);
            std::swap(v0, v);

            // use advection to move the velocity itself
            advection_step(N, 1, u, u0, u0, v0, dt);
            advection_step(N, 2, v, v0, u0, v0, dt);

            // stabilise the system using poisson
            project(N, u, v, u0, v0);
        }
    };
}
