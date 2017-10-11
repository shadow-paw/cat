#include "osal_gfx_shader.h"
#include "osal_util_log.h"
#include <string.h>
#include <glm/gtc/type_ptr.hpp>

using namespace osal;

// ----------------------------------------------------------------------------
Shader::Shader() {
    m_vs = 0;
    m_fs = 0;
    m_program = 0;
    memset(m_attr, 0, sizeof(m_attr));
    memset(m_uniform, 0, sizeof(m_uniform));
}
// ----------------------------------------------------------------------------
Shader::~Shader() {
    fini();
}
// ----------------------------------------------------------------------------
bool Shader::init(const char* vertex_shader, const char* fragment_shader) {
    memset (m_attr, 0, sizeof(m_attr));
    memset (m_uniform, 0, sizeof(m_uniform));
    GLint status = GL_FALSE;
    if (vertex_shader==nullptr || fragment_shader==nullptr) return false;
    m_vs = compile(GL_VERTEX_SHADER, vertex_shader);
    m_fs = compile(GL_FRAGMENT_SHADER, fragment_shader);
    if (m_vs == 0 || m_fs == 0) goto fail;
    m_program = glCreateProgram();
    if (m_program==0) goto fail;
    glAttachShader(m_program, m_vs);
    glAttachShader(m_program, m_fs);
    glLinkProgram (m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    if (status!=GL_TRUE) {
        char buf[512];
        glGetProgramInfoLog(m_program, sizeof(buf), NULL, buf);
        Logger::e("shader", "Could not link shader:\n%s\n", buf);
        goto fail;
    }
    return true;
fail:
    fini();
    return false;
}
// ----------------------------------------------------------------------------
void Shader::fini() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
    if (m_vs) { glDeleteShader(m_vs); m_vs = 0; }
    if (m_fs) { glDeleteShader(m_fs); m_fs = 0; }
    memset (m_attr, 0, sizeof(m_attr));
    memset (m_uniform, 0, sizeof(m_uniform));
}
// ----------------------------------------------------------------------------
GLuint Shader::compile(GLenum type, const char* code) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) return 0;
    GLint compiled = 0;
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char buf[512];
        glGetShaderInfoLog(shader, sizeof(buf), NULL, buf);
        Logger::e("shader", "Could not compile shader %d:\n%s\n", type, buf);
        glDeleteShader(shader);
        shader = 0;
    } return shader;
}
// ----------------------------------------------------------------------------
void Shader::bind() const {
    glUseProgram(m_program);
}
// ----------------------------------------------------------------------------
void Shader::unbind() const {
    glUseProgram(0);
}
// ----------------------------------------------------------------------------
void Shader::draw(GLint mode, int start, int count) const {
    glDrawArrays(mode, start, count);
}
// ----------------------------------------------------------------------------
void Shader::bind_attr(unsigned int slot, const char* name) {
    if ( name!=nullptr ) {
        m_attr[slot] = glGetAttribLocation(m_program, name);
        if (m_attr[slot]==GL_INVALID_OPERATION) m_attr[slot] = -1;
    } else m_attr[slot] = -1;
}
// ----------------------------------------------------------------------------
void Shader::bind_attr(unsigned int slot, const std::string& name) {
    bind_attr(slot, name.c_str());
}
// ----------------------------------------------------------------------------
void Shader::set_attr(unsigned int slot, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer) const {
    if (m_attr[slot]==-1) return;
    glVertexAttribPointer((GLuint)m_attr[slot], size, type, normalized, stride, pointer);
    glEnableVertexAttribArray((GLuint)m_attr[slot]);
}
// ----------------------------------------------------------------------------
void Shader::clr_attr(unsigned int slot) const {
    if (m_attr[slot]==-1) return;
    glDisableVertexAttribArray((GLuint)m_attr[slot]);
}
// ----------------------------------------------------------------------------
void Shader::bind_uniform(unsigned int slot, const char* name) {
    if ( name!=nullptr ) {
        m_uniform[slot] = glGetUniformLocation(m_program, name );
        if (m_uniform[slot]==GL_INVALID_OPERATION) m_attr[slot] = -1;
    } else m_uniform[slot] = -1;
}
// ----------------------------------------------------------------------------
void Shader::bind_uniform(unsigned int slot, const std::string& name) {
    bind_uniform(slot, name.c_str());
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLfloat f0) const {
    if (m_attr[slot]==-1) return;
    glUniform1f(m_uniform[slot], f0);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLfloat f0, GLfloat f1) const {
    if (m_attr[slot]==-1) return;
    glUniform2f(m_uniform[slot], f0, f1);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLfloat f0, GLfloat f1, GLfloat f2) const {
    if (m_attr[slot]==-1) return;
    glUniform3f(m_uniform[slot], f0, f1, f2);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLfloat f0, GLfloat f1, GLfloat f2, GLfloat f3) const {
    if (m_attr[slot]==-1) return;
    glUniform4f(m_uniform[slot], f0, f1, f2, f3);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLint i0) const {
    if (m_attr[slot]==-1) return;
    glUniform1i(m_uniform[slot], i0);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLint i0, GLint i1) const {
    if (m_attr[slot]==-1) return;
    glUniform2i(m_uniform[slot], i0, i1);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLint i0, GLint i1, GLint i2) const {
    if (m_attr[slot]==-1) return;
    glUniform3i(m_uniform[slot], i0, i1, i2);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLint i0, GLint i1, GLint i2, GLint i3) const {
    if (m_attr[slot]==-1) return;
    glUniform4i(m_uniform[slot], i0, i1, i2, i3);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::mat4& m) const {
    if (m_attr[slot]==-1) return;
    glUniformMatrix4fv (m_uniform[slot], 1, GL_FALSE, glm::value_ptr(m));
}
// ----------------------------------------------------------------------------
