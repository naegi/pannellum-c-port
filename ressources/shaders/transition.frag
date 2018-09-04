#version 430 core
out vec4 FragColor;
in vec3 coord;

uniform sampler2D tex;

#define tau 6.28318530718

void main() {
    vec3 a = normalize(coord);
    float lat = -asin(a.y);
    float lon = atan(a.x, a.z);
    float x = lon / tau + 0.5;
    float y = lat / tau * 2.0 + 0.5;

    FragColor = texture2D(tex, vec2(x, y));
}
