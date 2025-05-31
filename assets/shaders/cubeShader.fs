#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 FragColor;

struct DirectionalLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  float constant;
  float linear;
  float quadratic;

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

#define NR_POINT_LIGHTS 4

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

vec3 BlinnPhong(vec3 L_a, vec3 L_d, vec3 L_s, vec3 N, vec3 L, vec3 V) {
  vec3 H = normalize(L + V);

  vec3 k_d = texture(material.diffuse, fragTexCoord).rgb;
  vec3 k_a = k_d;
  vec3 k_s = texture(material.specular, fragTexCoord).rgb;

  vec3 I_a = k_a * L_a;
  vec3 I_d = k_d * max(dot(N, L), 0.0) * L_d;
  vec3 I_s = k_s * pow(max(dot(N, H), 0.0), material.shininess) * L_s;

  return I_a + I_d + I_s;
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  return BlinnPhong(light.ambient, light.diffuse, light.specular, normal, lightDir, viewDir);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float dist = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
  return attenuation * BlinnPhong(light.ambient, light.diffuse, light.specular, normal, lightDir, viewDir);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float dist = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  return intensity * attenuation * BlinnPhong(light.ambient, light.diffuse, light.specular, normal, lightDir, viewDir);
}

void main() {
  vec3 normal = normalize(fragNormal);
  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 result = calcDirectionalLight(directionalLight, normal, viewDir);
  for (int i = 0; i < NR_POINT_LIGHTS; i++) {
    result += calcPointLight(pointLights[i], normal, viewDir);
  }
  result += calcSpotLight(spotLight, normal, viewDir);

  FragColor = vec4(result, 1.0);
}
