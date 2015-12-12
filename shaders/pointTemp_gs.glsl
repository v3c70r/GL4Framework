#version 430

layout(points) in;
layout(triangle_strip, max_vertices=4) out;



layout(std140) uniform CameraMats
{
    mat4 projMat;
    mat4 viewMat;
    mat4 invViewMat;
}camMats;


in VertexData{
    float temp;
    vec3 normal;
}VertexIn[];

out VertexData{
    vec2 mapping;
    float temp;
    vec3 posCamera;     //postion in camera space
} VertexOut;

const float PI=3.1415926;
const float R = 0.35;



void main()
{
    /* Assume model matrix is identity*/
    vec4 pos =  camMats.viewMat * gl_in[0].gl_Position;

    vec3 up = vec3(0.0, 1.0, 0.0)*R;
    vec3 right = vec3(1.0, 0.0, 0.0)*R;

    VertexOut.mapping = vec2(-1,1);
    VertexOut.temp = VertexIn[0].temp;
    VertexOut.posCamera = pos.xyz/pos.w;
    gl_Position =  camMats.projMat * (pos+vec4(up,0.0)-vec4(right, 0.0));
    EmitVertex();

    VertexOut.mapping = vec2(-1,-1);
    VertexOut.temp = VertexIn[0].temp;
    VertexOut.posCamera = pos.xyz/pos.w;
    gl_Position =  camMats.projMat * (pos + vec4(-up, 0.0)-vec4(right, 0.0));
    EmitVertex();

    VertexOut.mapping = vec2(1,1);
    VertexOut.temp = VertexIn[0].temp;
    VertexOut.posCamera = pos.xyz/pos.w;
    gl_Position =  camMats.projMat * (pos + vec4(up, 0.0)+vec4(right, 0.0));
    EmitVertex();

    VertexOut.mapping = vec2(1,-1);
    VertexOut.temp = VertexIn[0].temp;
    VertexOut.posCamera = pos.xyz/pos.w;
    gl_Position =  camMats.projMat * (pos - vec4(up, 0.0)+vec4(right, 0.0));
    EmitVertex();
}
