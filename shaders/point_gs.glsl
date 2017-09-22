#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 modelViewMat;
uniform mat4 normalMat;
uniform mat4 projMat;

out VertexData { vec2 mapping; }
VertexOut;

const float PI = 3.1415926;
const float R = 0.1;

void main()
{
    vec4 pos = modelViewMat * gl_in[0].gl_Position;

    vec3 up = vec3(0.0, 1.0, 0.0) * R;
    vec3 right = vec3(1.0, 0.0, 0.0) * R;

    VertexOut.mapping = vec2(-1, 1);
    gl_Position = projMat * (pos + vec4(up, 0.0) - vec4(right, 0.0));
    EmitVertex();

    VertexOut.mapping = vec2(-1, -1);
    gl_Position = projMat * (pos + vec4(-up, 0.0) - vec4(right, 0.0));
    EmitVertex();

    VertexOut.mapping = vec2(1, 1);
    gl_Position = projMat * (pos + vec4(up, 0.0) + vec4(right, 0.0));
    EmitVertex();

    VertexOut.mapping = vec2(1, -1);
    gl_Position = projMat * (pos - vec4(up, 0.0) + vec4(right, 0.0));
    EmitVertex();
}
