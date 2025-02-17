#define MAX_DIRECTIONAL_LIGHTS 2
#define MAX_POINT_LIGHTS 6
#define MAX_SPOT_LIGHTS 4

const float PI = 3.14159265359f; 

layout (location = 0) out vec4 Fragcolor;
layout (location = 1) out vec4 highlightColor; 
layout (location = 2) out vec4 brightColor;

struct Material
{
    sampler2D diffuse_texture;
    vec4      diffuse_color;

    sampler2D specular_texture;
    float     shininess;

    sampler2D occlusion_texture;
    sampler2D normal_texture;

    sampler2D emissive_texture;
    vec3      emissive_color;

	sampler2D dissolve_texture;
	vec3 dissolve_color;

    float roughness;
	float bloomIntensity;
	vec3 specular;
};

struct DirLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight
{
	vec3  position;
	vec3  color;
	float radius;
	float intensity;
};

struct SpotLight
{
	vec3  position;
	vec3  direction;
	vec3  color;
	float inner;
	float outer;
	float radius;
	float intensity;
};

struct Lights
{
	vec3        ambient_color; 
	DirLight    directional[MAX_DIRECTIONAL_LIGHTS];
	int			num_directionals;
	PointLight  points[MAX_POINT_LIGHTS];
	int         num_points;
	SpotLight   spots[MAX_SPOT_LIGHTS];
	int         num_spots;
};


layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

in vec3 normalIn;
in vec3 position;
in vec3 positionWorld;
in vec2 uv0;
in vec3 viewPos;
in vec3 eye_pos;
in vec4 shadow_coord;

in vec3 pointPositions[MAX_POINT_LIGHTS]; //positions in tangent space
in vec3 spotPositions[MAX_SPOT_LIGHTS];   //positions in tangent space
in vec3 spotDirections[MAX_SPOT_LIGHTS];  //directions in tangent space
in vec3 directionalDirections[MAX_DIRECTIONAL_LIGHTS]; //directions in tangent space

uniform Material material;
uniform Lights lights;
uniform int hasNormalMap;
uniform sampler2D shadowTex;
uniform vec3 highlightColorUniform;
uniform float useHighlight;
uniform float borderAmount;
uniform float sliceAmount;
uniform float time;
uniform float waterMix;


vec4 textureGammaCorrected(sampler2D tex)
{
	vec4 texRaw = texture2D(tex, uv0);
	return vec4(pow(texRaw.r, 2.2), pow(texRaw.g, 2.2), pow(texRaw.b, 2.2), texRaw.a);
}

#ifndef WATER
vec4 get_albedo()
{
	return textureGammaCorrected(material.diffuse_texture) * material.diffuse_color;
}
#else
vec4 get_albedo()
{
	return mix(textureGammaCorrected(material.diffuse_texture), textureGammaCorrected(material.dissolve_texture), waterMix)  * material.diffuse_color;
}
#endif

vec3 get_occlusion_color()
{
	return textureGammaCorrected(material.occlusion_texture).rgb;
}

vec3 get_emissive_color()
{
	return textureGammaCorrected(material.emissive_texture).rgb * material.emissive_color;
}

float get_attenuation(float distance, float radius, float intensity)
{	
	float att = intensity / pow((max(distance, radius) / radius + 1), 2);
	return mix(att, 0, distance / radius);
}

float D(vec3 H, vec3 N)
{
	float r2 = pow(material.roughness, 2);
	float NdotH = dot(N,H);
	float NdotH2 = pow(NdotH, 2);
	float num = pow(r2, 2);
	float den = NdotH2 * (num - 1.f) + 1.f;
	return num / max(den, 0.001f);
}

float GGX(vec3 M, vec3 N)
{
	float NdotM = max(dot(N,M), 0.001f);

	float r = (material.roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotM;
    float denom = max(NdotM * (1.0 - k) + k, 0.001f);
	
    return num / denom;
}

float GSmith(vec3 L, vec3 V, vec3 N)
{
	return GGX(L, N) * GGX(V, N);
}

vec3 BRDF(vec3 F, vec3 L, vec3 V, vec3 N, vec3 H)
{
	vec3 num = F * GSmith(L, V, N) * D(H, N);
	float den = 4 * dot(N,L) * dot(N,V);
	return num / max(den, 0.001);
}

vec3 FSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
} 


vec3 CalculateNormal()
{
	vec3 normalM = texture(material.normal_texture, uv0).xyz;
	normalM = normalize(normalM * 2.0 - 1.0);
	return hasNormalMap * normalM + (1 - hasNormalMap) * normalIn;	
}

