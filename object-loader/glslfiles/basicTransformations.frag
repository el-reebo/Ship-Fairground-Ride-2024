#version 400

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightDir;  //light direction arriving from the vertex
in vec3 ex_LightDir2;

in vec3 ex_SpotlightDirection;

in vec3 ex_PositionEye;

//Light source 1
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 LightPos;

//Light source 2
uniform vec4 light_ambient2;
uniform vec4 light_diffuse2;
uniform vec4 light_specular2;
uniform vec4 LightPos2;

struct SpotLight
{
	float cutOff;
	float outerCutOff;
};

uniform SpotLight spotLight;


uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;

uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;

uniform sampler2D DiffuseMap;

void main(void)
{
	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture values

	//out_Color = vec4(ex_Normal,1.0); //Show normals

	//out_Color = vec4(ex_TexCoord,0.0,1.0); //show texture coords

	//Calculate lighting
	vec3 n, L, L2;
	vec4 totalColor, color, color2;
	float NdotL, NdotL2, dist, dist2, att, att2;
	
	n = normalize(ex_Normal); // ensures normal and light direction is of length 1
	L = normalize(ex_LightDir); //for point light
	L2 = normalize(ex_LightDir2);
	//L = normalize(vec3(LightPos)); // for directional light

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	vec3 r2 = normalize(-reflect(L2, n));
	
	float RdotV = max(0.0, dot(r, v));
	float RdotV2 = max(0.0, dot(r2, v));

	NdotL = max(dot(n, L),0.0);
	NdotL2 = max(dot(n, L2),0.0);

//Including ambient lighting
	color = light_ambient * material_ambient; 

//Including light source attenuation
	dist = length(vec3(LightPos) - ex_PositionEye);

	att = min(1.0f / (constantAttenuation + 
						linearAttenuation * dist +
						quadraticAttenuation * dist * dist), 1);
	
	dist2 = length(vec3(LightPos2) - ex_PositionEye);

	att2 = min(1.0f / (constantAttenuation +
						linearAttenuation * dist2 +
						quadraticAttenuation * dist2 * dist2), 1);

	
//Including diffuse lighting
	if(NdotL > 0.0) 
	{
		//including specular lighting
		color += att * (light_diffuse * material_diffuse);
		color += att * material_specular * light_specular * pow(RdotV, material_shininess);
	}

	if(NdotL2 > 0.0)
	{
		color2 += att2 * (light_diffuse2 * material_diffuse);
		color2 += att2 * material_specular * light_specular2 * pow(RdotV2, material_shininess);
	}
	

//spotlight calculations
	float theta = dot(ex_LightDir, normalize(-ex_SpotlightDirection)); 
    float cutOffDiff = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / cutOffDiff, 0.0, 1.0);
	//color = intensity * color;

	totalColor = color + color2;

	//out_Color = color;  //show just lighting

	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture only
    out_Color = totalColor * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}