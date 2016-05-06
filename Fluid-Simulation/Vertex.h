namespace Fluid {
    class Vertex {
    private:
        Vec3 position;
        Vec4 color;
    public:
        Vertex() {
            position = Vec3();
            color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        Vertex(Vec3 pos) {
            position = pos;
        }

        Vertex(Vec3 pos, Vec4 color) {
            this->position = pos;
            this->color = color;
        }

        Vec3 get_position() {
            return position;
        }

        void set_position(Vec3 pos) {
            position = pos;
        }

        Vec4 get_color() {
            return color;
        }

        void set_color(Vec4 color) {
            float r = (color.get_x() > 1.0f) ? 1.0f : color.get_x();
            float g = (color.get_y() > 1.0f) ? 1.0f : color.get_y();
            float b = (color.get_z() > 1.0f) ? 1.0f : color.get_z();
            float a = (color.get_w() > 1.0f) ? 1.0f : color.get_w();
            this->color = Vec4(r, g, b, a);
        }

    };
}
