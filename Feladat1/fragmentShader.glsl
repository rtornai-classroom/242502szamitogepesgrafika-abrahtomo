#version 330 core
out vec4 FragColor;
uniform vec2 circlePos;
uniform float radius;
uniform vec2 resolution;

void main() {
    vec2 coord = gl_FragCoord.xy;
    float dist = distance(coord, circlePos);
    if (dist < radius) {
        float mixVal = dist / radius;
        vec3 inner = vec3(128.0/255.0, 0.0, 0.0);
        vec3 outer = vec3(173.0/255.0, 1.0, 47.0/255.0);
        FragColor = vec4(mix(inner, outer, mixVal), 1.0);
    } else if (abs(coord.y - resolution.y / 2.0) < 1.5 && 
               coord.x > resolution.x / 3.0 && coord.x < resolution.x * 2.0 / 3.0) {
        FragColor = vec4(65.0/255.0, 105.0/255.0, 225.0/255.0, 1.0);
    } else {
        FragColor = vec4(1.0, 1.0, 0.4, 1.0); // háttér
    }
}