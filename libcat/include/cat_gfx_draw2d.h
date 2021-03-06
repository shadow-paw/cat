#ifndef __CAT_GFX_DRAW2D_H__
#define __CAT_GFX_DRAW2D_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "cat_platform.h"
#include "cat_data_copyable.h"
#include "cat_type.h"
#include "cat_gfx_type.h"
#include "cat_gfx_shader.h"
#include "cat_gfx_vbo.h"
#include "cat_gfx_fbo.h"
#include "cat_gfx_tex.h"
#include "cat_gfx_texref.h"
#include "cat_gfx_drawable.h"

namespace cat {
class Renderer;
class ResourceManager;
// ----------------------------------------------------------------------------
class Draw2D : private NonCopyable<Draw2D> {
friend class Renderer;
public:
    enum Effect {
        Color,
        Tex,
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
    void fill(const Rect2i& rect, uint32_t color, const Texture* tex, unsigned long t, Effect effect = Effect::Tex);
    void fill(const Rect2i& rect, uint32_t color, const TextureRef& texref, unsigned long t, Effect effect = Effect::Tex);
    void fill(const Rect2i& rect, uint32_t color, const Texture* tex, unsigned long t, const Shader* shader);
    void fill(const Rect2i& rect, uint32_t color, const TextureRef& texref, unsigned long t, const Shader* shader);
    // Text support
    void calctext(Size2i* size, const std::string& utf8, const TextStyle& style);
    void drawtext(const Rect2i& rect, const std::string& utf8, const TextStyle& style, float opacity = 1.0f);
    // clipping
    void clipping_push(const Rect2i& rect);
    void clipping_pop();
    // render to texture
    void target(Texture* tex);
    // Load compatible shader
    const Shader* retain_2dshader(ResourceManager* res, const std::string& name);
    void          release_2dshader(ResourceManager* res, const Shader* shader);

private:
    Draw2D(Renderer* r);
    ~Draw2D();
    bool init();
    void fini();
    void resize(int width, int height);
    bool render_text();
    void clipping_update();

private:
    Shader* m_shaders[Effect::Thermo + 1];
    VBO     m_vbo;
    int		m_width, m_height, m_scaled_height;
    float   m_scale;
    std::vector<Rect2i> m_clipping;
    // render target
    FBO      m_fbo;
    Texture* m_render_target;

    // uniform values
    struct {
        glm::vec2 center_multiplier;
        struct {
            bool    enabled;
            GLint   x, y;
            GLsizei w, h;
        } clipping;
    } m_uniforms;

private:
    // Builtin Shaders
    enum {
        u_CenterMultiplier,
        u_Tex0,
        u_Time,
    };
    enum {
        in_Position,
        in_Color,
        in_Texcoord,
    };
    static std::unordered_map<int, std::string> s_uniforms, s_attrs;
    static const char* m_shader_color;
    static const char* m_shader_tex;
    static const char* m_shader_blur;
    static const char* m_shader_ripple;
    static const char* m_shader_fisheye;
    static const char* m_shader_gray;
    static const char* m_shader_dream;
    static const char* m_shader_thermo;

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

    float convert_y(int h) const;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_GFX_DRAW2D_H__
