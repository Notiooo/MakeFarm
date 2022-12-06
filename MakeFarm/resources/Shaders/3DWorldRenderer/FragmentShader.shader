#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;
in float v_DirectionalLightning;

uniform sampler2D u_Texture;


void main()
{
	FragColor = texture(u_Texture, v_TexCoord);
	FragColor.r *= v_DirectionalLightning;
	FragColor.g *= v_DirectionalLightning;
	FragColor.b *= v_DirectionalLightning;
};