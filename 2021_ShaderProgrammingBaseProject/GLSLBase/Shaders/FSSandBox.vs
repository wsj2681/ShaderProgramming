#version 450

in vec3 a_Position;	

void main()
{
	gl_Position = vec4(a_Position, 1); // OpenGL 고유의 출력값
}
