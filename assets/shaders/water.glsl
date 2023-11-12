#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0f);
}

#type fragment
#version 330 core

out vec4 OutputColor;

uniform float u_Time;
uniform vec2 u_BottomLeftPx;
uniform vec2 u_SizePx;

void main()
{
    vec2 uv = ((gl_FragCoord.xy - u_BottomLeftPx) * 2.0f - u_SizePx * 0.5f) / u_SizePx.y;

    vec4 texture_color = vec4(0.192156862745098, 0.6627450980392157, 0.9333333333333333, 1.0);

    vec4 k = vec4(u_Time)*0.8;
	k.xy = uv * 2.0;
    float val1 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.5));
    float val2 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.2));
    float val3 = length(0.5-fract(k.xyw*=mat3(vec3(-2.0,-1.0,0.0), vec3(3.0,-1.0,1.0), vec3(1.0,-1.0,-1.0))*0.5));
    vec4 color = vec4(pow(min(min(val1, val2), val3), 7.0) * 3.0) + texture_color;

    OutputColor = color;
}
