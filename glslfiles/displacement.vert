varying vec4 diffuse,ambientGlobal,ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;

uniform float r;

void main(void)
{
	vec4 newVertexPos;
	vec4 dv;
	float df;
	
	dv = vec4(0.001*r,0.001*r,0.001*r,0.1);
	
	df = 0.30*dv.x + 0.59*dv.y + 0.11*dv.z;
	
	newVertexPos = vec4(gl_Normal * df * 100.0, 0.0) + gl_Vertex;
	
	vec4 ecPos;
	vec3 aux;
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	ecPos = gl_ModelViewMatrix * gl_Vertex;
	aux = vec3(gl_LightSource[0].position-ecPos);
	lightDir = normalize(aux);
	dist = length(aux);

	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	
	/* The ambient terms have been separated since one of them */
	/* suffers attenuation */
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
}