#version 450

in vec3 a_Position;	
in vec3 a_Valocity;
in float a_EmitTime;
in float a_LifeTime;
in float a_Period;
in float a_Amp;
in float a_RandValue;
in vec4 a_Color;

uniform float u_time;
uniform vec3 u_ExForce;

const vec3 c_Gravity = vec3(0, -1.8, 0);
const mat3 c_NV = mat3(0, -1, 0, 1, 0, 0, 0, 0, 0);

out vec4 outColor;

void main()
{
	float newTime = u_time - a_EmitTime;
	

	//vec3 newPosition = a_Position;
	vec3 newPosition;
	newPosition.x = a_Position.x + cos(a_RandValue * 2 * 3.14);
	newPosition.y = a_Position.y + sin(a_RandValue * 2 * 3.14);
	newPosition.z = 0;

	// Heart Shape
	//newPosition.x = a_Position.x + (16 * pow(sin(a_RandValue* 2 * 3.14), 3)) * 0.03;
	//newPosition.y = a_Position.y + (13 * cos(a_RandValue* 2 * 3.14) - 5 * cos(2 * a_RandValue* 2 * 3.14) 
	//- 2 * cos(3 * a_RandValue* 2 * 3.14) - cos(4* a_RandValue* 2 * 3.14)) * 0.03;
	
	// Star Shape
	//newPosition.x = a_Position.x + (5 * cos(2 * a_RandValue* 2 * 3.14) + 2 * cos(3 * a_RandValue* 2 * 3.14)) * 0.1;
	//newPosition.y = a_Position.y + (2 * sin(3 * a_RandValue* 2 * 3.14) - 5 * sin(2 * a_RandValue* 2 * 3.14)) * 0.1;

	// Butterfly Shape
	newPosition.x = a_Position.x + 0.3 * (cos(a_RandValue* 2 * 3.14) * (exp(cos(a_RandValue* 2 * 3.14)) - 2 * cos(4 * a_RandValue* 2 * 3.14) - pow(sin((a_RandValue * 2 * 3.14) / 12), 5)));
	newPosition.y = a_Position.y + 0.3 * (sin(a_RandValue* 2 * 3.14) * (exp(cos(a_RandValue* 2 * 3.14)) - 2 * cos(4 * a_RandValue* 2 * 3.14) - pow(sin((a_RandValue * 2 * 3.14) / 12), 5)));
	newPosition = newPosition * c_NV;

	vec4 Color = vec4(0);


	if(newTime < 0.0)
	{
		newPosition = vec3(10000, 10000, 10000);
	}
	else
	{
		//newTime = mod(newTime, a_LifeTime);
		//newPosition = newPosition + vec3(newTime, 0, 0);
		//newPosition.y = newPosition.y + (a_Amp * newTime) * sin(newTime * 3.14 * 2 * a_Period);

		newTime = mod(newTime, a_LifeTime);

		// newTime^2
		float powTime = newTime * newTime;
		
		vec3 newAcc = c_Gravity + u_ExForce;
		
		// Current speed over time
		vec3 currVel = a_Valocity + newTime * newAcc;

		// Normalize
		vec3 normalV = normalize(currVel * c_NV);

		// 
		newPosition = newPosition + (newTime * a_Valocity) + 0.5 * newAcc * powTime;
		newPosition = newPosition + normalV * a_Amp * sin(newTime * 3.14 * 2 * a_Period);
		//float intensity = 1 - newTime/a_LifeTime;
		float intensity = abs(sin(newTime* 2 * 3.14));

		Color = a_Color * intensity;
	}

	gl_Position = vec4(newPosition, 1); // OpenGL 고유의 출력값
	outColor = Color;
}
