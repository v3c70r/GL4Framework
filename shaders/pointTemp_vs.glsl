#version 430

uniform float pointScale;

layout(location=0) in vec3 position;
layout(location=1) in float temperature;
layout(location=2) in vec3 normal;   //model space
layout(location=3) in float cellID;
layout(location=4) in vec3 pressure;


out VertexData{
    float temp;
    vec3 normal;
}VertexOut;

void main() 
{
    VertexOut.normal = normal;
    VertexOut.temp = temperature;
    gl_Position = vec4(position, 1.0);
}
