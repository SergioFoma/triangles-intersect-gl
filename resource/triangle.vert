#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 pointColor;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0F);
    fragPos = vec3(model * vec4(aPos, 1.0F));
    pointColor = aColor;
    normal = normalMatrix * aNormal;
}

