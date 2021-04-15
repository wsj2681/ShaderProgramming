#version 450

layout(location=0) out vec4 FragColor;
varying vec4 v_Color;

uniform vec2 iResolution;
uniform float time;

uniform vec3 u_Point;
uniform vec3 u_Points[10];

const float PI = 3.141592;
const vec3 circle = vec3(0.5, 0.5, 0.0);

float text(vec2 pos) 
{
	return pow(max(0.f, sin(8.f * 3.1416f * pos.x) * sin(8.f * 3.1416f * pos.y)), 0.8f)+.3*(1.f - cos(0.025f * 6.28f * time));
}

vec4 pattern()
{
	vec2 uv = 2.0*(gl_FragCoord.xy / iResolution.y - vec2(0.9,0.5));	

    float r = length(uv);
	float a = atan(uv.y,uv.x);
    
    float v = pow(max(0.f, sin(3.f * 6.283f * log(r) - 0.f * 2.f * time)), 0.8f);

    vec2 grid = vec2((a + 0.f * time)* 4.f / 3.1416f, log(r) + a / 3.1416f);

    float t = text(grid+vec2(0.,0.15 * 3.f * sin(time) / 2.f * sin(2.f * grid.x)));

    v *= t;

	return vec4(v);
}

vec4 CenteredCircle1()
{
	vec2 position = -1.0 + 2.0 * gl_FragCoord.xy / iResolution.xy;
	float radius = sqrt(position.x * position.x + position.y * position.y);
	float modulo = mod(radius, 0.1);
	bool toggle = false;
	
	if(modulo > 0.05)
	{
		toggle = true;
	}
	
	if(toggle)
	{
		return vec4(1.f, 1.f, 1.f, 1.f);
	}
	else
	{
		return vec4(0.f, 0.f, 0.f, 1.f);
	}	
}

vec4 CenteredCircle2()
{
	float d = length(v_Color.rgb - circle);
	float count = 3;
	float rad = d * 2.0 * 2.0 * PI * count;
	float greyScale = sin(rad);
	float width = 200;
	greyScale = pow(greyScale, width);
	return vec4(greyScale);
}

vec4 CenteredCircle3()
{
	vec4 returnColor;
	for(int i = 0; i < 10; ++i)
	{
		float d = length(v_Color.rg - u_Points[i].xy);
		if(d < u_Points[i].z)
		{
			float count = 1.5;
			float rad = d * 2.0 * 2.0 * PI * count;
			float greyScale = sin(rad);
			float width = 1;
			greyScale = pow(greyScale, width);
			returnColor =  vec4(greyScale);
		}
	}

	return returnColor;
}

vec4 IndicatePoint()
{
	vec4 returnColor = vec4(0);
	float d = length(v_Color.rg - u_Point.xy);
	if(d < u_Point.z)
	{
		returnColor = vec4(1);
	}

	return returnColor;
}

vec4 IndicatePoints()
{
	vec4 returnColor = vec4(0);

	for(int i = 0; i < 10; ++i)
	{
		float d = length(v_Color.rg - u_Points[i].xy);
		if(d < u_Points[i].z)
		{
			returnColor = vec4(1);
		}
	}

	return returnColor;
}

void main()
{
	FragColor = CenteredCircle3();
}
