#version 450

layout(location=0) out vec4 FragColor;
in vec4 outColor;
//uniform vec4 color;

void main()
{
	//Pink Color
	//FragColor = vec4(1, 0.078, 0.576, 1);
	
	//Yellow Color
	FragColor = vec4(outColor);

	//FragColor = color;
}
