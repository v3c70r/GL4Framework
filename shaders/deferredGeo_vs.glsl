#version 410
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 TexCoord;


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


out VertexData{
    vec3 normal;
    vec3 pos;
    vec2 texCoord;
}VertexOut;

void main()
{
    mat4 normalMat = transpose(modelMats.invModelMat * camMats.invViewMat);
    gl_Position = camMats.projMat * camMats.viewMat * modelMats.modelMat * vec4(position, 1.0);
    VertexOut.normal = (normalMat * vec4(normal,0.0)).xyz;
    VertexOut.pos =  (camMats.viewMat * modelMats.modelMat * vec4(position, 1.0)).xyz;
    VertexOut.texCoord = TexCoord;
}
