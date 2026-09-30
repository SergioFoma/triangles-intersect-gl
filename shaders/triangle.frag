#version 410

in vec3 pointColor;
in vec3 normal;
in vec3 fragPos;

layout (location = 0) out vec4 FragColor;

struct Light {
  vec3 lightPos;
  vec3 direction;
  vec3 lightColor;
  vec3 viewPos;

  float constant;
  float linear;
  float quadratic;
  float cutOff;
};

uniform Light light;

void main() {
      
    // ===============================================================
    float distance = length(light.lightPos - fragPos);
    float attenuation = 1.0F / (light.constant + 
                                light.linear * distance + 
                                light.quadratic * distance * distance);
  
    // ================================================================
    float ambientStrength = 0.1F;
  
    vec3 lightTrace = normalize(light.lightPos - fragPos);
    float diffStrength = abs(dot(normal, lightTrace));
  
    vec3 diff = diffStrength * light.lightColor;
    vec3 ambient = ambientStrength * light.lightColor;
     
    float specularStrength = 0.5F;
    vec3 viewTrace = normalize(light.viewPos - fragPos);
    
    vec3 correctNormal = normal;
    if (dot(correctNormal, viewTrace) < 0.0F) {
      correctNormal = -correctNormal;
    }


    vec3 reflectTrace = reflect(-lightTrace, correctNormal);
    float spec = pow(abs(dot(viewTrace, reflectTrace)), 32.0F);
    vec3 specular = specularStrength * spec * light.lightColor;  

    ambient  *= attenuation;
    diff     *= attenuation;
    specular *= attenuation;

    vec3 totalColor;
    // ==========================================================
    float theta = dot(lightTrace, normalize(-light.direction));

    if (theta > light.cutOff) {
      totalColor = (ambient + diff + specular) * pointColor;
    } else {
      totalColor = ambient * pointColor;
    }
    // ===========================================================

    FragColor = vec4(totalColor, 1.0F);
}
