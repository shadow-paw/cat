  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uCenterMultiplier;
  #if __VERSION__ >= 140
    in       vec4 inPosition;
    in  lowp vec4 inColor;
    in       vec2 inTexcoord;
    out lowp vec4 vColor;
    out      vec2 vTexcoord;
  #else
    attribute      vec2 inPosition;
    attribute lowp vec4 inColor;
    attribute      vec2 inTexcoord;
    varying   lowp vec4 vColor;
    varying        vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.xy * uCenterMultiplier.xy - 1.0, 0.0, 1.0);
  }
