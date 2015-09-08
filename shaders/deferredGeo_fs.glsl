
#version 410

in VertexData{
    vec3 normal;
    vec3 pos;
    vec2 texCoord;
}VertexIn;


layout (location = 0) out vec4 PosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 TexCoordOut; 

uniform sampler2D tex;

void main()
{
    PosOut = vec4(/*normalize*/(VertexIn.pos), 1.0);
    DiffuseOut = texture(tex, VertexIn.texCoord);
    NormalOut = vec4(VertexIn.normal, 1.0);
    TexCoordOut = vec4(VertexIn.texCoord, 0.0, 1.0);

    //PosOut = vec3(1.0);
    //DiffuseOut = vec3(1.0);
    //NormalOut = vec3(1.0);
    //TexCoordOut = vec3(1.0);
}
