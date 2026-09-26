#version 410

in vec3 pointColor;

layout (location = 0) out vec4 FragColor;

uniform vec3 lightColor;

void main() {
    vec3 totalColor = lightColor * pointColor;

    FragColor = vec4(totalColor, 1.0F);
}
