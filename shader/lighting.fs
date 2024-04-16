#version 330 core

struct Light {
    // Point & Spot
    vec3    position;
    float   constant;
    float   linear;
    float   quadratic;

    // Directional & Spot
    vec3    direction;

    // Spot
    vec2    cutoff;

    // All
    vec3    ambient;
    vec3    diffuse;
    vec3    specular;
};

struct Material {
    sampler2D   diffuse;
    sampler2D   specular;
    float       shininess;
};


in VS_OUT {
  vec3 position;
  vec3 normal;
  vec2 texCoord;
  vec4 lightPosition; // directional shadow
} fs_in;

out vec4 fragColor;

uniform vec3 viewPos;
uniform int lightType;
uniform Light light;
uniform Material material;
uniform bool isPick;
uniform bool isBlinn;

// directional shadow
uniform sampler2D depthMap;

// omni-directional shadow
uniform samplerCube depthMap3d;
uniform float far_plane;

float ShadowCalculation2d(vec3 normal, vec3 lightDir) {
    vec3  depthMapCoords  = (fs_in.lightPosition.xyz / fs_in.lightPosition.w) * 0.5 + 0.5;
    float closestDepth    = texture(depthMap, depthMapCoords.xy).r;
    float currentDepth    = depthMapCoords.z;
    float bias            = max(0.02 * (1.0 - dot(normal, lightDir)), 0.001);
    float shadow          = 0.0;
    vec2  texelSize       = 1.0 / textureSize(depthMap, 0);
    int   count           = 1;

    for(int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depthMap, depthMapCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            ++count;
        }
    }
    shadow /= count;

    return shadow;
}

float ShadowCalculation3d()
{
    vec3  toLight       = fs_in.position - light.position;
    float closestDepth  = texture(depthMap3d, normalize(toLight)).r * far_plane;
    float currentDepth  = length(toLight);
    float bias          = 0.05; 
    float shadow        = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}  

vec3 calcAmbient(vec3 texColor) {
    return texColor * light.ambient;
}

vec3 calcDiffuse(vec3 normal, vec3 texColor, vec3 lightDir) {
    float   diff      = max(dot(normal, lightDir), 0.0);

    return diff * texColor * light.diffuse;
}

vec3 calcSpecular(vec3 normal, vec3 lightDir) {
    vec3    specColor = texture(material.specular, fs_in.texCoord).xyz;
    float   spec      = 0.0;
    vec3    viewDir   = normalize(viewPos - fs_in.position);
    if (isBlinn) {
      vec3 halfDir    = normalize(lightDir + viewDir);
      spec            = pow(max(dot(halfDir, normal), 0.0), material.shininess);
    } else {
      vec3 reflectDir = reflect(-lightDir, normal);
      spec            = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    return spec * specColor * light.specular;
}

vec3 directionalLight() {
    vec3  texColor = texture(material.diffuse, fs_in.texCoord).xyz;
    vec3  lightDir = normalize(-light.direction);
    vec3  normal   = normalize(fs_in.normal);

    vec3  ambient  = calcAmbient(texColor);
    vec3  diffuse  = calcDiffuse(normal, texColor, lightDir);
    vec3  specular = calcSpecular(normal, lightDir);
    float shadow   = ShadowCalculation2d(normal, lightDir);

    return ambient + (diffuse + specular) * (1.0 - shadow);
}

float calcAttenuation(float dist) {
    return (1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist)));
}

vec3 pointLight() {
    vec3    texColor    = texture(material.diffuse, fs_in.texCoord).xyz;
    float   dist        = length(light.position - fs_in.position);
    float   attenuation = calcAttenuation(dist);
    vec3    lightDir    = (light.position - fs_in.position) / dist;
    vec3    normal      = normalize(fs_in.normal);

    vec3    ambient   = calcAmbient(texColor);
    vec3    diffuse   = calcDiffuse(normal, texColor, lightDir);
    vec3    specular  = calcSpecular(normal, lightDir);
    float   shadow    = ShadowCalculation3d();

    return (ambient + (diffuse + specular) * (1.0 - shadow)) * attenuation;
}

vec3 spotLight() {
    vec3    texColor    = texture(material.diffuse, fs_in.texCoord).xyz;
    float   dist        = length(light.position - fs_in.position);
    float   attenuation = calcAttenuation(dist);
    vec3    lightDir    = (light.position - fs_in.position) / dist;

    vec3 result = calcAmbient(texColor);

    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutoff[0] - light.cutoff[1];
    float intensity = clamp((theta - light.cutoff[1]) / epsilon, 0.0, 1.0);

    if (intensity > 0.0) {
        vec3  normal    = normalize(fs_in.normal);
        vec3  diffuse   = calcDiffuse(normal, texColor, lightDir);
        vec3  specular  = calcSpecular(normal, lightDir);
        float shadow    = ShadowCalculation2d(normal, lightDir);
        result += (diffuse + specular) * intensity * (1.0 - shadow);
    }
    result *= attenuation;

    return result;
}

void main() {
    vec3 result;

    if (lightType == 0) {
        result = directionalLight();
    } else if (lightType == 1) {
        result = pointLight();
    } else {
        result = spotLight();
    }

    fragColor = vec4(result, 1.0);
    if (isPick) {
      fragColor = vec4(pow(fragColor.rgb, vec3(0.3)), 1.0);
    }
}
