#version 400

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

in vec3 in_Position;  // Position coming in
in vec2 aTex;

out vec2 texCoord;

void main(void)
{
	//gl_Position = vec4(in_Position, 1.0);
	
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
	texCoord = aTex;
}