#version 400

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;

in  vec3 in_Position;  // Position coming in
in  vec2 in_TexCoord;  // texture coordinate coming in
in  vec3 in_Normal;    // vertex normal used for lighting

uniform vec4 LightPos;  // light position
uniform vec3 SpotlightDirection;

uniform vec4 LightPos2;

out vec2 ex_TexCoord;  // exiting texture coord
out vec3 ex_Normal;    // exiting normal transformed by the normal matrix
out vec3 ex_PositionEye; 
out vec3 ex_LightDir; 
out vec3 ex_LightDir2;
out vec3 ex_SpotlightDirection;


void main(void)
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
	
	ex_TexCoord = in_TexCoord;
		
	ex_Normal = normalize(NormalMatrix*in_Normal); 

	ex_PositionEye = vec3((ModelViewMatrix * vec4(in_Position, 1.0)));


	// Case for directional light when LightPos.w = 0.0
	if (LightPos.w == 0.0)
	{
		ex_LightDir = normalize(vec3(ViewMatrix * LightPos));
	} else {
		ex_LightDir = normalize(vec3(ViewMatrix * LightPos) - ex_PositionEye);
	}

	if (LightPos2.w == 0.0)
	{
		ex_LightDir2 = normalize(vec3(ViewMatrix * LightPos2));
	} else {
		ex_LightDir2 = normalize(vec3(ViewMatrix * LightPos2) - ex_PositionEye);
	}

	ex_SpotlightDirection = normalize(mat3(ViewMatrix) * SpotlightDirection);
}