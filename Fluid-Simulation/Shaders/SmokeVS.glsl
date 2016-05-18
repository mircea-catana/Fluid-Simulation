#version 430 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

out vec4 color;

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 viewToProjection;
uniform float frameNumber;

void main() {

    float angle = mod(frameNumber, 360);

    mat4 yrot = mat4(vec4(cos(angle), 0.0, sin(angle), 0.0),
                     vec4(0.0, 1.0, 0.0, 0.0),
                     vec4(-sin(angle), 0.0, cos(angle), 0.0),
                     vec4(0.0, 0.0, 0.0, 1.0));

    mat4 rotatedModel = modelToWorld * yrot;

    mat4 modelToView = worldToView * rotatedModel;
    vec4 viewPos = modelToView * vec4(in_position, 1.0);
    gl_Position = viewToProjection * viewPos;
    color = in_color;
}
