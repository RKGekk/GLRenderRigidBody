#version 450 core
#define M_PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 2

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform vec3		viewPos;
uniform DirLight	dirLight;
uniform PointLight	pointLights[NR_POINT_LIGHTS];
uniform SpotLight	spotLight1;
uniform SpotLight	spotLight2;
uniform SpotLight	spotLight3;
uniform SpotLight	spotLight4;
uniform SpotLight	spotLight5;

uniform Material	material;

uniform vec3 gFogColor;
uniform float gFogStart;
uniform float gFogRange;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {    

	vec3 norm = texture(material.specular, fs_in.TexCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = normalize(fs_in.TBN * norm);

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    vec3 result = CalcDirLight(dirLight, norm, viewDir);


	/*
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
		result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
	}*/

    result += CalcSpotLight(spotLight1, norm, fs_in.FragPos, viewDir);
	result += CalcSpotLight(spotLight2, norm, fs_in.FragPos, viewDir);
	result += CalcSpotLight(spotLight3, norm, fs_in.FragPos, viewDir);
	result += CalcSpotLight(spotLight4, norm, fs_in.FragPos, viewDir);
	result += CalcSpotLight(spotLight5, norm, fs_in.FragPos, viewDir);

	vec3 toEyeW = viewPos - fs_in.FragPos;
	float distToEye = length(toEyeW);
	float fogAmount = clamp((distToEye - gFogStart) / gFogRange, 0.0f, 1.0f);
	result = mix(result, gFogColor, fogAmount);

	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) {
        BrightColor = vec4(result, 1.0);
	}
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec; //* vec3(texture(material.specular, fs_in.TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec; //* vec3(texture(material.specular, fs_in.TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec; //* vec3(texture(material.specular, fs_in.TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}