#version 410 core

in vec3 pointColor;
in vec3 fragPos;

layout (location = 0) out vec4 FragColor;

struct Light {
  vec3 direction;
  vec3 lightColor;
  vec3 viewPos;

  float constant;
  float linear;
  float quadratic;
  float cutOff;
  float outerCutOff;
};

uniform Light light;

void main() {

  vec3 lightTrace = normalize(light.viewPos - fragPos);

  float distance = length(light.viewPos - fragPos);
  float attenuation = 1.0F / (light.constant +
                              light.linear * distance +
                              light.quadratic * distance * distance);

  float theta = dot(lightTrace, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intencity = clamp((theta - light.outerCutOff) / epsilon, 0.0F, 1.0F);

  vec3 totalColor = intencity * attenuation * pointColor;

  FragColor = vec4(totalColor, 1.0F);
}
