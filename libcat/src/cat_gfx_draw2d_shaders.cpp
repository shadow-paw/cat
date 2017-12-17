#include "cat_gfx_draw2d.h"

using namespace cat;

// ----------------------------------------------------------------------------
// Plain Color Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_col_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform vec2 uCenterMultiplier;
  in      vec4 inPosition;
  in      vec4 inColor;
  out     vec4 vColor;
  void main() {
      vColor = inColor;
      gl_Position = vec4(inPosition.xy * uCenterMultiplier.xy - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_col_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  in  vec4 vColor;
  out vec4 oFragColor;
  void main() {
      oFragColor = vColor;
  }
)GLSL";
// ----------------------------------------------------------------------------
// Simple Texture Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_tex_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_tex_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform sampler2D uTex0;
  in      vec4 vColor;
  in      vec2 vTexcoord;
  out     vec4 oFragColor;
  void main() {
      oFragColor = vColor * texture(uTex0, vTexcoord);
  }
)GLSL";
// ----------------------------------------------------------------------------
// Gray Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_gray_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_gray_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform sampler2D uTex0;
  in      vec4 vColor;
  in      vec2 vTexcoord;
  out     vec4 oFragColor;
  void main() {
      vec4 c = texture(uTex0, vTexcoord.xy);
      c.rgb = vec3(c.r * 0.33 + c.g * 0.59 + c.b * 0.11);
      oFragColor = vColor * c;
  }
)GLSL";
// ----------------------------------------------------------------------------
// Blur Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_blur_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_blur_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
// ----------------------------------------------------------------------------
// Ripple Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_ripple_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_ripple_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform sampler2D uTex0;
  uniform float uTime;
  in      vec4 vColor;
  in      vec2 vTexcoord;
  out     vec4 oFragColor;
  void main() {
      vec2 p = vTexcoord * 2.0 - 1.0;
      float len = length(p);
      vec2 uv = vTexcoord + (p / len)*cos(len*12.0 - uTime*6.2831852)*0.02;
      oFragColor = vColor * texture(uTex0, uv);
  }
)GLSL";
// ----------------------------------------------------------------------------
// Fish Eye Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_fisheye_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_fisheye_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform sampler2D uTex0;
  uniform float uTime;
  in      vec4 vColor;
  in      vec2 vTexcoord;
  out     vec4 oFragColor;
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
)GLSL";
// ----------------------------------------------------------------------------
// Dream Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_dream_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_dream_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform sampler2D uTex0;
  uniform float uTime;
  in      vec4 vColor;
  in      vec2 vTexcoord;
  out     vec4 oFragColor;
  void main() {
      vec2 uv = vTexcoord.xy;
      vec4 c = texture(uTex0, uv);
      c += texture(uTex0, uv + 0.001);
      c += texture(uTex0, uv + 0.003);
      c += texture(uTex0, uv + 0.005);
      c += texture(uTex0, uv + 0.007);
      c += texture(uTex0, uv + 0.009);
      c += texture(uTex0, uv + 0.011);
      c += texture(uTex0, uv - 0.001);
      c += texture(uTex0, uv - 0.003);
      c += texture(uTex0, uv - 0.005);
      c += texture(uTex0, uv - 0.007);
      c += texture(uTex0, uv - 0.009);
      c += texture(uTex0, uv - 0.011);
      c.rgb = vec3(c.r * 0.33 + c.g * 0.59 + c.b * 0.11);
      oFragColor = vColor * c / 14.5;
  }
)GLSL";
// ----------------------------------------------------------------------------
// Thermo Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_thermo_v = R"GLSL(
  #version 330, 300 es
  precision highp float;
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
)GLSL";
const char* Draw2D::m_shader_thermo_f = R"GLSL(
  #version 330, 300 es
  precision highp float;
  uniform sampler2D uTex0;
  uniform float uTime;
  in      vec4 vColor;
  in      vec2 vTexcoord;
  out     vec4 oFragColor;
  void main() {
      vec2 uv = vTexcoord.xy;
      vec3 tc = vec3(1.0, 0.0, 0.0);
      if (uv.x < (1.0 - 0.005)) {
          vec3 pixcol = texture(uTex0, uv).rgb;
          vec3 colors[3];
          colors[0] = vec3(0., 0., 1.);
          colors[1] = vec3(1., 1., 0.);
          colors[2] = vec3(1., 0., 0.);
          float lum = dot(vec3(0.30, 0.59, 0.11), pixcol.rgb);
          int ix = (lum < 0.5) ? 0 : 1;
          tc = mix(colors[ix], colors[ix + 1], (lum - float(ix)*0.5) / 0.5);
      } else if (uv.x >= (1.0 + 0.005)) {
          tc = texture(uTex0, uv).rgb;
      }
      oFragColor = vColor * vec4(tc, 1.0);
  }
)GLSL";
// ----------------------------------------------------------------------------
