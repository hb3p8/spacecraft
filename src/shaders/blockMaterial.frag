#version 330 core

uniform sampler2D colorTexture;

in vec2 fragTexcoord;
in vec4 pos;
in vec3 norm;
in vec3 fragColor;

out vec4 color;

void main(void)
{
    color = texture(colorTexture, fragTexcoord) * vec4( fragColor, 1.0 );

    color.w = 1.0;
}
