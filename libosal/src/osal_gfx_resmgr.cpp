#include "osal_gfx_resmgr.h"
#include "libpng/png.h"
#include <string.h>

using namespace osal;

// ----------------------------------------------------------------------------
ResourceManager::ResourceManager(VFS* vfs) {
    m_contextready = false;
    m_vfs = vfs;
}
// ----------------------------------------------------------------------------
ResourceManager::~ResourceManager() {
    fini();
}
// ----------------------------------------------------------------------------
bool ResourceManager::init() {
    return true;
}
// ----------------------------------------------------------------------------
void ResourceManager::fini() {
    context_lost();
    m_shaders.clear();
    m_texs.clear();
}
// ----------------------------------------------------------------------------
void ResourceManager::context_lost() {
    if (!m_contextready) return;
    m_contextready = false;
    // release all shaders
    for (auto& it : m_shaders) {
        it.second.first.shader->fini();
    }
    // release all tex
    for (auto& it : m_texs) {
        it.second.first->release();
    }
}
// ----------------------------------------------------------------------------
bool ResourceManager::context_restored() {
    if (m_contextready) return true;
    m_contextready = true;
    // restore all shaders
    for (auto it: m_shaders) {
        reload_shader(&it.second.first, it.first);
    }
    // reload all tex
    for (auto it : m_texs) {
        reload_tex(it.second.first, it.first);
    }
    return true;
}
// ----------------------------------------------------------------------------
const Shader* ResourceManager::retain_shader(const char* name, const std::unordered_map<int, std::string>& uniforms, const std::unordered_map<int, std::string>& attrs) {
    std::string path(name);
    Buffer vs, fs;
    auto cached = m_shaders.find(path);
    if (cached!=m_shaders.end()) {
        auto& pair = cached->second;
        pair.second ++;
        return pair.first.shader;
    }
    Shader* shader = new Shader();
    if (m_contextready) {
        if (!m_vfs->read((path + ".vs").c_str(), vs)) return nullptr;
        if (!m_vfs->read((path + ".fs").c_str(), fs)) return nullptr;
        if (!shader->init((const char*)vs.data(), (const char*)fs.data())) {
            delete shader;
            return nullptr;
        }
        // set uniforms and attrs
        shader->bind();
        for (auto it : uniforms) {
            shader->bind_uniform(it.first, it.second.c_str());
        }
        for (auto it : attrs) {
            shader->bind_attr(it.first, it.second.c_str());
        }
        shader->unbind();
    }
    m_shaders.emplace(name, std::make_pair(SHADER_DATA{ shader, uniforms, attrs }, 1));
    return shader;
}
// ----------------------------------------------------------------------------
bool ResourceManager::reload_shader(SHADER_DATA* sd, const std::string& name) {
    std::string path(name);
    Buffer vs, fs;
    if (!m_vfs->read((path + ".vs").c_str(), vs)) return false;
    if (!m_vfs->read((path + ".fs").c_str(), fs)) return false;
    if (!sd->shader->init((const char*)vs.data(), (const char*)fs.data())) return false;
    // set uniforms and attrs
    sd->shader->bind();
    for (auto it : sd->uniforms) {
        sd->shader->bind_uniform(it.first, it.second.c_str());
    }
    for (auto it : sd->attrs) {
        sd->shader->bind_attr(it.first, it.second.c_str());
    }
    sd->shader->unbind();
    return true;
}
// ----------------------------------------------------------------------------
bool ResourceManager::release_shader(const char *name) {
    auto cached = m_shaders.find(name);
    if (cached == m_shaders.end()) return false;
    auto& pair = cached->second;
    pair.second --;
    if (pair.second == 0) {
        delete pair.first.shader;
        m_shaders.erase(name);
    } return true;
}
// ----------------------------------------------------------------------------
bool ResourceManager::release_shader(const Shader* shader) {
    if (shader == nullptr ) return false;
    for (auto it=m_shaders.begin(); it!=m_shaders.end(); ++it) {
        auto& pair = it->second;
        if (pair.first.shader != shader) continue;
        pair.second --;
        if (pair.second == 0) {
            delete pair.first.shader;
            m_shaders.erase(it);
        } return true;
    }
    return false;
}
// ----------------------------------------------------------------------------
const Texture* ResourceManager::retain_tex(const char* name) {
    Buffer buf;
    auto cached = m_texs.find(name);
    if (cached!=m_texs.end()) {
        auto& pair = cached->second;
        pair.second ++;
        return pair.first;
    }
    // TODO: check filename extension and also handle jpeg
    Texture* tex = new Texture();
    if (m_contextready) {
        if (!m_vfs->read(name, buf)) return nullptr;
        if (!load_tex_png(tex, buf)) {
            delete tex;
            return nullptr;
        }
    }
    m_texs.emplace(name, std::make_pair(tex, 1));
    return tex;
}
// ----------------------------------------------------------------------------
bool ResourceManager::reload_tex(Texture* tex, const std::string& name) {
    Buffer buf;
    if (!m_vfs->read(name.c_str(), buf)) return false;
    return load_tex_png(tex, buf);
}
// ----------------------------------------------------------------------------
bool ResourceManager::release_tex(const char *name) {
    auto cached = m_texs.find(name);
    if (cached == m_texs.end()) return false;
    auto& pair = cached->second;
    pair.second --;
    if (pair.second == 0) {
        delete pair.first;
        m_texs.erase(name);
    } return true;
}
// ----------------------------------------------------------------------------
bool ResourceManager::release_tex(const Texture* tex) {
    if (tex == nullptr ) return false;
    for (auto it=m_texs.begin(); it!=m_texs.end(); ++it) {
        auto& pair = it->second;
        if (pair.first != tex) continue;
        pair.second --;
        if (pair.second == 0) {
            delete pair.first;
            m_texs.erase(it);
        } return true;
    }
    return false;
}
// ----------------------------------------------------------------------------
// PNG Helper
// ----------------------------------------------------------------------------
typedef struct {
    Buffer* buffer;
    size_t offset;
} PNGBufferReader;
// ----------------------------------------------------------------------------
void _png_read(png_structp png, png_bytep data, png_size_t size) {
    PNGBufferReader* br = (PNGBufferReader*) png_get_io_ptr(png);
    if ( br->offset + size <= br->buffer->size() ) {
        memcpy ( data, br->buffer->data() + br->offset, size );
        br->offset += size;
    }
}
// ----------------------------------------------------------------------------
bool ResourceManager::load_tex_png(Texture* tex, Buffer& buf) {
    // image
    Texture::Format format;
    unsigned int width, height;
    uint8_t* pixel = nullptr;
    // png magic
    png_structp png = NULL;
    png_infop info  = NULL;
    png_bytepp row_pointers;
    size_t row_bytes;
    PNGBufferReader bufreader;

    if (png_sig_cmp(buf.data(), 0, 8)) goto fail;
    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL) goto fail;
    info = png_create_info_struct(png);
    if (info == NULL) goto fail;
    if (setjmp(png_jmpbuf(png))) goto fail;

    bufreader.buffer = &buf;
    bufreader.offset = 8;
    png_set_read_fn(png, &bufreader, _png_read);

    png_set_sig_bytes(png, 8);
    png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    switch ( png_get_color_type(png, info) ) {
        case PNG_COLOR_TYPE_RGBA: format = Texture::Format::RGBA;  break;
        case PNG_COLOR_TYPE_RGB:  format = Texture::Format::RGB;   break;
        default: goto fail;
    }
    //    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    row_bytes = png_get_rowbytes(png, info);
    width  = (unsigned int)png_get_image_width(png, info);
    height = (unsigned int)png_get_image_height(png, info);
    if (width > 4096 || height > 4096) goto fail;   // saneity check!
    pixel = new uint8_t[row_bytes * height];

    row_pointers = png_get_rows(png, info);
    for (unsigned int i = 0; i < height; i++) {
        memcpy( pixel + (row_bytes * (height-1-i)), row_pointers[i], row_bytes );
    }
    png_destroy_read_struct(&png, &info, NULL);

    tex->update(format, (int)width, (int)height, pixel);
    delete pixel;
    return true;
fail:
    png_destroy_read_struct(&png, &info, NULL);
    delete pixel;
    return false;
}
// ----------------------------------------------------------------------------

