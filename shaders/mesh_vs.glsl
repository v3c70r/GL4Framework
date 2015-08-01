#version 410
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 TexCoord;

uniform mat4 modelViewMat;
uniform mat4 normalMat;
uniform mat4 projMat;
//uniform mat4 shadowMVP;

out VertexData{
    vec3 normal;
    vec3 toEye;
    vec4 shadowCoord;
    vec2 texCoord;
    mat4 viewMat;
}VertexOut;

void main()
{
    VertexOut.normal = (normalMat * vec4(normal,0.0)).xyz;
    VertexOut.toEye = - (modelViewMat * vec4(position, 1.0)).xyz;
    VertexOut.texCoord = TexCoord;
    VertexOut.viewMat = modelViewMat;
    gl_Position = projMat * modelViewMat*vec4(position, 1.0);
    //VertexOut.shadowCoord = shadowMVP*vec4(position, 1.0);
}
