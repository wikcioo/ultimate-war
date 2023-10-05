#type vertex
#version 330 core

layout(location = 0) in vec2 a_Data;

out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ProjectionView;

void main()
{
    v_TexCoord = vec2(a_Data.x, 1.0 - a_Data.y);
    gl_Position = u_ProjectionView * u_Model * vec4(a_Data.xy, 0.0f, 1.0f);
}

#type fragment
#version 330 core

in vec2 v_TexCoord;
out vec4 OutputColor;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, v_TexCoord).r);
    OutputColor = vec4(u_Color, 1.0) * sampled;
}
