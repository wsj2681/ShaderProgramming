#version 450

//in vec3 a_Position; //����� �Է°�

in vec3 a_Position;		//float3 1��
//in vec3 a_Position1;	//float3 1��, �� 2�� float3 2��

//uniform float scale;	//������� �����ΰ�
//uniform vec3 position;

void main()
{
	//vec3 temp = a_Position;
	//temp = temp + position;
	gl_Position = vec4(a_Position, 1); // OpenGL ������ ��°�
}
