#version 410 core
in vec3 lineColor;
out vec4 fragColor;
void main() { fragColor = vec4(lineColor, 1.0); }
