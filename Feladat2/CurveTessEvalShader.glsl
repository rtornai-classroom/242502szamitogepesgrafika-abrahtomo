#version 400 core
#define BEZIER_BERNSTEIN 3

layout (isolines, equal_spacing, ccw) in;

uniform mat4 matModelView;
uniform mat4 matProjection;
uniform int controlPointsNumber;

float NCR(int n, int r) {
    if (r == 0) return 1;
    float result = 1.0f;
    for (int k = 1; k <= r; ++k) {
        result *= n - k + 1;
        result /= k;
    }
    return result;
}

float blending(int n, int i, float t) {
    return NCR(n, i) * pow(t, i) * pow(1.0f - t, n - i);
}

vec3 BezierCurve(float t) {
    vec3 nextPoint = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < controlPointsNumber; i++)
        nextPoint += blending(controlPointsNumber - 1, i, t) * vec3(gl_in[i].gl_Position);
    return nextPoint;
}

void main() {
    gl_Position = matProjection * matModelView * vec4(BezierCurve(gl_TessCoord.x), 1.0);
}
