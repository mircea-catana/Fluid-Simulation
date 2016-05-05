namespace Fluid {
    class Vec3 {
    private:
        float v[3];
    public:
        Vec3() {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
        }

        Vec3(const float x, const float y, const float z) {
            v[0] = x;
            v[1] = y;
            v[2] = z;
        }

        float magnitude() {
            return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }

        float sq_magnitude() {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }

        Vec3 normalize() {
            float mag = magnitude();
            return Vec3(v[0] / mag, v[1] / mag, v[2] / mag);
        }

        Vec3 negative() {
            return Vec3(-v[0], -v[1], -v[2]);
        }

        float dot(const Vec3 &rhs) {
            return v[0] * rhs.get_x() + v[1] * rhs.get_y() + v[2] * rhs.get_z();
        }

        Vec3 cross(const Vec3 &rhs) {
            return Vec3(v[1] * rhs.get_z() - v[2] * rhs.get_y(), v[2] * rhs.get_x() - v[0] * rhs.get_z(), v[0] * rhs.get_y() - v[1] * rhs.get_x());
        }

        Vec3 operator+ (const Vec3 &rhs) {
            return Vec3(v[0] + rhs.get_x(), v[1] + rhs.get_y(), v[2] + rhs.get_z());
        }

        Vec3 operator- (const Vec3 &rhs) {
            return Vec3(v[0] - rhs.get_x(), v[1] - rhs.get_y(), v[2] - rhs.get_z());
        }

        Vec3 operator* (const float &scalar) {
            return Vec3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
        }

        Vec3 operator* (const Vec3 &rhs) {
            return Vec3(v[0] * rhs.get_x(), v[1] * rhs.get_y(), v[2] * rhs.get_z());
        }

        Vec3 operator/ (const float &scalar) {
            return Vec3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
        }

        Vec3 operator/ (const Vec3 &rhs) {
            return Vec3(v[0] / rhs.get_x(), v[1] / rhs.get_y(), v[2] / rhs.get_z());
        }

        float& operator[] (const int &i) {
            return v[i];
        }

        Vec3* clone() {
            return new Vec3(v[0], v[1], v[2]);
        }

        float get_x() const {
            return v[0];
        }

        void set_x(const float x) {
            v[0] = x;
        }

        float get_y() const {
            return v[1];
        }

        void set_y(const float y) {
            v[1] = y;
        }

        float get_z() const {
            return v[2];
        }

        void set_z(const float z) {
            v[2] = z;
        }
    };
}
