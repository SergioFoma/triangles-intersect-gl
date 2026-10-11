#version 410 core

in vec3 TexCoords;

layout (location = 0) out vec4 FragColor;

uniform samplerCube skybox;

void main() {
  
  FragColor = texture(skybox, TexCoords);
}
