
#version 410
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
}VertexIn;
 out vec4 color;

vec4 circle(vec4 color)
{
    float lensqr = dot(VertexIn.mapping, VertexIn.mapping);
    if(lensqr > 1.0)
        discard;
    return  color;
}
void main() {
    
    //clamp(VertexIn.val/10, 0.0, 1.0);
    color = circle((vec4(1.0, 0.0, 0.0, 1.0)));
}
