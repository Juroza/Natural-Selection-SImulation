#version 330 core
uniform vec4 ourColour;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D Texture;

void main(){
    color = texture(Texture, TexCoord) * ourColour;
}
//color = texture(Texture,TexCoord);
