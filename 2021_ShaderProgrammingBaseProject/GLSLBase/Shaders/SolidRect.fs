#version 450

layout(location=0) out vec4 FragColor;

//uniform vec4 color;

void main()
{
	//Pink Color
	FragColor = vec4(1, 0.078, 0.576, 1);
	
	//Yellow Color
	FragColor = vec4(0, 1, 0, 1);

	//FragColor = color;
}
