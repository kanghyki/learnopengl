#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D colorTex;
uniform sampler2D bloomBlur;
uniform float gamma;
uniform float exposure;
uniform bool bloom_on;
uniform bool hdr_on;

void main() {
  vec3 pixel = texture(colorTex, texCoord).xyz;
  if (bloom_on) {
    vec3 bloomPixel = texture(bloomBlur, texCoord).xyz;
    pixel += bloomPixel;
  }

  vec3 result;
  if (hdr_on) {
    result = vec3(1.0) - exp(-pixel * exposure);
  }
  else {
    result = pixel;
  }

  result = pow(result.rgb, vec3(gamma));
  fragColor = vec4(result, 1.0);
}