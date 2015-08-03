#version 410

in VertexData{
    vec3 normal;
    vec3 toEye;
    vec4 shadowCoord;
    vec2 texCoord;
    //debug
    vec4 weights;
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

//uniform sampler2D shadowMap[MAX_NUM_DIRLIGHTS+MAX_NUM_POINTLIGHTS];
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


uniform sampler2D tex;
out vec4 color;


void main()
{
    color = vec4(0.0);
    for (int i=0; i<dirLights.numDirLights; i++)
    {
        color += blinnPhongDir(
                dirLights.lights[i].dir.xyz,
                VertexIn.toEye,
                VertexIn.normal,
                texture(tex, VertexIn.texCoord),
                material.ambient,
                material.specular,
                material.shininess
                );
    }
    //color = material.specular;
       //color = vec4(dirLights.numDirLights / 5.0);


    //color = VertexIn.weights;
    color.w = 1.0;
}

