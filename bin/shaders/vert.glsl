#version 330 core

// матрица преобразования координат, получаемая из программы
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

// входные вершинные атрибуты
in vec3 position;
in vec3 normal;
in vec2 texcoord;

// исходящие параметры, которые будут переданы в фрагментный шейдер
out vec2 fragTexcoord;
out vec4 pos;
out vec3 norm;

void main(void)
{

        pos = modelMatrix * vec4(position, 1.0);
        gl_Position =  projectionMatrix * viewMatrix * pos;
        
        norm = normal;

        // передадим текстурные координаты в фрагментный шейдер
        fragTexcoord = texcoord;
}