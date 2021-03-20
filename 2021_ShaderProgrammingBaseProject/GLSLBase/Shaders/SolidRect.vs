#version 450

in vec3 a_Position; //사용자 입력값

void main()
{
	gl_Position = vec4(a_Position, 1); // OpenGL 출력값
}
