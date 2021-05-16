#version 330 core

in vec3 fragColor;
in vec2 texCoord;

uniform sampler2D texture0;

out vec4 color;

void main() {
    color = texture(texture0, texCoord)*vec4(fragColor,1.0);
}

