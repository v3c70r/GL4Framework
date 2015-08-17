#version 410
//Die Apple
//Die OpenGL 4.1

#define NO_BONE 999

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 TexCoord;
layout(location=3) in vec4 weights;
layout(location=4) in ivec4 IDs;

layout(std140) uniform CameraMats
{
    mat4 projMat;
    mat4 viewMat;
    mat4 invViewMat;
}camMats;

layout(std140) uniform ModelMats
{
    mat4 modelMat;
    mat4 invModelMat;
}modelMats;

#define NUM_BONES 20
layout( std140) uniform BoneTrans
{
    mat4 m[NUM_BONES];
}boneTrans;

out VertexData{
    vec3 normal;
    vec3 toEye;
    vec4 shadowCoord;
    vec2 texCoord;
}VertexOut;



void main()
{
    mat4 modelViewMat = camMats.viewMat * modelMats.modelMat;
    mat4 normalMat = transpose(modelMats.invModelMat * camMats.invViewMat);
    VertexOut.toEye = - (modelViewMat * vec4(position, 1.0)).xyz;
    VertexOut.texCoord = TexCoord;

    mat4 trans = mat4(0.0);
    for (int i=0; i<4; i++)
    {
        if (IDs[i] == NO_BONE) continue;
        else trans += boneTrans.m[IDs[i]] * weights[i];
    }
    
    VertexOut.normal = (normalMat *trans* vec4(normal,0.0)).xyz;
    gl_Position = camMats.projMat * modelViewMat*trans*vec4(position, 1.0);
}
