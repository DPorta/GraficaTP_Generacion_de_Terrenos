#version 330 core

in vec3 Normal;
in vec3 fragColor;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 xyz;           // light position
uniform vec3 xyzColor;      // light color
uniform vec3 xyzView;       // camera position
uniform sampler2D texture0; // textura

out vec4 color;

void main() {
    // ambient
    float strength = 0.10;
    vec3 ambient   = strength * xyzColor;

    // diffuse
    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(xyz - fragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * xyzColor;

    // specular
    float specularStrength = 0.5;
    vec3  viewDir          = normalize(xyzView - fragPos);
    vec3  reflectDir       = reflect(-lightDir, norm);
    float spec             = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3  specular         = specularStrength * spec * xyzColor;

    vec3 result    = (ambient + diffuse + specular) * fragColor;
    color          = texture(texture0, texCoord)*vec4(result, 1.0);
}