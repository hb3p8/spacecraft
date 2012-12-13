#version 330 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float radius;
uniform sampler2D gradientTexture;

in vec3 position;

void main()
{
	gl_Position = viewMatrix * vec4(position, 1.0);
}
