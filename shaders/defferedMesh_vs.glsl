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
    vec2 texCoord;
}VertexOut;

void main()
{
    VertexOut.normal = (normalMat * vec4(normal,0.0)).xyz;
    VertexOut.toEye = - (modelViewMat * vec4(position, 1.0)).xyz;
    VertexOut.texCoord = TexCoord;
    gl_Position = projMat * modelViewMat*vec4(position, 1.0);
}
