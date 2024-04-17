#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices=6) out;

uniform mat4 transform;
uniform float length;
in vec3 normal[];

void gen(int i) {
  gl_Position = transform * gl_in[i].gl_Position;
  EmitVertex();

  gl_Position = transform * (gl_in[i].gl_Position + normalize(vec4(normal[i], 0.0)) * length);
  EmitVertex();
  EndPrimitive();
}

void main() {
  gen(0);
  gen(1);
  gen(2);
}