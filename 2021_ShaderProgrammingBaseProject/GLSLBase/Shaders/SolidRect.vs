#version 450

//in vec3 a_Position; //사용자 입력값

in vec3 a_Position;		//float3 1개
in vec3 a_Position1;	//float3 1개, 총 2개 float3 2개

uniform float scale;	//상수버퍼 개념인가

void main()
{
	gl_Position = vec4(a_Position * scale, 1); // OpenGL 고유의 출력값
}
