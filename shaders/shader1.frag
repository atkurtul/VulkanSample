#version 450


layout(location = 0) in vec2 tex;
layout(location = 1) in mat3 norm;
layout(location = 4) in vec4 pos;
layout(location = 5) in vec3 fnorm;
layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform sampler2D albedo;
layout(set = 2, binding = 1) uniform sampler2D normal;
layout(set = 2, binding = 2) uniform sampler2D metalic;


layout(set=0, binding=0) uniform UBO00 {
    mat4 prj;
	vec4 pos;
    ivec4 nLights;
    vec4 lights[1024];
} cam;

const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 srgb_to_linear(vec3 c) {
    return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

void main() 
{
	vec3 col = texture(albedo, tex).rgb;
	vec3 metal = texture(metalic, tex).rgb;
    vec3 N = texture(normal, tex).rgb * 2 - 1;
    N = normalize(norm * N);
    if(cam.nLights.y == 1)
        N = normalize(norm * vec3(0, 0, 1));
    vec3 V = normalize((cam.pos - pos).xyz);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, col, metal);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < cam.nLights.x; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize((cam.lights[2*i] - pos).xyz);
        vec3 H = normalize(V + L);
        float distance    = length(L);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = cam.lights[2*i + 1].xyz * attenuation;        
        
        // cook-torrance brdf
        float roughness = float(cam.nLights.z) / 255.0;
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metal;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * col / PI + specular) * radiance * NdotL; 
    }   
  
    vec3 ambient = vec3(0.03) * col;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    outColor = vec4(color, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  