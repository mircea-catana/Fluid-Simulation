namespace Fluid {
    class Vec4 {
    private:
        float v[4];
    public:
        Vec4() {
            v[0] = 0;
            v[1] = 0;
            v[2] = 0;
            v[3] = 0;
        }

        Vec4(const float x, const float y, const float z, const float w) {
            v[0] = x;
            v[1] = y;
            v[2] = z;
            v[3] = w;
        }

        Vec4(const Vec3 v, const float w = 0) {
            this->v[0] = v.get_x();
            this->v[1] = v.get_y();
            this->v[2] = v.get_z();
            this->v[3] = w;
        }

        float magnitude() {
            return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
        }

        float sq_magnitude() {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        }

        Vec4 normalize() {
            float mag = magnitude();
            return Vec4(v[0] / mag, v[1] / mag, v[2] / mag, v[3] / mag);
        }

        Vec4 negative() {
            return Vec4(-v[0], -v[1], -v[2], -v[3]);
        }

        float dot(const Vec4 &rhs) {
            return v[0] * rhs.get_x() + v[1] * rhs.get_y() + v[2] * rhs.get_z() + v[3] * rhs.get_w();
        }

        Vec3 xyz() {
            return Vec3(v[0], v[1], v[2]);
        }

        Vec4 xxxx() const {
            return Vec4(v[0], v[0], v[0], v[0]);
        }

        Vec4 yyyy() const {
            return Vec4(v[1], v[1], v[1], v[1]);
        }

        Vec4 zzzz() const {
            return Vec4(v[2], v[2], v[2], v[2]);
        }

        Vec4 wwww() const {
            return Vec4(v[3], v[3], v[3], v[3]);
        }

        Vec4 operator+ (const Vec4 &rhs) {
            return Vec4(v[0] + rhs.get_x(), v[1] + rhs.get_y(), v[2] + rhs.get_z(), v[3] + rhs.get_w());
        }

        Vec4 operator- (const Vec4 &rhs) {
            return Vec4(v[0] - rhs.get_x(), v[1] - rhs.get_y(), v[2] - rhs.get_z(), v[3] - rhs.get_w());
        }

        Vec4 operator* (const float &scalar) {
            return Vec4(v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar);
        }

        Vec4 operator* (const Vec4 &rhs) {
            return Vec4(v[0] * rhs.get_x(), v[1] * rhs.get_y(), v[2] * rhs.get_z(), v[3] * rhs.get_w());
        }

        Vec4 operator/ (const float &scalar) {
            return Vec4(v[0] / scalar, v[1] / scalar, v[2] / scalar, v[3] / scalar);
        }

        Vec4 operator/ (const Vec4 &rhs) {
            return Vec4(v[0] / rhs.get_x(), v[1] / rhs.get_y(), v[2] / rhs.get_z(), v[3] / rhs.get_w());
        }

        float& operator[] (const int &i) {
            return v[i];
        }

        const float& operator[] (const int &i) const {
            return v[i];
        }

        Vec4* clone() {
            return new Vec4(v[0], v[1], v[2], v[3]);
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

        float get_w() const {
            return v[3];
        }

        void set_w(const float w) {
            v[3] = w;
        }
    };
}
