	#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 lightTransform;

layout (std140) uniform Transform {
  mat4 view;
  mat4 projection;
};

out VS_OUT {
  vec3 position;
  vec3 normal;
  vec2 texCoord;
  vec4 positionLight;
} vs_out;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.position = (model * vec4(aPos, 1.0)).xyz;
    vs_out.normal = (transpose(inverse(model)) * vec4(aNormal, 0.0)).xyz;
    vs_out.texCoord = aTexCoord;
    vs_out.positionLight = lightTransform * vec4(vs_out.position, 1.0);
}