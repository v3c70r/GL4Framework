#version 410
 
in vec2 UV;
out vec4 color;
 
uniform sampler2D colorTexture;
 
void main(){
        color = vec4( texture(colorTexture, UV));
        //gl_FragDepth = 1.00;
}

