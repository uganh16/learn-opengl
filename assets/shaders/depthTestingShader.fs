#version 330 core

out vec4 FragColor;

float near = 0.1;
float far  = 100.0;

void main() {
  float z_ndc = gl_FragCoord.z * 2.0 - 1.0;
  float z = (2.0 * near * far) / (far + near - z_ndc * (far - near));
  FragColor = vec4(vec3(z / far), 1.0);
}
