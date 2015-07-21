#version 430
//Die Apple
//Die OpenGL 4.1


layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 TexCoord;

uniform mat4 modelViewMat;
uniform mat4 normalMat;
uniform mat4 projMat;

out VertexData{
    vec3 normal;
    vec3 toEye;
    vec4 shadowCoord;
    vec2 texCoord;
    mat4 viewMat;
    //debug
    vec4 weights;
}VertexOut;

#define MAX_NUM_BONES 20
layout (std140) uniform BoneTrans
{
    mat4 transMat[MAX_NUM_BONES];
};

#define MAX_NUM_VERTS 1000
layout (std140) buffer BoneWeights
{
    float weights[MAX_NUM_BONES * MAX_NUM_VERTS];
};


void main()
{
    VertexOut.normal = (normalMat * vec4(normal,0.0)).xyz;
    VertexOut.toEye = - (modelViewMat * vec4(position, 1.0)).xyz;
    VertexOut.texCoord = TexCoord;
    VertexOut.viewMat = modelViewMat;

    //debug, output first for weights
    VertexOut.weights.x = weights[gl_VertexID * MAX_NUM_BONES + 0];
    VertexOut.weights.y = weights[gl_VertexID * MAX_NUM_BONES + 1];
    VertexOut.weights.z = weights[gl_VertexID * MAX_NUM_BONES + 2];
    VertexOut.weights.w = 1.0;

    gl_Position = projMat * modelViewMat*vec4(position, 1.0);
}
