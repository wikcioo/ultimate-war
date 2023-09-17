#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

out vec3 v_Position;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
    v_Position = a_Position;
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0f);
}

#type fragment
#version 330 core

in vec3 v_Position;
out vec4 OutputColor;

uniform bool u_Filled;
uniform float u_AspectRatio;
uniform float u_BorderThickness;
uniform vec4 u_Color;

void main()
{
    if (u_Filled)
    {
        OutputColor = u_Color;
    }
    else
    {
        vec2 thickness = vec2(0.5 - u_BorderThickness / u_AspectRatio, 0.5 - u_BorderThickness);
        if (v_Position.x > -thickness.x && v_Position.x < thickness.x &&
            v_Position.y > -thickness.y && v_Position.y < thickness.y)
        {
            discard;
        }
        else
        {
            OutputColor = u_Color;
        }
    }
}
