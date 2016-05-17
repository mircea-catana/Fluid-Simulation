#version 430 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

out vec4 color;

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 viewToProjection;

void main() {
    mat4 modelToView = worldToView * modelToWorld;
    vec4 viewPos = modelToView * vec4(in_position, 1.0);
    gl_Position = viewToProjection * viewPos;
    color = in_color;
}
