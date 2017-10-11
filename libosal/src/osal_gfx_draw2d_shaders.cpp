#include "osal_gfx_draw2d.h"

using namespace osal;

// ----------------------------------------------------------------------------
// Plain Color Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_col_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    out vec4 vColor;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    varying   vec4 vColor;
  #endif
  void main() {
      vColor = inColor;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_col_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
  #if __VERSION__ >= 140
    in  vec4 vColor;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
  #endif
  void main() {
    #if __VERSION__ >= 140
      oFragColor = vColor;
    #else
      gl_FragColor = vColor;
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
// Simple Texture Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_tex_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_tex_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    in  vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
  #endif
  void main() {
    #if __VERSION__ >= 140
      oFragColor = vColor * texture2D(uTex0, vTexcoord);
    #else
      gl_FragColor = vColor * texture2D(uTex0, vTexcoord);
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
// Gray Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_gray_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_gray_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    in  vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
  #endif
  void main() {
      vec4 c = texture2D(uTex0, vTexcoord.xy);
      c.rgb = vec3(c.r * 0.33 + c.g * 0.59 + c.b * 0.11);
    #if __VERSION__ >= 140
      oFragColor = vColor * c;
    #else
      gl_FragColor = vColor * c;
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
// Blur Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_blur_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_blur_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    int vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
  #endif
  void main() {
      vec4 sum = vec4(0.0);
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.028))*0.0044299121055113265;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.024))*0.00895781211794;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.020))*0.0215963866053;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.016))*0.0443683338718;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.012))*0.0776744219933;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.008))*0.115876621105;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, -0.004))*0.147308056121;
      sum += texture2D(uTex0, vTexcoord)*0.159576912161;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.004))*0.147308056121;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.008))*0.115876621105;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.012))*0.0776744219933;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.016))*0.0443683338718;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.020))*0.0215963866053;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.024))*0.00895781211794;
      sum += texture2D(uTex0, vTexcoord + vec2(0.0, 0.028))*0.0044299121055113265;
    #if __VERSION__ >= 140
      oFragColor = vColor * sum;
    #else
      gl_FragColor = vColor * sum;
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
// Ripple Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_ripple_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_ripple_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
    uniform float uTime;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    in  vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
  #endif
  void main() {
      vec2 tc = vTexcoord.xy;
      vec2 p = -1.0 + 2.0 * tc;
      float len = length(p);
      vec2 uv = tc + (p / len)*cos(len*12.0 - uTime*6.2831852)*0.02;
    #if __VERSION__ >= 140
      oFragColor = vColor * texture2D(uTex0, uv);
    #else
      gl_FragColor = vColor * texture2D(uTex0, uv);
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
// Fish Eye Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_fisheye_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_fisheye_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
    uniform float uTime;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    in  vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
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
)GLSL";
// ----------------------------------------------------------------------------
// Dream Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_dream_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_dream_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
    uniform float uTime;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    in  vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
  #endif
  void main() {
      vec2 uv = vTexcoord.xy;
      vec4 c = texture2D(uTex0, uv);
      c += texture2D(uTex0, uv + 0.001);
      c += texture2D(uTex0, uv + 0.003);
      c += texture2D(uTex0, uv + 0.005);
      c += texture2D(uTex0, uv + 0.007);
      c += texture2D(uTex0, uv + 0.009);
      c += texture2D(uTex0, uv + 0.011);
      c += texture2D(uTex0, uv - 0.001);
      c += texture2D(uTex0, uv - 0.003);
      c += texture2D(uTex0, uv - 0.005);
      c += texture2D(uTex0, uv - 0.007);
      c += texture2D(uTex0, uv - 0.009);
      c += texture2D(uTex0, uv - 0.011);
      c.rgb = vec3(c.r * 0.33 + c.g * 0.59 + c.b * 0.11);
    #if __VERSION__ >= 140
      oFragColor = vColor * c / 14.5;
    #else
      gl_FragColor = vColor * c / 14.5;
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
// Thermo Shader
// ----------------------------------------------------------------------------
const char* Draw2D::m_shader_thermo_v = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform vec2 uScreenHalf;
  #if __VERSION__ >= 140
    in  vec4 inPosition;
    in  vec4 inColor;
    in  vec2 inTexcoord;
    out vec4 vColor;
    out vec2 vTexcoord;
  #else
    attribute vec2 inPosition;
    attribute vec4 inColor;
    attribute vec2 inTexcoord;
    varying   vec4 vColor;
    varying   vec2 vTexcoord;
  #endif
  void main() {
      vColor = inColor;
      vTexcoord = inTexcoord;
      gl_Position = vec4(inPosition.x / uScreenHalf.x - 1.0, inPosition.y / uScreenHalf.y - 1.0, 0.0, 1.0);
  }
)GLSL";
const char* Draw2D::m_shader_thermo_f = R"GLSL(
  #ifdef GL_ES
    precision highp float;
  #endif
    uniform sampler2D uTex0;
    uniform float uTime;
  #if __VERSION__ >= 140
    in  vec4 vColor;
    in  vec2 vTexcoord;
    out vec4 oFragColor;
  #else
    varying vec4 vColor;
    varying vec2 vTexcoord;
  #endif
  void main() {
      vec2 uv = vTexcoord.xy;
      vec3 tc = vec3(1.0, 0.0, 0.0);
      if (uv.x < (1.0 - 0.005)) {
          vec3 pixcol = texture2D(uTex0, uv).rgb;
          vec3 colors[3];
          colors[0] = vec3(0., 0., 1.);
          colors[1] = vec3(1., 1., 0.);
          colors[2] = vec3(1., 0., 0.);
          float lum = dot(vec3(0.30, 0.59, 0.11), pixcol.rgb);
          int ix = (lum < 0.5) ? 0 : 1;
          tc = mix(colors[ix], colors[ix + 1], (lum - float(ix)*0.5) / 0.5);
      } else if (uv.x >= (1.0 + 0.005)) {
          tc = texture2D(uTex0, uv).rgb;
      }
    #if __VERSION__ >= 140
      oFragColor = vColor * vec4(tc, 1.0);
    #else
      gl_FragColor = vColor * vec4(tc, 1.0);
    #endif
  }
)GLSL";
// ----------------------------------------------------------------------------
