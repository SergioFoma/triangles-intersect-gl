#version 410

in vec3 pointColor;
in vec3 normal;
in vec3 fragPos;

layout (location = 0) out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    
    float ambientStrength = 0.1F;
  
    vec3 lightTrace = normalize(lightPos - fragPos);
    float diffStrength = abs(dot(normal, lightTrace));
  
    vec3 diff = diffStrength * lightColor;
    vec3 ambient = ambientStrength * lightColor;
     
    float specularStrength = 0.5F;
    vec3 viewTrace = normalize(viewPos - fragPos);
    
    vec3 correctNormal = normal;
    if (dot(correctNormal, viewTrace) < 0.0F) {
      correctNormal = -correctNormal;
    }


    vec3 reflectTrace = reflect(-lightTrace, correctNormal);
    float spec = pow(abs(dot(viewTrace, reflectTrace)), 32.0F);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 totalColor = (ambient + diff + specular) * pointColor;

    FragColor = vec4(totalColor, 1.0F);
}
