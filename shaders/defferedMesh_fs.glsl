#version 410
in VertexData{
    vec3 normal;
    vec3 toEye;
    vec2 texCoord;
}VertexIn;

uniform sampler2D tex;

layout (location=0) out vec3 pos;
layout (location=1) out vec3 diffuse;
layout (location=2) out vec3 normal;
layout (location=4) out vec2 texCoord;

void main()
{
    pos = VertexIn.toEye;
    diffuse = texture(tex, VertexIn.texCoord).xyz;
    normal = VertexIn.normal;
    texCoord = VertexIn.texCoord;
}
