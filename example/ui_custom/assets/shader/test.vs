#version 330, 300 es
#ifdef GL_ES
    precision highp float;
#endif

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
