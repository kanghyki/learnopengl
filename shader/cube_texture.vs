#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 texCoord;

uniform mat4 model;

layout (std140) uniform Transform {
  mat4 view;
  mat4 projection;
};

void main() {
    texCoord = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}