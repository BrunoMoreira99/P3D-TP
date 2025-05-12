#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec4 Position;
} u_Camera;

uniform mat4 u_Transform; // Model matrix

struct VertexOutput
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

layout(location = 0) out VertexOutput v_Output;

void main()
{
    vec4 worldPosition = u_Transform * vec4(a_Position, 1.0);
    v_Output.Position = worldPosition.xyz;
    v_Output.Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    v_Output.TexCoord = a_TexCoord;

    gl_Position = u_Camera.ViewProjection * worldPosition;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

layout(location = 0) in VertexOutput v_Input;

layout(binding = 0) uniform sampler2D u_Texture;

layout(std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
	vec4 Position;
} u_Camera;

// Material properties
uniform vec3 u_Ambient;    // Ambient reflectivity (Ka)
uniform vec3 u_Diffuse;    // Diffuse reflectivity (Kd)
uniform vec3 u_Specular;   // Specular reflectivity (Ks)
uniform float u_Shininess; // Shininess (Ns)

// Light sources
struct AmbientLight {
    bool enabled;
    vec3 color;        // Light color
};

struct DirectionalLight {
    bool enabled;
    vec3 color;        // Light color
    vec3 direction;    // Light direction (world space)
};

struct SpotLight {
    bool enabled;
    vec3 color;        // Light color
    vec3 position;     // Light position (world space)
    vec3 direction;    // Light direction (world space)
    float cutoff;      // Inner cutoff angle (cosine)
    float outerCutoff; // Outer cutoff angle (cosine)
};

struct ConeLight {
    bool enabled;
    vec3 color;        // Light color
    vec3 position;     // Light position (world space)
    vec3 direction;    // Light direction (world space)
    float angle;       // Cone cutoff angle (cosine)
};

uniform AmbientLight u_AmbientLight;
uniform DirectionalLight u_DirLight;
uniform SpotLight u_SpotLight;
uniform ConeLight u_ConeLight;

// --- Light Calculation Functions ---
vec3 CalcAmbientLight(AmbientLight light, vec3 ambientMat) {
    if (!light.enabled) return vec3(0.0);
    return light.color * ambientMat;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 N, vec3 V, vec3 diffuseMat, vec3 specularMat, float shininess) {
    if (!light.enabled) return vec3(0.0);
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    return light.color * (diffuseMat * diff + specularMat * spec);
}

vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 N, vec3 V, vec3 diffuseMat, vec3 specularMat, float shininess) {
    if (!light.enabled) return vec3(0.0);
    vec3 L = normalize(light.position - fragPos);
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    vec3 H = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    return intensity * light.color * (diffuseMat * diff + specularMat * spec);
}

vec3 CalcConeLight(ConeLight light, vec3 fragPos, vec3 N, vec3 V, vec3 diffuseMat, vec3 specularMat, float shininess) {
    if (!light.enabled) return vec3(0.0);
    vec3 L = normalize(light.position - fragPos);
    float theta = dot(L, normalize(-light.direction));
    if (theta > light.angle) {
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), shininess);
        return light.color * (diffuseMat * diff + specularMat * spec);
    }
    return vec3(0.0);
}

void main()
{
    vec3 N = normalize(v_Input.Normal);                             // Normal vector
    vec3 V = normalize(vec3(u_Camera.Position) - v_Input.Position); // View vector
    vec3 result = vec3(0.0);

    result += CalcAmbientLight(u_AmbientLight, u_Ambient);
    result += CalcDirectionalLight(u_DirLight, N, V, u_Diffuse, u_Specular, u_Shininess);
    result += CalcSpotLight(u_SpotLight, v_Input.Position, N, V, u_Diffuse, u_Specular, u_Shininess);
    result += CalcConeLight(u_ConeLight, v_Input.Position, N, V, u_Diffuse, u_Specular, u_Shininess);

    vec4 textureColor = texture(u_Texture, v_Input.TexCoord); // Sample the texture
    o_Color = vec4(result, 1.0) * textureColor; // Combine lighting with texture color
}
