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

in vec3 normal;
in vec2 texCoord;
in vec3 position;

out vec4 fragColor;

uniform vec3 viewPos;
uniform int lightType;
uniform Light light;
uniform Material material;

vec3 calcAmbient(vec3 texColor) {
    vec3 ambient = texColor * light.ambient;

    return ambient;
}

vec3 calcDiffuse(vec3 texColor, vec3 lightDir) {
    vec3    pixelNorm   = normalize(normal);
    float   diff        = max(dot(pixelNorm, lightDir), 0.0);
    vec3    diffuse     = diff * texColor * light.diffuse;

    return diffuse;
}

vec3 calcSpecular(vec3 lightDir) {
    vec3    pixelNorm   = normalize(normal);
    vec3    specColor   = texture(material.specular, texCoord).xyz;
    vec3    viewDir     = normalize(viewPos - position);
    vec3    reflectDir  = reflect(-lightDir, pixelNorm);
    float   spec        = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3    specular    = spec * specColor * light.specular;

    return specular;
}

vec3 directionalLight() {
    vec3 texColor   = texture(material.diffuse, texCoord).xyz;
    vec3 lightDir   = normalize(-light.direction);

    vec3 ambient    = calcAmbient(texColor);
    vec3 diffuse    = calcDiffuse(texColor, lightDir);
    vec3 specular   = calcSpecular(lightDir);

    return ambient + diffuse + specular;
}

float calcAttenuation(float dist) {

    return (1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist)));
}

vec3 pointLight() {
    vec3    texColor    = texture(material.diffuse, texCoord).xyz;
    float   dist        = length(light.position - position);
    float   attenuation = calcAttenuation(dist);
    vec3    lightDir = (light.position - position) / dist;

    vec3    ambient = calcAmbient(texColor);
    vec3    diffuse = calcDiffuse(texColor, lightDir);
    vec3    specular = calcSpecular(lightDir);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 spotLight() {
    vec3    texColor    = texture(material.diffuse, texCoord).xyz;
    float   dist        = length(light.position - position);
    float   attenuation = calcAttenuation(dist);
    vec3    lightDir    = (light.position - position) / length(light.position - position);

    vec3 result = calcAmbient(texColor);

    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutoff[0] - light.cutoff[1];
    float intensity = (theta - light.cutoff[1]) / epsilon;
    intensity = clamp(intensity, 0.0, 1.0);

    if (intensity > 0.0) {
        vec3 diffuse = calcDiffuse(texColor, lightDir);
        vec3 specular = calcSpecular(lightDir);
        result += (diffuse + specular) * intensity;
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
}
