#ifndef __OSAL_GFX_DRAW2D_H__
#define __OSAL_GFX_DRAW2D_H__

#include <string>
#include <unordered_map>
#include "osal_platform.h"
#include "osal_type.h"
#include "osal_gfx_type.h"
#include "osal_gfx_shader.h"
#include "osal_gfx_vbo.h"
#include "osal_gfx_tex.h"
#include "osal_gfx_texref.h"
#include "osal_gfx_drawable.h"

namespace osal {
class Renderer;
class ResourceManager;
// ----------------------------------------------------------------------------
class Draw2D {
friend class Renderer;
public:
    enum Effect {
        None,
        Gray,
        Blur,
        Ripple,
        Fisheye,
        Dream,
        Thermo
    };
    void scale(float factor);

    void drawline(int x1, int y1, int x2, int y2, uint32_t color);
    void outline(const Rect2i& rect, uint32_t color);
    void fill(const Rect2i& rect, uint32_t color);
    void fill(const Rect2i& rect, uint32_t color, const Texture* tex, unsigned long t, Effect effect = Effect::None);
    void fill(const Rect2i& rect, uint32_t color, const TextureRef& texref, unsigned long t, Effect effect = Effect::None);
    void fill(const Rect2i& rect, uint32_t color, const Texture* tex, unsigned long t, const Shader* shader);
    void fill(const Rect2i& rect, uint32_t color, const TextureRef& texref, unsigned long t, const Shader* shader);
    // Text support
    void calctext(Size2i* size, const std::string& utf8, const TextStyle& style);
    void drawtext(const Rect2i& rect, const std::string& utf8, const TextStyle& style);

    // Load compatible shader
    const Shader* retain_2dshader(ResourceManager* res, const char* name);
    void          release_2dshader(ResourceManager* res, const Shader* shader);

private:
    Draw2D();
    ~Draw2D();
    bool init();
    void fini();
    void resize(int width, int height);
    bool render_text();

private:
    Shader* m_shader_col;
    Shader* m_shaders[Effect::Thermo + 1];
    VBO     m_vbo;
    int		m_width, m_height, m_scaled_height;
    float   m_scale;

private:
    // Builtin Shaders
    enum {
        u_ScreenHalf,
        u_Tex0,
        u_Time,
    };
    enum {
        in_Position,
        in_Color,
        in_Texcoord,
    };
    static std::unordered_map<int, std::string> s_uniforms, s_attrs;
    static const char* m_shader_col_v;
    static const char* m_shader_col_f;
    static const char* m_shader_tex_v;
    static const char* m_shader_tex_f;
    static const char* m_shader_blur_v;
    static const char* m_shader_blur_f;
    static const char* m_shader_ripple_v;
    static const char* m_shader_ripple_f;
    static const char* m_shader_fisheye_v;
    static const char* m_shader_fisheye_f;
    static const char* m_shader_gray_v;
    static const char* m_shader_gray_f;
    static const char* m_shader_dream_v;
    static const char* m_shader_dream_f;
    static const char* m_shader_thermo_v;
    static const char* m_shader_thermo_f;

private: 
    // Draw Text
    static const int kTextTextureWidth = 1024;
    static const int kTextTextureHeight = 512;
    static const int kTextTextureMax = 8;
    struct TextEntry {
        // std::string text;
        TextStyle   style;
        bool        ready, expired;
        int         tex_plane;
        TextureRef  texref;
        Size2i      size;
    };

    Renderer* m_renderer;
    std::unordered_multimap<std::string, TextEntry> m_texts;
    Drawable m_drawable[kTextTextureMax];
    struct {
        int plane, x, y, height;
    } m_textcursor;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_GFX_DRAW2D_H__
