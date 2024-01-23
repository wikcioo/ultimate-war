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
uniform vec3 u_Color;

float sdHexagon( in vec2 p, in float r )
{
    const vec3 k = vec3(-0.866025404,0.5,0.577350269);
    p = abs(p);
    p -= 2.0*min(dot(k.xy,p),0.0)*k.xy;
    p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
    return length(p)*sign(p.y);
}

void main()
{
    vec2 uv = ((gl_FragCoord.xy - u_BottomLeftPx) * 2.0f - u_SizePx * 0.5f) / u_SizePx.y;
    float d = sdHexagon(uv, 1.0f);
    float alpha = max(0.0f, 0.4f - abs(d)) + 0.4f;
    OutputColor = vec4(u_Color, alpha);
}
