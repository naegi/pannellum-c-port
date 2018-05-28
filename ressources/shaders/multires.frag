#version 430 core
out vec4 FragColor;

in vec2 texCoord;

// texture samplers
uniform sampler2D text;

void main() {
    FragColor = texture(text, texCoord);
}
