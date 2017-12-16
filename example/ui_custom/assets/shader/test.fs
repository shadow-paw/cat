  #ifdef GL_ES
    precision highp float;
  #endif
    #define CLIPPING_MAX 4
    uniform vec4 uClipping[CLIPPING_MAX];
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
  float inside(vec2 v, vec2 bottomleft, vec2 topright) {
      vec2 s = step(bottomleft, v) - step(topright, v);
      return s.x * s.y;   
  }
  float clipping(vec2 v) {
      float alpha = 1.0;
      for (int i=0; i<CLIPPING_MAX; i++) {
          alpha *= inside(v, uClipping[i].xy, uClipping[i].zw);
      } return alpha;
  }
  void main() {
      float alpha = clipping(gl_FragCoord.xy);
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
      oFragColor = vColor * texture2D(uTex0, uv) * vec4(1.0, 1.0, 1.0, alpha);
    #else
      gl_FragColor = vColor * texture2D(uTex0, uv) * vec4(1.0, 1.0, 1.0, alpha);
    #endif
  }
