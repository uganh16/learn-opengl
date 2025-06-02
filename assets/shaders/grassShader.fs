#version 330 core

out vec4 FragColor;

in vec2 fragTexCoord;

struct Material {
  sampler2D diffuse0;
};

uniform Material material;

void main() {
  vec4 texColor = texture(material.diffuse0, fragTexCoord);
  if (texColor.a < 0.1) {
    discard;
  }
  FragColor = texColor;
}
