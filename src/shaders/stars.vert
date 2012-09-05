#version 330 core

// матрица преобразования координат, получаемая из программы
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

// входные вершинные атрибуты
in vec3 position;
in vec3 normal;
in vec2 texcoord;

// исходящие параметры, которые будут переданы в фрагментный шейдер
out vec2 fragTexcoord;


void main(void)
{
        gl_Position =  projectionMatrix * viewMatrix * vec4( position, 1.0 );

        fragTexcoord = texcoord;
}
