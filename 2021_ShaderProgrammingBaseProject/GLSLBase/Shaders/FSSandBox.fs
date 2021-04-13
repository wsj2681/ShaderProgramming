#version 450

layout(location=0) out vec4 FragColor;
varying vec4 v_Color;

const vec3 circle = vec3(0.5, 0.5, 0.0);
uniform vec2 iResolution;
uniform float time;

float text(vec2 pos) {
  return pow(max(0.f, sin(8.f * 3.1416f * pos.x) * sin(8.f * 3.1416f * pos.y)), 0.8f)+.3*(1.f - cos(0.025f * 6.28f * time));
}

void main()
{
	
	vec2 uv = 2.0*(gl_FragCoord.xy / iResolution.y - vec2(0.9,0.5));	
    float r = length(uv), a = atan(uv.y,uv.x);
    
    float v = pow(max(0.f, sin(3.f * 6.283f * log(r) - 0.f * 2.f * time)), 0.8f);
    
    // pattern
    vec2 grid = vec2((a + 0.f * time)* 4.f / 3.1416f, log(r) + a / 3.1416f);
    float t = text(grid+vec2(0.,0.15 * 3.f * sin(time) / 2.f * sin(2.f * grid.x)));
    v *= t;
    
	FragColor = vec4(v);

	//vec2 position = -1.0 + 2.0 * gl_FragCoord.xy / iResolution.xy;
	//float radius = sqrt(position.x * position.x + position.y * position.y);
	//float modulo = mod(radius, 0.1);
	//bool toggle = false;
	//
	//if(modulo > 0.05)
	//{
	//	toggle = true;
	//}
	//
	//if(toggle)
	//{
	//	FragColor = vec4(1.f, 1.f, 1.f, 1.f);
	//}
	//else
	//{
	//	FragColor = vec4(0.f, 0.f, 0.f, 1.f);
	//}
}
