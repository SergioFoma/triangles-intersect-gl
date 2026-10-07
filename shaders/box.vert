#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 mortonColor;
uniform int mortonMode;
out vec3 lineColor;
uniform mat4 view;
uniform mat4 projection;
void main() {
    lineColor = mortonMode != 0 ? mortonColor : color;
    gl_Position = projection * view * vec4(position, 1.0);
}
