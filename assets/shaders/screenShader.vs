#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 fragTexCoord;

uniform mat3 transform;

void main() {
  gl_Position = vec4((transform * vec3(aPos, 1.0)).xy, 0.0, 1.0);
  fragTexCoord = aTexCoord;
}
