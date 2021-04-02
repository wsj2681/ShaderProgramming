#version 450

in vec3 a_Position;		//float3 1개
in vec3 a_Valocity;
in float a_EmitTime;
in float a_LifeTime;
in float a_Period;
in float a_Amp;

uniform float u_time;
//const vec3 c_Gravity = vec3(0, -2.8, 0);
const vec3 c_Gravity = vec3(0, 0, 0);
void main()
{
	float newTime = u_time - a_EmitTime;
	

	vec3 newPosition = a_Position;
	

	if(newTime < 0.0)
	{
		newPosition = vec3(10000, 10000, 10000);
	}
	else
	{
		newTime = mod(newTime, a_LifeTime);
		newPosition = newPosition + vec3(newTime, 0, 0);
		newPosition.y = newPosition.y + a_Amp * sin(newTime * 3.14 * 2 * a_Period);

		//float t = newTime;
		//float tt = newTime * newTime;

		//newPosition = newPosition + (a_Valocity + 0.5 * c_Gravity * tt);
	}

	gl_Position = vec4(newPosition, 1); // OpenGL 고유의 출력값
}
