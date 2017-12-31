#version 330, 300 es
precision highp float;
#if defined(VERTEX_SHADER)
uniform vec2 uCenterMultiplier;
in      vec4 inPosition;
in      vec4 inColor;
in      vec2 inTexcoord;
out     vec4 vColor;
out     vec2 vTexcoord;
void main() {
    vColor = inColor;
    vTexcoord = inTexcoord;
    gl_Position = vec4(inPosition.xy * uCenterMultiplier.xy - 1.0, 0.0, 1.0);
}
#elif defined(FRAGMENT_SHADER)
uniform sampler2D uTex0;
in      vec4 vColor;
in      vec2 vTexcoord;
out     vec4 oFragColor;
void main() {
    vec4 sum = vec4(0.0);
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.028))*0.0044299121055113265;
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.024))*0.00895781211794;
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.020))*0.0215963866053;
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.016))*0.0443683338718;
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.012))*0.0776744219933;
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.008))*0.115876621105;
    sum += texture(uTex0, vTexcoord + vec2(0.0, -0.004))*0.147308056121;
    sum += texture(uTex0, vTexcoord)*0.159576912161;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.004))*0.147308056121;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.008))*0.115876621105;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.012))*0.0776744219933;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.016))*0.0443683338718;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.020))*0.0215963866053;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.024))*0.00895781211794;
    sum += texture(uTex0, vTexcoord + vec2(0.0, 0.028))*0.0044299121055113265;
    oFragColor = vColor * sum;
}
#endif