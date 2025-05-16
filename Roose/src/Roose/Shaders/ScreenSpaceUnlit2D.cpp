#include "rspch.h"

std::string ScreenSpaceUnlit2DShaderSource = R"(
#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

uniform mat4 u_OrthoProjection;
uniform mat4 u_Transform;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_OrthoProjection * u_Transform * vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
    o_Color = texture(u_Texture, v_TexCoord);
}
)";
