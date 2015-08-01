#version 410
 
in vec2 UV;
out vec4 color;
 
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
 
void main(){

        color = vec4( texture(colorTexture, UV));
}

