#version 430
//Die Apple
//Die OpenGL 4.1

#define NO_BONE 999
#define NUM_BONES 4

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 TexCoord;
layout(location=3) in vec4 weights;
layout(location=4) in ivec4 IDs;

uniform mat4 modelViewMat;
uniform mat4 normalMat;
uniform mat4 projMat;
uniform mat4 bonesTrans[NUM_BONES];

out VertexData{
    vec3 normal;
    vec3 toEye;
    vec4 shadowCoord;
    vec2 texCoord;
    mat4 viewMat;
    //debug
    vec4 weights;
}VertexOut;



void main()
{
    VertexOut.normal = (normalMat * vec4(normal,0.0)).xyz;
    VertexOut.toEye = - (modelViewMat * vec4(position, 1.0)).xyz;
    VertexOut.texCoord = TexCoord;
    VertexOut.viewMat = modelViewMat;

    VertexOut.weights.x = float(IDs.x);
    VertexOut.weights.y = float(IDs.y);
    VertexOut.weights.z = float(IDs.z);
    VertexOut.weights.w = 1.0;
    gl_Position = projMat * modelViewMat*vec4(position, 1.0);
}
