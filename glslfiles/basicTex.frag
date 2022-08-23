// Fragment Shader
#version 400

out vec4 out_Color; //colour for the pixel

in vec2 texCoord;

uniform sampler2D tex0;

void main(void)
{
	//out_Color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	out_Color = texture(tex0, texCoord);
}