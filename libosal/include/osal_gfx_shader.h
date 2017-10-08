#ifndef __OSAL_GFX_SHADER_H__
#define __OSAL_GFX_SHADER_H__

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <glm/glm.hpp>
#include "osal_platform.h"

namespace osal { namespace gfx {
// ----------------------------------------------------------------------------
#define OSAL_GFX_OFFSETOF(struc,member) ((void*)((size_t) &((struc*)0)->member))
class Shader {
public:
    enum {
        kTriangles = GL_TRIANGLES,
        kTriangleStrip = GL_TRIANGLE_STRIP,
        kTriangleFan = GL_TRIANGLE_FAN,
        kLineLoop = GL_LINE_LOOP,
        kLine = GL_LINE_STRIP,
    };

    Shader();
    ~Shader();
    bool init(const char* vertex_shader, const char* fragment_shader);
    void fini();
    void bind() const;
    void unbind() const;

    void draw(GLint mode, int start, int count) const;

    void bind_attr(unsigned int slot, const char* name);
    void bind_attr(unsigned int slot, const std::string& name);
    void set_attr(unsigned int slot, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer) const;
    void clr_attr(unsigned int slot) const;

    void bind_uniform(unsigned int slot, const char* name);
    void bind_uniform(unsigned int slot, const std::string& name);
    void uniform(unsigned int slot, GLfloat f0) const;
    void uniform(unsigned int slot, GLfloat f0, GLfloat f1) const;
    void uniform(unsigned int slot, GLfloat f0, GLfloat f1, GLfloat f2) const;
    void uniform(unsigned int slot, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3) const;
    void uniform(unsigned int slot, GLint i0) const;
    void uniform(unsigned int slot, GLint i0, GLint i1) const;
    void uniform(unsigned int slot, GLint i0, GLint i1, GLint i2) const;
    void uniform(unsigned int slot, GLint i0, GLint i1, GLint i2, GLint i3) const;
    void uniform(unsigned int slot, const glm::mat4& m) const;
private:
    GLuint compile(GLenum type, const char* code);
private:
    GLuint m_program, m_vs, m_fs;
    GLint  m_attr[64];
    GLint  m_uniform[64];
};
// ----------------------------------------------------------------------------
}} // namespace osal::gfx

#endif // __OSAL_GFX_SHADER_H__
