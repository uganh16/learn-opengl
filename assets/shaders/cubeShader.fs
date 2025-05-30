#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
  vec3 normal = normalize(fragNormal);
  vec3 lightDir = normalize(lightPos - fragPos);
  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);

  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * lightColor;

  vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;

  float specularStrength = 0.5;
  vec3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;

  vec3 resultColor = (ambient + diffuse + specular) * objectColor;
  FragColor = vec4(resultColor, 1.0);
}
