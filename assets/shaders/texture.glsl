#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_ProjectionView;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_ProjectionView * vec4(a_Position, 1.0f);
}

#type fragment
#version 330 core

in vec2 v_TexCoord;
out vec4 OutputColor;

uniform sampler2D u_Texture;

void main()
{
    OutputColor = texture(u_Texture, v_TexCoord);
}
