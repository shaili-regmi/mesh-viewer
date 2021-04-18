#version 400
//per-pixel

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNor;

struct LightInfo
{
	vec3 position;
	vec3 color; 
};

struct Material
{
	vec3 Kd;
	vec3 Ka;
	vec3 Ks;
	float shininess;
};

uniform LightInfo uLight;
uniform Material uMaterial;
uniform mat4 uMV;
uniform mat4 uMVP;
uniform mat3 uNMV;

out vec3 color;

vec3 phongModel(in vec3 ePos, in vec3 eNormal)
{
	vec3 s = normalize(uLight.position.xyz-ePos);
	
	float angle = max( dot(s,eNormal), 0.0 );
	vec3 diffuse = angle * uLight.color * uMaterial.Kd;
	
	return diffuse;
}

void main()
{
	vec4 ePos = uMV * vec4(vPos, 1.0);
	vec3 eNormal = normalize( uNMV * vNor);
	color = phongModel(ePos.xyz, eNormal);
	
	gl_Position = uMVP * vec4(vPos, 1.0);
}

