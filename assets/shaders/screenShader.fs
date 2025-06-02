#version 330 core

out vec4 FragColor;

in vec2 fragTexCoord;

uniform sampler2D screenTexture;

uniform int postProcessingType;

void main() {
  if (postProcessingType == 0) {
    FragColor = texture(screenTexture, fragTexCoord);
  } else if (postProcessingType == 1) { /* Inversion */
    FragColor = vec4(1.0 - texture(screenTexture, fragTexCoord).rgb, 1.0);
  } else if (postProcessingType == 2) { /* Grayscale */
    vec4 color = texture(screenTexture, fragTexCoord);
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    FragColor = vec4(average, average, average, 1.0);
  } else { /* Kernel effects */
    float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
      vec2(-offset,  offset), // top-left
      vec2(   0.0f,  offset), // top-center
      vec2( offset,  offset), // top-right
      vec2(-offset,    0.0f), // center-left
      vec2(   0.0f,    0.0f), // center-center
      vec2( offset,    0.0f), // center-right
      vec2(-offset, -offset), // bottom-left
      vec2(   0.0f, -offset), // bottom-center
      vec2( offset, -offset)  // bottom-right
    );
    float kernel[9] = float[](
      -1, -1, -1,
      -1,  8, -1,
      -1, -1, -1
    );
    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; i++) {
      color += kernel[i] * texture(screenTexture, fragTexCoord.st + offsets[i]).rgb;
    }
    FragColor = vec4(color, 1.0);
  }
}
