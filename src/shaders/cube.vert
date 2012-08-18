#version 330 core

// матрица преобразования координат, получаемая из программы
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

// входные вершинные атрибуты
in vec3 position;
in vec3 normal;
in vec2 texcoord;
in vec3 color;

// исходящие параметры, которые будут переданы в фрагментный шейдер
out vec2 fragTexcoord;
out vec4 pos;
out vec3 norm;
out vec3 fragColor;

void main(void)
{

        pos = modelMatrix * vec4(position, 1.0);
        gl_Position =  projectionMatrix * viewMatrix * pos;
        
        norm = normal;
        fragColor = color;

        // передадим текстурные координаты в фрагментный шейдер
        fragTexcoord = texcoord;
}
