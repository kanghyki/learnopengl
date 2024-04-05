#version 330 core
out vec4 fragColor;
in vec3 texCoord;

uniform samplerCube cube;

void main() {
    fragColor = texture(cube, texCoord);
}