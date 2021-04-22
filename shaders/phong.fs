// per-pixel model
#version 400

in vec4 ePos;
in vec3 eNormal;

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

out vec4 FragColor;

vec3 phongModel(vec3 ePos, vec3 eNormal)
{
	vec3 s = normalize(uLight.position.xyz-ePos);
	vec3 v = normalize(-ePos.xyz);
	vec3 r = reflect( -s, eNormal );
	
	float angle = max( dot(s,eNormal), 0.0 );
	vec3 diffuse = angle * uLight.color * uMaterial.Kd;
	vec3 ambient = uLight.color * uMaterial.Ka;
	vec3 spec = vec3(0.0);

	if( angle > 0.0 )
	spec = uLight.color * uMaterial.Ks *
	pow( max( dot(r,v), 0.0 ), uMaterial.shininess );

	return ambient + diffuse + spec;
}

void main()
{
	vec3 color = phongModel(ePos.xyz, eNormal);
	FragColor = vec4(color, 1.0);
}

/*
// per-vertex model
#version 400

in vec3 color;
out vec4 FragColor;

void main()
{
   FragColor = vec4(color, 1.0);
}
*/