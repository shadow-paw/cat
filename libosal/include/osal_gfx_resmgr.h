#ifndef __OSAL_GFX_RESMGR_H__
#define __OSAL_GFX_RESMGR_H__

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <memory>
#include <list>
#include <functional>
#include <unordered_map>
#include "osal_storage_vfs.h"
#include "osal_gfx_shader.h"
#include "osal_gfx_tex.h"

namespace osal {
// ----------------------------------------------------------------------------
class ResourceManager {
public:
    ResourceManager(VFS* vfs);
    ~ResourceManager();

    bool init();
    void fini();
    void context_lost();
    bool context_restored();

    const Shader*  retain_shader(const char* name, const std::unordered_map<int,std::string>& uniforms, const std::unordered_map<int, std::string>& attrs);
    bool           release_shader(const char* name);
    bool           release_shader(const Shader* shader);
    const Texture* retain_tex(const char* name);
    bool           release_tex(const char* name);
    bool           release_tex(const Texture* tex);

private:
    struct SHADER_DATA {
        Shader* shader;
        std::unordered_map<int, std::string> uniforms, attrs;
    };
    bool reload_shader(SHADER_DATA* sd, const std::string& name);
    bool reload_tex   (Texture* tex, const std::string& name);

private:
    bool load_tex_png(Texture* tex, Buffer& buf);

private:
    bool m_contextready;
    VFS* m_vfs;
    // map { name, pair { res, refcount} }
    std::unordered_map<std::string, std::pair<SHADER_DATA, int>>  m_shaders;
    std::unordered_map<std::string, std::pair<Texture*, int>> m_texs;
};
// ----------------------------------------------------------------------------
} // namespace osal

#endif // __OSAL_GFX_RESMGR_H__
