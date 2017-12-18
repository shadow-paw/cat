#version 330, 300 es
precision highp float;

#if defined(VERTEX_SHADER)
uniform  vec2 uCenterMultiplier;
in       vec4 inPosition;
in  lowp vec4 inColor;
in       vec2 inTexcoord;
out lowp vec4 vColor;
out      vec2 vTexcoord;

void main() {
    vColor = inColor;
    vTexcoord = inTexcoord;
    gl_Position = vec4(inPosition.xy * uCenterMultiplier.xy - 1.0, 0.0, 1.0);
}

#elif defined(FRAGMENT_SHADER)
uniform sampler2D uTex0;
uniform float uTime;
in  lowp vec4 vColor;
in       vec2 vTexcoord;
out      vec4 oFragColor;

const float PI = 3.1415926535;
void main() {
    float aperture = 178.0;
    float apertureHalf = 0.5 * aperture * (PI / 180.0);
    float maxFactor = sin(apertureHalf);
    vec2 uv;
    vec2 xy = 2.0 * vTexcoord.xy - 1.0;
    float d = length(xy);
    if (d < (2.0 - maxFactor)) {
        d = length(xy * maxFactor);
        float z = sqrt(1.0 - d * d);
        float r = atan(d, z) / PI;
        float phi = atan(xy.y, xy.x);
        uv.x = r * cos(phi) + 0.5;
        uv.y = r * sin(phi) + 0.5;
    } else {
        uv = vTexcoord.xy;
    }
    oFragColor = vColor * texture(uTex0, uv);
}
#endif
