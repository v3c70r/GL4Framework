
#version 410

in VertexData{
    vec3 normal;
    vec3 pos;
    vec2 texCoord;
}VertexIn;

layout( std140) uniform Material
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    float shininess;
    float opacity;
    vec2 padding;
}material;

#define MAX_DIRLIGHTS 5
struct DirLight
{
    vec4 dir;
};

layout(std140) uniform DirLights
{
    DirLight lights[MAX_DIRLIGHTS];
    int numDirLights;
    vec3 padding;
}dirLights;

layout (location = 0) out vec4 PosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 TexCoordOut; 


uniform sampler2D tex;

vec4 blinnPhongDir(vec3 dir, vec3 toEye, vec3 normal, vec4 matDif, vec4 matAmb, vec4 matSpec, float matShininess)
{
    vec3 lightDir = normalize(dir);
    vec3 N = normalize(normal);
    //diffuse
    float diff = max(dot(lightDir, N), 0.0);
    vec3 diffuse = diff * matDif.xyz;
    //specular
    float spec = 0.0;
    vec3 viewDir = normalize(toEye);
    vec3 reflectDir = reflect(-lightDir, N);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(N, halfwayDir), 0.0), 32.0);
    vec3 specular = matSpec.xyz * spec; 
    return vec4(matAmb.xyz + diffuse + specular, 1.0f);

}

void main()
{
    vec4 color = vec4(0.0);
    for (int i=0; i<dirLights.numDirLights; i++)
    {
        color += blinnPhongDir(
                dirLights.lights[i].dir.xyz,
                VertexIn.pos,
                VertexIn.normal,
                texture(tex, VertexIn.texCoord),
                material.ambient,
                material.specular,
                material.shininess
                );
    }
    color.w = 1.0;

    PosOut = vec4(/*normalize*/(VertexIn.pos), 1.0);
    DiffuseOut = texture(tex, VertexIn.texCoord);
    NormalOut = vec4(VertexIn.normal, 1.0);
    //TexCoordOut = vec4(VertexIn.texCoord, 0.0, 1.0);
    //TexCoordOut = vec4(vec3(gl_FragCoord.z), 1.0);
    TexCoordOut = color;

    //PosOut = vec3(1.0);
    //DiffuseOut = vec3(1.0);
    //NormalOut = vec3(1.0);
    //TexCoordOut = vec3(1.0);
}
