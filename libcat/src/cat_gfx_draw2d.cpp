#include <new>
#include "cat_gfx_draw2d.h"
#include "cat_gfx_shader.h"
#include "cat_storage_resmgr.h"
#include "cat_util_log.h"

using namespace cat;

// Fix android
#if defined(PLATFORM_ANDROID)
  #define GRAPHITE_DRAWTEXT_FLIPPED
#endif

std::unordered_map<int, std::string> Draw2D::s_uniforms = {
    { Draw2D::u_CenterMultiplier, "uCenterMultiplier" },
    { Draw2D::u_Tex0, "uTex0" },
    { Draw2D::u_Time, "uTime" }
};
std::unordered_map<int, std::string> Draw2D::s_attrs = {
    { Draw2D::in_Position, "inPosition" },
    { Draw2D::in_Color,    "inColor"    },
    { Draw2D::in_Texcoord, "inTexcoord" }
};
// ----------------------------------------------------------------------------
Draw2D::Draw2D() {
    for (auto&& shader: m_shaders) {
        shader = nullptr;
    }
    m_width = m_height = m_scaled_height = 1;
    m_scale = 1.0f;
    // Text
    m_textcursor.plane = 0;
    m_textcursor.x = m_textcursor.y = 0;
    m_textcursor.height = 0;
    m_uniforms.clipping.enabled = false;
    m_render_target = nullptr;
}
// ----------------------------------------------------------------------------
Draw2D::~Draw2D() {
    fini();
}
// ----------------------------------------------------------------------------
bool Draw2D::init() {
    std::unordered_map<Effect,std::string> sources = {
        {Effect::Color, m_shader_color},
        {Effect::Tex, m_shader_tex},
        {Effect::Gray, m_shader_gray},
        {Effect::Blur, m_shader_blur},
        {Effect::Ripple, m_shader_ripple},
        {Effect::Fisheye, m_shader_fisheye},
        {Effect::Dream, m_shader_dream},
        {Effect::Thermo, m_shader_thermo}
    };
    // Texture Shaders
    for (auto it=sources.begin(); it!=sources.end(); ++it) {
        const auto& effect = it->first;
        const auto& source = it->second;

        Shader* shader = new (std::nothrow) Shader();
        if (!shader) goto fail;
        if (!shader->init(source.c_str(), source.c_str())) {
            Logger::e("gfx", "Failed to compile builtin shader effect = %d", effect);
            delete shader;
            continue;
        }
        shader->bind();
        for (auto& kv : s_uniforms) {
            shader->bind_uniform(kv.first, kv.second);
        }
        for (auto& kv : s_attrs) {
            shader->bind_attr(kv.first, kv.second);
        }
        shader->unbind();
        m_shaders[effect] = shader;
    }
    if (!m_vbo.init(54 * sizeof(Vertex2f), true)) goto fail;
    m_fbo.init();
    // Text
    m_drawable[0].resize(kTextTextureWidth, kTextTextureHeight);
    return true;
fail:
    fini();
    return false;
}
// ----------------------------------------------------------------------------
void Draw2D::fini() {
    m_width = m_height = 0;
    m_fbo.fini();
    m_vbo.fini();
    for (auto&& shader : m_shaders) {
        if (shader) { delete shader; shader = nullptr; }
    }
    // Text
    m_texts.clear();
    for (int i = 0; i < kTextTextureMax; i++) {
        m_drawable[i].release();
    }
}
// ----------------------------------------------------------------------------
void Draw2D::resize(int width, int height) {
    if (m_width==width && m_height==height) return;
    m_width = width;
    m_height = height;
    m_scaled_height = (int)(m_height / m_scale);
    m_uniforms.center_multiplier.x = m_scale * 2 / m_width;
    m_uniforms.center_multiplier.y = m_scale * 2 / m_height;
}
// ----------------------------------------------------------------------------
void Draw2D::scale(float factor) {
    if (m_scale == factor) return;
    m_scale = factor;
    m_scaled_height = (int)(m_height / m_scale);
    m_uniforms.center_multiplier.x = m_scale * 2 / m_width;
    m_uniforms.center_multiplier.y = m_scale * 2 / m_height;
}
// ----------------------------------------------------------------------------
float Draw2D::convert_y(int y) const {
    if (m_render_target) return (float)(m_render_target->height() - y);
    return (float)(m_scaled_height - y);
}
// ----------------------------------------------------------------------------
void Draw2D::drawline(int x1, int y1, int x2, int y2, uint32_t color) {
    float x[2], y[2];
    x[0] = (float)x1;
    x[1] = (float)x2;
    y[0] = convert_y(y1);
    y[1] = convert_y(y2);
    Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
    vertex[0].x = x[0]; vertex[0].y = y[0]; vertex[0].color = color;
    vertex[1].x = x[1]; vertex[1].y = y[1]; vertex[1].color = color;
    m_vbo.unlock();

    // clipping
    if (m_uniforms.clipping.enabled && !m_render_target) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_uniforms.clipping.x, m_uniforms.clipping.y, m_uniforms.clipping.w, m_uniforms.clipping.h);
    }
    auto& shader = m_shaders[Effect::Color];
    m_vbo.bind();
    shader->bind();
    shader->uniform(u_CenterMultiplier, m_render_target ? glm::vec2(2.0f / m_render_target->width(), 2.0f / m_render_target->height()) : m_uniforms.center_multiplier);
    shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, x));
    shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, color));
    shader->draw(Shader::kLine, 0, 2);
    shader->clr_attr(in_Color);
    shader->clr_attr(in_Position);
    shader->unbind();
    m_vbo.unbind();
    if (m_uniforms.clipping.enabled && !m_render_target) {
        glDisable(GL_SCISSOR_TEST);
    }
}
// ----------------------------------------------------------------------------
void Draw2D::outline(const Rect2i& rect, uint32_t color) {
    float x[2], y[2];
    x[0] = (float)rect.origin.x;
    x[1] = (float)(rect.origin.x+ rect.size.width+1);
    y[0] = convert_y(rect.origin.y);
    y[1] = convert_y(rect.origin.y + rect.size.height+1);
    Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
    vertex[0].x = x[0]; vertex[0].y = y[0]; vertex[0].color = color;
    vertex[1].x = x[0]; vertex[1].y = y[1]; vertex[1].color = color;
    vertex[2].x = x[1]; vertex[2].y = y[1]; vertex[2].color = color;
    vertex[3].x = x[1]; vertex[3].y = y[0]; vertex[3].color = color;
    m_vbo.unlock();

    if (m_uniforms.clipping.enabled && !m_render_target) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_uniforms.clipping.x, m_uniforms.clipping.y, m_uniforms.clipping.w, m_uniforms.clipping.h);
    }
    auto& shader = m_shaders[Effect::Color];
    m_vbo.bind();
    shader->bind();
    shader->uniform(u_CenterMultiplier, m_render_target ? glm::vec2(2.0f / m_render_target->width(), 2.0f / m_render_target->height()) : m_uniforms.center_multiplier);
    shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, x));
    shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, color));
    shader->draw(Shader::kLineLoop, 0, 4);
    shader->clr_attr(in_Color);
    shader->clr_attr(in_Position);
    shader->unbind();
    m_vbo.unbind();
    if (m_uniforms.clipping.enabled && !m_render_target) {
        glDisable(GL_SCISSOR_TEST);
    }
}
// ----------------------------------------------------------------------------
void Draw2D::fill(const Rect2i& rect, uint32_t color) {
    float x[2], y[2];
    x[0] = (float)rect.origin.x;
    x[1] = (float)(rect.origin.x + rect.size.width+1);
    y[0] = convert_y(rect.origin.y);
    y[1] = convert_y(rect.origin.y + rect.size.height+1);
    Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
    vertex[0].x = x[0]; vertex[0].y = y[0]; vertex[0].color = color;
    vertex[1].x = x[0]; vertex[1].y = y[1]; vertex[1].color = color;
    vertex[2].x = x[1]; vertex[2].y = y[1]; vertex[2].color = color;
    vertex[3].x = x[1]; vertex[3].y = y[0]; vertex[3].color = color;
    m_vbo.unlock();

    if (m_uniforms.clipping.enabled && !m_render_target) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(m_uniforms.clipping.x, m_uniforms.clipping.y, m_uniforms.clipping.w, m_uniforms.clipping.h);
    }
    auto& shader = m_shaders[Effect::Color];
    m_vbo.bind();
    shader->bind();
    shader->uniform(u_CenterMultiplier, m_render_target ? glm::vec2(2.0f / m_render_target->width(), 2.0f / m_render_target->height()) : m_uniforms.center_multiplier);
    shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, x));
    shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, color));
    shader->draw(Shader::kTriangleFan, 0, 4);
    shader->clr_attr(in_Color);
    shader->clr_attr(in_Position);
    shader->unbind();
    m_vbo.unbind();
    if (m_uniforms.clipping.enabled && !m_render_target) {
        glDisable(GL_SCISSOR_TEST);
    }
}
// ----------------------------------------------------------------------------
void Draw2D::fill(const Rect2i& rect, uint32_t color, const Texture* tex, unsigned long t, Effect effect) {
    // if (!m_shaders[effect]) effect = Effect::None;
    fill(rect, color, tex, t, m_shaders[effect]);
}
// ----------------------------------------------------------------------------
void Draw2D::fill(const Rect2i& rect, uint32_t color, const TextureRef& texref, unsigned long t, Effect effect) {
    // if (!m_shaders[effect]) effect = Effect::None;
    fill(rect, color, texref, t, m_shaders[effect]);
}
// ----------------------------------------------------------------------------
void Draw2D::fill(const Rect2i& rect, uint32_t color, const Texture* tex, unsigned long t, const Shader* shader) {
    if (color == 0) return;
    if (tex) {
        float x[2], y[2];
        x[0] = (float)rect.origin.x;
        x[1] = (float)(rect.origin.x+rect.size.width+1);
        y[0] = convert_y(rect.origin.y);
        y[1] = convert_y(rect.origin.y + rect.size.height+1);

        Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
        vertex[0] = { x[0], y[0], 0.0f, 1.0f, color };
        vertex[1] = { x[0], y[1], 0.0f, 0.0f, color };
        vertex[2] = { x[1], y[0], 1.0f, 1.0f, color };
        vertex[3] = { x[1], y[1], 1.0f, 0.0f, color };
        m_vbo.unlock();

        if (m_uniforms.clipping.enabled && !m_render_target) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(m_uniforms.clipping.x, m_uniforms.clipping.y, m_uniforms.clipping.w, m_uniforms.clipping.h);
        }
        shader->bind();
        m_vbo.bind();
        tex->bind(0);
        shader->uniform(u_CenterMultiplier, m_render_target ? glm::vec2(2.0f / m_render_target->width(), 2.0f / m_render_target->height()) : m_uniforms.center_multiplier);
        shader->uniform(u_Tex0, 0);
        shader->uniform(u_Time, (float)(t & 0xffffff)/1000);
        shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, x));
        shader->set_attr(in_Texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, u));
        shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, color));
        shader->draw(Shader::kTriangleStrip, 0, 4);
        shader->clr_attr(in_Color);
        shader->clr_attr(in_Texcoord);
        shader->clr_attr(in_Position);
        tex->unbind(0);
        m_vbo.unbind();
        shader->unbind();
        if (m_uniforms.clipping.enabled && !m_render_target) {
            glDisable(GL_SCISSOR_TEST);
        }
    } else {
        fill(rect, color);
    }
}
// ----------------------------------------------------------------------------
void Draw2D::fill(const Rect2i& rect, uint32_t color, const TextureRef& texref, unsigned long t, const Shader* shader) {
    if (color == 0) return;
    if (texref.tex) {
        if (texref.tex->width() == 0 || texref.tex->height() == 0) return;
        if (texref.border_u == 0 && texref.border_v == 0) {
            float x[2], y[2], u[2], v[2];
            const float tex_w = (float)texref.tex->width();
            const float tex_h = (float)texref.tex->height();
            x[0] = (float)rect.origin.x;
            x[1] = (float)(rect.origin.x+rect.size.width+1);
            y[0] = convert_y(rect.origin.y);
            y[1] = convert_y(rect.origin.y + rect.size.height+1);
            u[0] = (float)(texref.u1) / tex_w;
            u[1] = (float)(texref.u2 + 1) / tex_w;
            v[0] = (float)(tex_h - texref.v1) / tex_h;
            v[1] = (float)(tex_h - texref.v2 - 1) / tex_h;

            Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
            vertex[0] = { x[0], y[0], u[0], v[0], color };
            vertex[1] = { x[0], y[1], u[0], v[1], color };
            vertex[2] = { x[1], y[0], u[1], v[0], color };
            vertex[3] = { x[1], y[1], u[1], v[1], color };
            m_vbo.unlock();

            if (m_uniforms.clipping.enabled && !m_render_target) {
                glEnable(GL_SCISSOR_TEST);
                glScissor(m_uniforms.clipping.x, m_uniforms.clipping.y, m_uniforms.clipping.w, m_uniforms.clipping.h);
            }
            shader->bind();
            m_vbo.bind();
            texref.tex->bind(0);
            shader->uniform(u_CenterMultiplier, m_render_target ? glm::vec2(2.0f / m_render_target->width(), 2.0f / m_render_target->height()) : m_uniforms.center_multiplier);
            shader->uniform(u_Tex0, 0);
            shader->uniform(u_Time, (float)(t & 0xffffff)/1000);
            shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, x));
            shader->set_attr(in_Texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, u));
            shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, color));
            shader->draw(Shader::kTriangleStrip, 0, 4);
            shader->clr_attr(in_Color);
            shader->clr_attr(in_Texcoord);
            shader->clr_attr(in_Position);
            texref.tex->unbind(0);
            m_vbo.unbind();
            shader->unbind();
            if (m_uniforms.clipping.enabled && !m_render_target) {
                glDisable(GL_SCISSOR_TEST);
            }
        } else {
            // TOP-LEFT
            const float tex_w = (float)texref.tex->width();
            const float tex_h = (float)texref.tex->height();
            float x[4], y[4], u[4], v[4];

            x[0] = (float)(rect.origin.x);
            x[1] = (float)(rect.origin.x + texref.border_u);
            x[2] = (float)(rect.origin.x + rect.size.width+1 - texref.border_u);
            x[3] = (float)(rect.origin.x + rect.size.width+1);
            y[0] = convert_y(rect.origin.y);
            y[1] = convert_y(rect.origin.y + texref.border_v);
            y[2] = convert_y(rect.origin.y + rect.size.height+1 - texref.border_v);
            y[3] = convert_y(rect.origin.y + rect.size.height+1);
            u[0] = ((float)(texref.u1)) / tex_w;
            u[1] = ((float)(texref.u1 + texref.border_u )) / tex_w;
            u[2] = ((float)(texref.u2 - texref.border_u + 1)) / tex_w;
            u[3] = ((float)(texref.u2 + 1)) / tex_w;
            v[0] = ((float)(tex_h - texref.v1)) / tex_h;
            v[1] = ((float)(tex_h - (texref.v1 + texref.border_v +1))) / tex_h;
            v[2] = ((float)(tex_h - (texref.v2 - texref.border_v +1))) / tex_h;
            v[3] = ((float)(tex_h - (texref.v2 + 1))) / tex_h;

            Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
            // TOP-LEFT
            vertex[ 0] = { x[0], y[0], u[0], v[0], color };
            vertex[ 1] = { x[0], y[1], u[0], v[1], color };
            vertex[ 2] = { x[1], y[0], u[1], v[0], color };
            vertex[ 3] = { x[0], y[1], u[0], v[1], color };
            vertex[ 4] = { x[1], y[0], u[1], v[0], color };
            vertex[ 5] = { x[1], y[1], u[1], v[1], color };
            // TOP-CENTER
            vertex[ 6] = { x[1], y[0], u[1], v[0], color };
            vertex[ 7] = { x[1], y[1], u[1], v[1], color };
            vertex[ 8] = { x[2], y[0], u[2], v[0], color };
            vertex[ 9] = { x[1], y[1], u[1], v[1], color };
            vertex[10] = { x[2], y[0], u[2], v[0], color };
            vertex[11] = { x[2], y[1], u[2], v[1], color };
            // TOP-RIGHT
            vertex[12] = { x[2], y[0], u[2], v[0], color };
            vertex[13] = { x[2], y[1], u[2], v[1], color };
            vertex[14] = { x[3], y[0], u[3], v[0], color };
            vertex[15] = { x[2], y[1], u[2], v[1], color };
            vertex[16] = { x[3], y[0], u[3], v[0], color };
            vertex[17] = { x[3], y[1], u[3], v[1], color };
            // VCENTER-LEFT
            vertex[18] = { x[0], y[1], u[0], v[1], color };
            vertex[19] = { x[0], y[2], u[0], v[2], color };
            vertex[20] = { x[1], y[1], u[1], v[1], color };
            vertex[21] = { x[0], y[2], u[0], v[2], color };
            vertex[22] = { x[1], y[1], u[1], v[1], color };
            vertex[23] = { x[1], y[2], u[1], v[2], color };
            // VCENTER-CENTER
            vertex[24] = { x[1], y[1], u[1], v[1], color };
            vertex[25] = { x[1], y[2], u[1], v[2], color };
            vertex[26] = { x[2], y[1], u[2], v[1], color };
            vertex[27] = { x[1], y[2], u[1], v[2], color };
            vertex[28] = { x[2], y[1], u[2], v[1], color };
            vertex[29] = { x[2], y[2], u[2], v[2], color };
            // VCENTER-RIGHT
            vertex[30] = { x[2], y[1], u[2], v[1], color };
            vertex[31] = { x[2], y[2], u[2], v[2], color };
            vertex[32] = { x[3], y[1], u[3], v[1], color };
            vertex[33] = { x[2], y[2], u[2], v[2], color };
            vertex[34] = { x[3], y[1], u[3], v[1], color };
            vertex[35] = { x[3], y[2], u[3], v[2], color };
            // BOTTOM-LEFT
            vertex[36] = { x[0], y[2], u[0], v[2], color };
            vertex[37] = { x[0], y[3], u[0], v[3], color };
            vertex[38] = { x[1], y[2], u[1], v[2], color };
            vertex[39] = { x[0], y[3], u[0], v[3], color };
            vertex[40] = { x[1], y[2], u[1], v[2], color };
            vertex[41] = { x[1], y[3], u[1], v[3], color };
            // BOTTOM-CENTER
            vertex[42] = { x[1], y[2], u[1], v[2], color };
            vertex[43] = { x[1], y[3], u[1], v[3], color };
            vertex[44] = { x[2], y[2], u[2], v[2], color };
            vertex[45] = { x[1], y[3], u[1], v[3], color };
            vertex[46] = { x[2], y[2], u[2], v[2], color };
            vertex[47] = { x[2], y[3], u[2], v[3], color };
            // BOTTOM-RIGHT
            vertex[48] = { x[2], y[2], u[2], v[2], color };
            vertex[49] = { x[2], y[3], u[2], v[3], color };
            vertex[50] = { x[3], y[2], u[3], v[2], color };
            vertex[51] = { x[2], y[3], u[2], v[3], color };
            vertex[52] = { x[3], y[2], u[3], v[2], color };
            vertex[53] = { x[3], y[3], u[3], v[3], color };
            m_vbo.unlock();

            if (m_uniforms.clipping.enabled && !m_render_target) {
                glEnable(GL_SCISSOR_TEST);
                glScissor(m_uniforms.clipping.x, m_uniforms.clipping.y, m_uniforms.clipping.w, m_uniforms.clipping.h);
            }
            shader->bind();
            m_vbo.bind();
            texref.tex->bind(0);
            shader->uniform(u_CenterMultiplier, m_render_target ? glm::vec2(2.0f / m_render_target->width(), 2.0f / m_render_target->height()) : m_uniforms.center_multiplier);
            shader->uniform(u_Tex0, 0);
            shader->uniform(u_Time, (float)(t&0xffffff)/1000);
            shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, x));
            shader->set_attr(in_Texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, u));
            shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), CAT_GFX_OFFSETOF(Vertex2f, color));
            shader->draw(Shader::kTriangles, 0, 54);
            shader->clr_attr(in_Color);
            shader->clr_attr(in_Texcoord);
            shader->clr_attr(in_Position);
            texref.tex->unbind(0);
            m_vbo.unbind();
            shader->unbind();
            if (m_uniforms.clipping.enabled && !m_render_target) {
                glDisable(GL_SCISSOR_TEST);
            }
        }
    } else {
        fill(rect, color);
    }
}
// ----------------------------------------------------------------------------
// Text Functions
// ----------------------------------------------------------------------------
bool Draw2D::render_text() {
    for (auto it = m_texts.begin(); it != m_texts.end(); ) {
        TextEntry& entry = it->second;
        if (entry.expired) {
            it = m_texts.erase(it);
            continue;
        } else if (entry.ready) {
            ++it;
            continue;
        }

        const std::string& text = it->first;
        Rect2i rc;
        m_drawable[m_textcursor.plane].calctext(&rc.size, text, entry.style);
        if (m_textcursor.x + rc.size.width > kTextTextureWidth) {
            m_textcursor.x = 0;
            m_textcursor.y += m_textcursor.height +1;
        }
        if (m_textcursor.y + rc.size.height > kTextTextureHeight) {
            m_textcursor.x = 0;
            m_textcursor.y = 0;
            m_textcursor.height = 0;
            m_textcursor.plane++;
            if (m_textcursor.plane >= kTextTextureMax) m_textcursor.plane = 0;
            m_drawable[m_textcursor.plane].resize(kTextTextureWidth, kTextTextureHeight);
            // NOTE: invalidate everything on new plane
            for (auto inv = m_texts.begin(); inv != m_texts.end();) {
                if (inv->second.tex_plane == m_textcursor.plane) {
                    inv->second.expired = true;
                    inv->second.ready = false;
                }
            }
        }
        if (m_textcursor.height < rc.size.height) m_textcursor.height = rc.size.height;
        rc.origin.x = m_textcursor.x;
        rc.origin.y = m_textcursor.y;
        m_drawable[m_textcursor.plane].begin_draw();
        m_drawable[m_textcursor.plane].drawtext(rc, text, entry.style);
        m_drawable[m_textcursor.plane].end_draw();
        m_textcursor.x += rc.size.width +1;
        entry.tex_plane = m_textcursor.plane;
        entry.size.width = rc.size.width;
        entry.size.height = rc.size.height;
        entry.texref.tex = m_drawable[entry.tex_plane].tex();
        entry.texref.u1 = rc.origin.x;
        entry.texref.u2 = rc.origin.x + rc.size.width;
#ifdef GRAPHITE_DRAWTEXT_FLIPPED
        entry.texref.v2 = rc.origin.y;
        entry.texref.v1 = rc.origin.y + rc.size.height;
#else
        entry.texref.v1 = rc.origin.y;
        entry.texref.v2 = rc.origin.y + rc.size.height;
#endif
        entry.ready = true;
        ++it;
    }
    bool updated = false;
    for (int i = 0; i < kTextTextureMax; i++) {
        updated |= m_drawable[i].update();
    }
    return updated;
}
// ----------------------------------------------------------------------------
void Draw2D::calctext(Size2i *size, const std::string& utf8, const TextStyle& style) {
    m_drawable[m_textcursor.plane].calctext(size, utf8.c_str(), style);
}
// ----------------------------------------------------------------------------
void Draw2D::drawtext(const Rect2i& rect, const std::string& utf8, const TextStyle& style, float opacity) {
    auto range = m_texts.equal_range(utf8);
    for (auto it = range.first; it != range.second; ++it) {
        const TextEntry& entry = it->second;
        if (entry.style.appearance!=style.appearance ||
            entry.style.fontsize!=style.fontsize ||
            entry.style.color != style.color) continue;

        if (entry.ready) {
            Rect2i rc;
            rc.origin.x = rect.origin.x;
            rc.origin.y = rect.origin.y;
            rc.size.width = entry.size.width;
            rc.size.height = entry.size.height;
            if (style.gravity & TextStyle::Gravity::Right) {
                rc.origin.x = rect.origin.x + rect.size.width - rc.size.width - style.padding_x;
            } else if (style.gravity & TextStyle::Gravity::CenterHorizontal) {
                rc.origin.x = rect.origin.x + (rect.size.width-rc.size.width)/2;
            } else rc.origin.x = rect.origin.x + style.padding_x;
            if (style.gravity & TextStyle::Gravity::Bottom) {
                rc.origin.y = rect.origin.y + rect.size.height - rc.size.height - style.padding_y;
            } else if (style.gravity & TextStyle::Gravity::CenterVertical) {
                rc.origin.y = rect.origin.y + (rect.size.height - rc.size.height) / 2;
            } else rc.origin.y = rect.origin.y + style.padding_y;
            fill(rc, 0xffffff | ((uint32_t)(255*opacity)<<24), entry.texref, 0, Effect::Tex);
        }
        return;
    }
    // Add to entry
    TextEntry entry;
    entry.style = style;
    entry.expired = false;
    entry.ready = false;
    m_texts.emplace(utf8, entry);
}
// ----------------------------------------------------------------------------
const Shader* Draw2D::retain_2dshader(ResourceManager* res, const std::string& name) {
    return res->retain_shader(name, s_uniforms, s_attrs);
}
// ----------------------------------------------------------------------------
void Draw2D::release_2dshader(ResourceManager* res, const Shader* shader) {
    res->release_shader(shader);
}
// ----------------------------------------------------------------------------
void Draw2D::clipping_push(const Rect2i& rect) {
    m_clipping.push_back(rect);
    if (m_clipping.size() == 1) {
        m_uniforms.clipping.enabled = true;
    } clipping_update();
}
// ----------------------------------------------------------------------------
void Draw2D::clipping_pop() {
    m_clipping.pop_back();
    if (m_clipping.size() == 0) {
        m_uniforms.clipping.enabled = false;
    } else {
        clipping_update();
    }
}
// ----------------------------------------------------------------------------
void Draw2D::clipping_update() {
    float clipping_x1 = 0;
    float clipping_y1 = 0;
    float clipping_x2 = (float)m_width;
    float clipping_y2 = (float)m_height;
    for (auto& clipping : m_clipping) {
        float x1 = (float)clipping.origin.x * m_scale;
        float y1 = (float)m_height - (float)(clipping.origin.y + clipping.size.height) * m_scale;
        float x2 = (float)(clipping.origin.x + clipping.size.width) * m_scale;
        float y2 = (float)m_height - (float)clipping.origin.y * m_scale;
        if (clipping_x1 < x1) clipping_x1 = x1;
        if (clipping_y1 < y1) clipping_y1 = y1;
        if (clipping_x2 > x2) clipping_x2 = x2;
        if (clipping_y2 > y2) clipping_y2 = y2;
    }
    m_uniforms.clipping.x = (GLint)clipping_x1;
    m_uniforms.clipping.y = (GLint)clipping_y1;
    m_uniforms.clipping.w = (GLsizei)(clipping_x2 - clipping_x1 +1);
    m_uniforms.clipping.h = (GLsizei)(clipping_y2 - clipping_y1 + 1);
}
// ----------------------------------------------------------------------------
void Draw2D::target(Texture* tex) {
    if (tex) {
        if (m_render_target) m_fbo.unbind();
        m_fbo.bind(*tex);
        m_render_target = tex;
    } else {
        if (m_render_target) {
            m_fbo.unbind();
            m_render_target = nullptr;
        }
    }
}
// ----------------------------------------------------------------------------
