#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 cameraPos;

uniform samplerCube skybox;

void main() {
  vec3 V = normalize(cameraPos - fragPos);
  vec3 N = normalize(fragNormal);
  vec3 R = reflect(-V, N);
  FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
