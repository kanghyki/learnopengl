#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 ourST;

uniform int type;
uniform float alpha;

uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
    if (type == 0) {
        FragColor = vec4(ourColor, alpha);
    } else if (type == 1) {
        FragColor = texture(tex, ourST);
    } else if (type == 2) {
        FragColor = texture(tex2, ourST);
    } else {
        FragColor = (texture(tex, ourST) * 0.5 + texture(tex2, ourST) * 0.5) * vec4(ourColor, alpha);
    }
}