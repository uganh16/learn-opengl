#version 330 core

out vec4 FragColor;

in vec2 fragTexCoord;

struct Material {
  sampler2D diffuse0;
};

uniform Material material;

uniform float outline;
uniform vec3 outlineColor;

void main() {
  FragColor = vec4(mix(texture(material.diffuse0, fragTexCoord).rgb, outlineColor, outline), 1.0);
}
