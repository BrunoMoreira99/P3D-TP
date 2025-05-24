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

layout(std140, binding = 1) uniform Model
{
    mat4 u_Transform;
};

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

layout(binding = 0) uniform sampler2D u_DiffuseTexture;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
    vec4 Position;
} u_Camera;

layout(std140, binding = 2) uniform MaterialData
{
    vec4 u_Diffuse;    // Diffuse reflectivity (Kd)
    vec4 u_Ambient;    // Ambient reflectivity (Ka)
    vec4 u_Specular;   // Specular reflectivity (Ks)
    float u_Shininess; // Shininess (Ns)
};

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

struct PointLight {
    bool enabled;
    vec3 color;        // Light color
    vec3 position;     // Light position (world space)
    float range;       // Light range
};

struct SpotLight {
    bool enabled;
    vec3 color;        // Light color
    vec3 position;     // Light position (world space)
    vec3 direction;    // Light direction (world space)
    float cutoff;      // Inner cutoff angle (cosine)
    float outerCutoff; // Outer cutoff angle (cosine)
};

uniform AmbientLight u_AmbientLight;
uniform DirectionalLight u_DirLight;
uniform PointLight u_PointLight;
uniform SpotLight u_SpotLight;

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
    float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), shininess) : 0.0;
    return light.color * (diffuseMat * diff + specularMat * spec);
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 N, vec3 V, vec3 diffuseMat, vec3 specularMat, float shininess) {
    if (!light.enabled) return vec3(0.0);
    vec3 lightVec = light.position - fragPos;
    vec3 L = normalize(lightVec);
    float distance = length(lightVec);
    float quadratic = 1.0 / (light.range * light.range);
    float attenuation = 1.0 / (1.0 + quadratic * distance * distance);
    vec3 H = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), shininess) : 0.0;
    return attenuation * light.color * (diffuseMat * diff + specularMat * spec);
}

vec3 CalcSpotLight(SpotLight light, vec3 fragPos, vec3 N, vec3 V, vec3 diffuseMat, vec3 specularMat, float shininess) {
    if (!light.enabled) return vec3(0.0);
    vec3 L = normalize(light.position - fragPos);
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    vec3 H = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), shininess) : 0.0;
    return intensity * light.color * (diffuseMat * diff + specularMat * spec);
}

void main()
{
    vec3 N = normalize(v_Input.Normal);                                 // Normal vector
    vec3 V = normalize(vec3(u_Camera.Position) - v_Input.Position);     // View vector
    vec4 diffuseTexColor = texture(u_DiffuseTexture, v_Input.TexCoord); // Sample the texture
    vec3 result = vec3(0.0);

    vec3 ambientMat = u_Ambient.rgb;
    vec3 diffuseMat = u_Diffuse.rgb * diffuseTexColor.rgb; // Modulate diffuse (Kd) by diffuse texture color (map_Kd)
    vec3 specularMat = u_Specular.rgb;

    result += CalcAmbientLight(u_AmbientLight, ambientMat);
    result += CalcDirectionalLight(u_DirLight, N, V, diffuseMat, specularMat, u_Shininess);
    result += CalcPointLight(u_PointLight, v_Input.Position, N, V, diffuseMat, specularMat, u_Shininess);
    result += CalcSpotLight(u_SpotLight, v_Input.Position, N, V, diffuseMat, specularMat, u_Shininess);

    o_Color = vec4(result, diffuseTexColor.a); // Use texture alpha
}