void main()
{
#ifndef WATER
	vec3 normal = CalculateNormal();	
	vec4 albedo = get_albedo();
#else
	vec3 normal = normalIn;
	vec4 albedo = get_albedo();
#endif
	if (albedo.a < 0.1f)
		discard;

	float metallic = min(1.f, length(material.specular));

	vec3 F0 = mix(albedo.rgb, material.specular, metallic);

	vec3 color = albedo.rgb * lights.ambient_color; 
	
	vec3 N = normal;
	vec3 V = normalize(viewPos - position);
	for(int i=0; i < lights.num_directionals; ++i)
	{
#ifdef SHADOWS_ENABLED
		vec4 sCoord = shadow_coord / shadow_coord.w;
		sCoord = sCoord * .5f + .5f;
		bool isLit = !(sCoord.x >= .0f && sCoord.x <= 1.f 
					&& sCoord.y >= .0f && sCoord.y <= 1.f
					&& texture2D(shadowTex, sCoord.xy).x < clamp(sCoord.z, 0, 1) - 0.005f);
#endif					
#ifndef WATER
		vec3 L = directionalDirections[i];
#else
		vec3 L = lights.directional[i].direction;
#endif
		vec3 H = normalize(V + L);	

		vec3 radiance = lights.directional[i].color * lights.directional[i].intensity;				
		
		vec3 F = FSchlick(max(dot(H, V), 0.0), F0);   

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0f - metallic;
		
		float NdotL = max(dot(N, L), 0.0);        
		color += (kD * albedo.rgb / PI + BRDF(F, L, V, N, H)) * radiance * NdotL;  
#ifdef SHADOWS_ENABLED
		if (!isLit)
		{
			color = color * .01f;	
		}
#endif		
	}
	for(int i=0; i < lights.num_points; ++i)
	{	
#ifndef WATER
		vec3 lightPos = pointPositions[i];
#else
		vec3 lightPos = lights.points[i].position;
#endif
		vec3 L = normalize(lightPos - position);
		vec3 H = normalize(V + L);

		float distance = length(lightPos - position);

		float att = max(get_attenuation(distance, lights.points[i].radius, lights.points[i].intensity), 0);

		vec3 radiance = lights.points[i].color * att;				
		
		vec3 F = FSchlick(max(dot(H, V), 0.0), F0);   

		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
		kD *= 1.0f - metallic;

		float NdotL = max(dot(N, L), 0.0);        
		color += (kD * albedo.rgb / PI + BRDF(F, L, V, N, H)) * radiance * NdotL;  
	}
	
	for(int i=0; i < lights.num_spots; ++i)
	{
		vec3 lightPos = spotPositions[i];
		vec3 L = normalize(lightPos - position);
		vec3 H = normalize(V + L);
		float distance = length(lightPos - position);

		float theta = dot(normalize(L), normalize(-(spotDirections[i])));
		float epsilon = max(0.0001, lights.spots[i].inner - lights.spots[i].outer);
		float cone = clamp((theta - lights.spots[i].outer) / epsilon, 0.0, 1.0); 
	
		float att = max(get_attenuation(distance, lights.spots[i].radius, lights.spots[i].intensity), 0);
		vec3 radiance = lights.spots[i].color * att * cone;
		
		vec3 F = FSchlick(max(dot(H, V), 0.0), F0);   

		vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
		kD *= 1.0f - metallic;
      
		float NdotL = max(dot(N, L), 0.0);        
		color += (kD * albedo.rgb / PI + BRDF(F, L, V, N, H)) * radiance * NdotL;
	}
	
	//color *= get_occlusion_color();
	color += get_emissive_color() * material.bloomIntensity;

#ifdef IS_EDITOR
	color = vec3(pow(color.r, (1.0 / 2.2)), pow(color.g, (1.0 / 2.2)), pow(color.b, (1.0 / 2.2)));
#endif
	Fragcolor = vec4(color, albedo.a);
	
	highlightColor = vec4(highlightColorUniform, 1);
	
	float brightness = dot(Fragcolor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1)
        brightColor = vec4(Fragcolor.rgb, albedo.a);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
#ifdef DISSOLVE
	float phi = texture2D(material.dissolve_texture, uv0).r - sliceAmount;
	if (phi < 0 && phi > -borderAmount) 
	{
		Fragcolor = vec4(material.dissolve_color, Fragcolor.a);
	}
	else if (phi < 0)
	{
		discard;
	}		
#endif	

	//Fragcolor = vec4(normal,1);
			
	//Fragcolor = texture2D(material.dissolve_texture, uv0);
}
