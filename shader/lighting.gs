#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in VS_OUT {
  vec3 position;
  vec3 normal;
  vec2 texCoord;
  vec4 lightPosition; // directional shadow
} gs_in[];

out VS_OUT {
  vec3 position;
  vec3 normal;
  vec2 texCoord;
  vec4 lightPosition; // directional shadow
} gs_out;

void main() {
    gl_Position = gl_in[0].gl_Position;
    gs_out.position = gs_in[0].position;
    gs_out.normal = gs_in[0].normal;
    gs_out.texCoord = gs_in[0].texCoord;
    gs_out.lightPosition = gs_in[0].lightPosition;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position; 
    gs_out.position = gs_in[1].position;
    gs_out.normal = gs_in[1].normal;
    gs_out.texCoord = gs_in[1].texCoord;
    gs_out.lightPosition = gs_in[1].lightPosition;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    gs_out.position = gs_in[2].position;
    gs_out.normal = gs_in[2].normal;
    gs_out.texCoord = gs_in[2].texCoord;
    gs_out.lightPosition = gs_in[2].lightPosition;
    EmitVertex();

    EndPrimitive();
}  