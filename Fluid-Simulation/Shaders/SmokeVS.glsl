#version 430 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

out vec4 color;

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 viewToProjection;

void main() {
    mat4 rot = mat4(vec4(cos(45.0), 0.0, sin(45.0), 0.0), 
                    vec4(0.0, 1.0, 0.0, 0.0f),
                    vec4(-sin(45.0), 0.0, cos(45.0), 0.0),
                    vec4(0.0, 0.0, 0.0, 1.0));
    mat4 x = modelToWorld * rot;
    mat4 modelToView = worldToView * modelToWorld;
    vec4 viewPos = modelToView * vec4(in_position, 1.0);
    gl_Position = viewToProjection * viewPos;
    color = in_color;
}
