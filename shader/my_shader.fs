#version 330 core
out vec4 FragColor;

in vec2 ourST;

uniform int type;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    if (type == 0) {
        FragColor = texture(tex, ourST);
    } else if (type == 1) {
        FragColor = texture(tex2, ourST);
    }
}
