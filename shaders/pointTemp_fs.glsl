
#version 430
/*----Materials---*/
layout (std140) uniform Material
{
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    float shininess;
};

/*-----Directional Light----*/
layout (std140) uniform DirectionalLight
{
    vec4 l_dir;
    vec4 l_diffuse;
    vec4 l_ambient;
    vec4 l_specular;
};
in VertexData
{
    vec2 mapping;
    float temp;
    vec3 posCamera;
}VertexIn;

layout(std140) uniform CameraMats
{
    mat4 projMat;
    mat4 viewMat;
    mat4 invViewMat;
}camMats;

layout (binding = 0) uniform sampler2D tempTexture;
out vec4 color;

vec4 circle(vec4 color)
{
    float lensqr = dot(VertexIn.mapping, VertexIn.mapping);
    if(lensqr > 1.0)
        discard;
    return  color;
}

vec3 getNormal()
{
    vec3 N;
    N.xy = VertexIn.mapping;
    float lensqr = dot(VertexIn.mapping, VertexIn.mapping);
    if (lensqr > 1.0) discard;
    N.z = sqrt(1.0-lensqr);
    return normalize(N);
}

void main() {
    
    vec3 normalCamera = getNormal();
    vec4 pixelPos = vec4(VertexIn.posCamera + normalCamera*0.3, 0.5f);

    vec4 clipSpacePos = camMats.projMat * pixelPos;
    float depth = clipSpacePos.z / clipSpacePos.w;
    float diffuse = max (0.0, dot (normalCamera, vec3 (0.0, 0.0, 1.0)));

    int tempOffset = 200;
    vec4 temColor = texture2D(tempTexture, vec2(0.5, VertexIn.temp/tempOffset));
    color = temColor*diffuse;
    color.w = 1.0;
    gl_FragDepth = depth;
}
