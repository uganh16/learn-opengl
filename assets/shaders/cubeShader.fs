#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 FragColor;

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material {
  sampler2D diffuse;
  sampler2D specular;
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

  vec3 ambient = light.ambient * texture(material.diffuse, fragTexCoord).rgb;
  vec3 diffuse = light.diffuse * (texture(material.diffuse, fragTexCoord).rgb * max(dot(normal, lightDir), 0.0));
  vec3 specular = light.specular * (texture(material.specular, fragTexCoord).rgb * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess));

  FragColor = vec4(ambient + diffuse + specular, 1.0);
}
