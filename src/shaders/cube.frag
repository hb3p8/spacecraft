#version 330 core

// текстура
uniform sampler2D colorTexture;

uniform vec4 baseColor;
uniform vec3 point;
uniform float ambient;

// параметры, полученные из вершинного шейдера
in vec2 fragTexcoord;
in vec4 pos;
in vec3 norm;
in vec3 fragColor;

// результирующий цвет пикселя на экране
out vec4 color;

vec3 light = vec3( -10.0, 15.0, 0.0 ); 

void main(void)
{
	vec3 position = pos.xyz / pos.w;
	vec3 dpos = position - point;
	float dist = sqrt( dot( dpos, dpos ) );
    // получим цвет пикселя из текстуры по текстурным координатам
    color = ( /*vec4( 1.0 )*/ texture(colorTexture, fragTexcoord) + baseColor ) *
        vec4( fragColor, 1.0 ) *
        ( 0.3  + 0.7 * max( dot( norm, normalize( light - position ) ), 0.1 ) ) *
        ( dist < 0.08 ? vec4( 1.0, 0.0, 0.0, 1.0 ) : vec4( 1.0, 1.0, 1.0, 1.0 ) ) ;
    color.w = 1.0;
}
