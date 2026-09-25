#version 410

in vec3 pointColor;

layout (location = 0) out vec4 FragColor;

void main() {
    FragColor = vec4(pointColor, 1.0F);
}
