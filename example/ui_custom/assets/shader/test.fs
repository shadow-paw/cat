  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
    uniform float uTime;
  #if __VERSION__ >= 140
    in  lowp vec4 vColor;
    in       vec2 vTexcoord;
    out      vec4 oFragColor;
  #else
    varying lowp vec4 vColor;
    varying      vec2 vTexcoord;
  #endif
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
    #if __VERSION__ >= 140
      oFragColor = vColor * texture2D(uTex0, uv);
    #else
      gl_FragColor = vColor * texture2D(uTex0, uv);
    #endif
  }
