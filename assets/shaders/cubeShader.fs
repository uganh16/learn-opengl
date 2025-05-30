#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 FragColor;

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main() {
  vec3 normal = normalize(fragNormal);
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);

  vec3 ambient = light.ambient * material.ambient;
  vec3 diffuse = light.diffuse * (material.diffuse * max(dot(normal, lightDir), 0.0));
  vec3 specular = light.specular * (material.specular * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess));

  vec3 resultColor = ambient + diffuse + specular;
  FragColor = vec4(resultColor, 1.0);
}
