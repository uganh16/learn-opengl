#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 fragTexCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
  /* We want the skybox to be centered around the player so that no matter how
   * far the player moves, the skybox won't get any closer, giving the
   * impression the surrounding environment is extremely large. The view matrix
   * however transforms all the skybox's positions by rotating and translating
   * them, so if the player moves, the cubemap moves as well! We want to remove
   * the translation part of the view matrix so only rotation will affect the
   * skybox's position vectors. */
  vec4 pos = projectionMatrix * vec4(mat3(viewMatrix) * aPos, 1.0);
  /* We need to trick the depth buffer into believing that the skybox has the
   * maximum depth value of 1.0 so that it fails the depth test wherever there's
   * a different object in front of it. We can set the z component of the output
   * position equal to its w component which will result in a z component that
   * is always equal to 1.0, because when the perspective division is applied
   * its z component translates to w / w = 1.0. */
  gl_Position = pos.xyww;
  fragTexCoord = aPos;
}
