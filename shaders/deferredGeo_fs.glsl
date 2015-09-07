
#version 410

in VertexData{
    vec3 normal;
    vec3 pos;
    vec2 texCoord;
}VertexIn;


layout (location = 0) out vec3 PosOut;
layout (location = 1) out vec3 DiffuseOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec3 TexCoordOut; 

uniform sampler2D tex;

void main()
{
    //PosOut = VertexIn.toEye;
    //DiffuseOut = texture(tex, VertexIn.texCoord).xyz;
    //NormalOut = VertexIn.normal;
    //TexCoordOut = vec3(VertexIn.texCoord, 0.0);

    PosOut = vec3(1.0);
    DiffuseOut = vec3(1.0);
    NormalOut = vec3(1.0);
    TexCoordOut = vec3(1.0);
}
