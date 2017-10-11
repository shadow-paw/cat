#include "osal_gfx_draw2d.h"
#include "osal_gfx_shader.h"
#include "osal_gfx_resmgr.h"
#include "osal_util_log.h"

using namespace osal;

// Fix android
#if defined(PLATFORM_ANDROID)
  #define GRAPHITE_DRAWTEXT_FLIPPED
#endif

std::unordered_map<int, std::string> Draw2D::s_uniforms = {
    { Draw2D::u_ScreenHalf, "uScreenHalf" },
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
    m_shader_col = nullptr;
    for (int i = 0; i<sizeof(m_shaders)/sizeof(m_shaders[0]); i++) {
        m_shaders[i] = nullptr;
    }
    m_width = m_height = m_scaled_height = 1;
    m_scale = 1.0f;
    // Text
    m_textcursor.plane = 0;
    m_textcursor.x = m_textcursor.y = 0;
    m_textcursor.height = 0;
}
// ----------------------------------------------------------------------------
Draw2D::~Draw2D() {
    fini();
}
// ----------------------------------------------------------------------------
bool Draw2D::init() {
    struct {
        const char *vs, *fs;
    } shaders[sizeof(m_shaders)/sizeof(m_shaders[0])];
    shaders[Effect::None]    = { m_shader_tex_v,     m_shader_tex_f };
    shaders[Effect::Gray]    = { m_shader_gray_v,    m_shader_gray_f };
    shaders[Effect::Blur]    = { m_shader_blur_v,    m_shader_blur_f };
    shaders[Effect::Ripple]  = { m_shader_ripple_v,  m_shader_ripple_f };
    shaders[Effect::Fisheye] = { m_shader_fisheye_v, m_shader_fisheye_f };
    shaders[Effect::Dream]   = { m_shader_dream_v,   m_shader_dream_f };
    shaders[Effect::Thermo]  = { m_shader_thermo_v,  m_shader_thermo_f };
    if (!m_vbo.init(54 * sizeof(Vertex2f), true)) goto fail;
    // Plain Color shader
    m_shader_col = new Shader();
    if (!m_shader_col) goto fail;
    if (!m_shader_col->init(m_shader_col_v, m_shader_col_f)) goto fail;
    m_shader_col->bind();
    for (auto& kv: s_uniforms) {
        m_shader_col->bind_uniform(kv.first, kv.second);
    }
    for (auto& kv : s_attrs) {
        m_shader_col->bind_attr(kv.first, kv.second);
    }
    m_shader_col->unbind();
    // Texture Shaders
    for (int i = 0; i<sizeof(shaders)/sizeof(shaders[0]); i++) {
        m_shaders[i] = new Shader();
        if (!m_shaders[i]) goto fail;
        if (!m_shaders[i]->init(shaders[i].vs, shaders[i].fs)) {
            Logger::e("gfx", "Failed to compile builtin shader %d", i);
            delete m_shaders[i];
            m_shaders[i] = nullptr;
            continue;
        }
        m_shaders[i]->bind();
        for (auto& kv : s_uniforms) {
            m_shaders[i]->bind_uniform(kv.first, kv.second);
        }
        for (auto& kv : s_attrs) {
            m_shaders[i]->bind_attr(kv.first, kv.second);
        }
        m_shaders[i]->unbind();
    }
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
    if (m_shader_col) { delete m_shader_col; m_shader_col = nullptr; }
    for (int i = 0; i<sizeof(m_shaders) / sizeof(m_shaders[0]); i++) {
        if (m_shaders[i]) { delete m_shaders[i]; m_shaders[i] = nullptr; }
    }
    m_vbo.fini();
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
}
// ----------------------------------------------------------------------------
void Draw2D::scale(float factor) {
    if (m_scale == factor) return;
    m_scale = factor;
    m_scaled_height = (int)(m_height / m_scale);
}
// ----------------------------------------------------------------------------
void Draw2D::drawline(int x1, int y1, int x2, int y2, uint32_t color) {
    float x[2], y[2];
    x[0] = (float)x1;
    x[1] = (float)x2;
    y[0] = (float)(m_scaled_height - y1);
    y[1] = (float)(m_scaled_height - y2);
    Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
    vertex[0].x = x[0]; vertex[0].y = y[0]; vertex[0].color = color;
    vertex[1].x = x[1]; vertex[1].y = y[1]; vertex[1].color = color;
    m_vbo.unlock();

    m_shader_col->bind();
    m_vbo.bind();
    m_shader_col->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, x));
    m_shader_col->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, color));
    m_shader_col->draw(Shader::kLine, 0, 2);
    m_shader_col->clr_attr(in_Color);
    m_shader_col->clr_attr(in_Position);
    m_vbo.unbind();
    m_shader_col->unbind();
}
// ----------------------------------------------------------------------------
void Draw2D::outline(const Rect2i& rect, uint32_t color) {
    float x[2], y[2];
    x[0] = (float)rect.origin.x;
    x[1] = (float)(rect.origin.x+ rect.size.width+1);
    y[0] = (float)(m_scaled_height - rect.origin.y);
    y[1] = (float)(m_scaled_height - (rect.origin.y+ rect.size.height+1));
    Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
    vertex[0].x = x[0]; vertex[0].y = y[0]; vertex[0].color = color;
    vertex[1].x = x[0]; vertex[1].y = y[1]; vertex[1].color = color;
    vertex[2].x = x[1]; vertex[2].y = y[1]; vertex[2].color = color;
    vertex[3].x = x[1]; vertex[3].y = y[0]; vertex[3].color = color;
    m_vbo.unlock();

    m_shader_col->bind();
    m_vbo.bind();
    m_shader_col->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, x));
    m_shader_col->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, color));
    m_shader_col->draw(Shader::kLineLoop, 0, 4);
    m_shader_col->clr_attr(in_Color);
    m_shader_col->clr_attr(in_Position);
    m_vbo.unbind();
    m_shader_col->unbind();
}
// ----------------------------------------------------------------------------
void Draw2D::fill(const Rect2i& rect, uint32_t color) {
    float x[2], y[2];
    x[0] = (float)rect.origin.x;
    x[1] = (float)(rect.origin.x + rect.size.width+1);
    y[0] = (float)(m_scaled_height - rect.origin.y);
    y[1] = (float)(m_scaled_height - (rect.origin.y + rect.size.height+1));
    Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
    vertex[0].x = x[0]; vertex[0].y = y[0]; vertex[0].color = color;
    vertex[1].x = x[0]; vertex[1].y = y[1]; vertex[1].color = color;
    vertex[2].x = x[1]; vertex[2].y = y[1]; vertex[2].color = color;
    vertex[3].x = x[1]; vertex[3].y = y[0]; vertex[3].color = color;
    m_vbo.unlock();

    m_shader_col->bind();
    m_vbo.bind();
    m_shader_col->uniform(u_ScreenHalf, (float)m_width / 2 / m_scale, (float)m_height / 2 / m_scale);
    m_shader_col->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, x));
    m_shader_col->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, color));
    m_shader_col->draw(Shader::kTriangleFan, 0, 4);
    m_shader_col->clr_attr(in_Color);
    m_shader_col->clr_attr(in_Position);
    m_vbo.unbind();
    m_shader_col->unbind();
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
        y[0] = (float)(m_scaled_height - rect.origin.y);
        y[1] = (float)(m_scaled_height - (rect.origin.y+rect.size.height+1));

        Vertex2f* vertex = (Vertex2f*)m_vbo.lock();
        vertex[0] = { x[0], y[0], 0.0f, 1.0f, color };
        vertex[1] = { x[0], y[1], 0.0f, 0.0f, color };
        vertex[2] = { x[1], y[0], 1.0f, 1.0f, color };
        vertex[3] = { x[1], y[1], 1.0f, 0.0f, color };
        m_vbo.unlock();

        shader->bind();
        m_vbo.bind();
        tex->bind(0);
        shader->uniform(u_ScreenHalf, (float)m_width / 2 / m_scale, (float)m_height / 2 / m_scale);
        shader->uniform(u_Tex0, 0);
        shader->uniform(u_Time, (float)(t & 0xffffff)/1000);
        shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, x));
        shader->set_attr(in_Texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, u));
        shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, color));
        shader->draw(Shader::kTriangleStrip, 0, 4);
        shader->clr_attr(in_Color);
        shader->clr_attr(in_Texcoord);
        shader->clr_attr(in_Position);
        tex->unbind(0);
        m_vbo.unbind();
        shader->unbind();
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
            y[0] = (float)(m_scaled_height - rect.origin.y);
            y[1] = (float)(m_scaled_height - (rect.origin.y+rect.size.height+1));
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

            shader->bind();
            m_vbo.bind();
            texref.tex->bind(0);
            shader->uniform(u_ScreenHalf, (float)m_width / 2 / m_scale, (float)m_height / 2 / m_scale);
            shader->uniform(u_Tex0, 0);
            shader->uniform(u_Time, (float)(t & 0xffffff)/1000);
            shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, x));
            shader->set_attr(in_Texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, u));
            shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, color));
            shader->draw(Shader::kTriangleStrip, 0, 4);
            shader->clr_attr(in_Color);
            shader->clr_attr(in_Texcoord);
            shader->clr_attr(in_Position);
            texref.tex->unbind(0);
            m_vbo.unbind();
            shader->unbind();
        } else {
            // TOP-LEFT
            const float tex_w = (float)texref.tex->width();
            const float tex_h = (float)texref.tex->height();
            float x[4], y[4], u[4], v[4];

            x[0] = (float)(rect.origin.x);
            x[1] = (float)(rect.origin.x + texref.border_u);
            x[2] = (float)(rect.origin.x + rect.size.width+1 - texref.border_u);
            x[3] = (float)(rect.origin.x + rect.size.width+1);
            y[0] = (float)(m_scaled_height - rect.origin.y);
            y[1] = (float)(m_scaled_height - (rect.origin.y + texref.border_v));
            y[2] = (float)(m_scaled_height - (rect.origin.y + rect.size.height+1 - texref.border_v));
            y[3] = (float)(m_scaled_height - (rect.origin.y + rect.size.height+1));
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

            shader->bind();
            m_vbo.bind();
            texref.tex->bind(0);
            shader->uniform(u_ScreenHalf, (float)m_width / 2 / m_scale, (float)m_height / 2 / m_scale);
            shader->uniform(u_Tex0, 0);
            shader->uniform(u_Time, (float)(t&0xffffff)/1000);
            shader->set_attr(in_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, x));
            shader->set_attr(in_Texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, u));
            shader->set_attr(in_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2f), OSAL_GFX_OFFSETOF(Vertex2f, color));
            shader->draw(Shader::kTriangles, 0, 54);
            shader->clr_attr(in_Color);
            shader->clr_attr(in_Texcoord);
            shader->clr_attr(in_Position);
            texref.tex->unbind(0);
            m_vbo.unbind();
            shader->unbind();
        }
    } else {
        fill(rect, color);
    }
}
// ----------------------------------------------------------------------------
// Text Functions
// ----------------------------------------------------------------------------
bool Draw2D::render_text() {
    // remove expired entry
    for (auto it = m_texts.begin(); it != m_texts.end();) {
        if (it->second.expired) {
            it = m_texts.erase(it);
        } else {
            ++ it;
        }
    }
    for (auto it = m_texts.begin(); it != m_texts.end(); ++it) {
        const std::string& text = it->first;
        TextEntry& entry = it->second;
        if (!entry.ready && !entry.expired) {
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
        }
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
void Draw2D::drawtext(const Rect2i& rect, const std::string& utf8, const TextStyle& style) {
    auto range = m_texts.equal_range(utf8);
    for (auto it = range.first; it != range.second; ++it) {
        const TextEntry& entry = it->second;
        if (entry.style.appearance!=style.appearance || entry.style.fontsize!=style.fontsize) continue;

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
            fill(rc, 0xffffffff, entry.texref, 0, Effect::None);
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
const Shader* Draw2D::retain_2dshader(ResourceManager* res, const char* name) {
    return res->retain_shader(name, s_uniforms, s_attrs);
}
// ----------------------------------------------------------------------------
void Draw2D::release_2dshader(ResourceManager* res, const Shader* shader) {
    res->release_shader(shader);
}
// ----------------------------------------------------------------------------
