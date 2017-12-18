#include "cat_gfx_shader.h"
#include <string.h>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "cat_util_log.h"
#include "cat_util_string.h"

using namespace cat;

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
GLuint Shader::compile(GLenum type, const std::string& code) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) return 0;
    GLint compiled = 0;
    std::string pp = preprocessor(type, code);
    const char* sources[] = { pp.c_str() };
    glShaderSource(shader, 1, sources, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char buf[512];
        glGetShaderInfoLog(shader, sizeof(buf), NULL, buf);
        Logger::e("shader", "Could not compile shader %d:\n%s\n%s\n", type, buf, pp.c_str());
        glDeleteShader(shader);
        shader = 0;
    } return shader;
}
// ----------------------------------------------------------------------------
std::string Shader::preprocessor(GLenum type, const std::string& code) const {
    const char* defines = nullptr;
    switch (type) {
    case GL_VERTEX_SHADER:   defines = "#define VERTEX_SHADER\n"; break;
    case GL_FRAGMENT_SHADER: defines = "#define FRAGMENT_SHADER\n"; break;
    }
    const char version_tag[] = "#version ";
    const size_t version_len = sizeof(version_tag) -1;
    std::string result;
    std::istringstream is(code);
    for (std::string line; std::getline(is, line);) {
        auto trimmed = StringUtil::trim(line);
        if (trimmed.empty()) continue;
        if (trimmed.compare(0, version_len, version_tag) == 0) {
            auto versions = StringUtil::split(trimmed.substr(version_len), ",");
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64) || defined(PLATFORM_MAC)
            if (versions.size() > 0) {
                result += version_tag + versions[0] + "\n";
            } else {
                result += line + "\n";
            }
#elif defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
            if (versions.size() > 1) {
                result += version_tag + versions[1] + "\n";
            } else if (versions.size() > 0) {
                result += version_tag + versions[0] + "\n";
            } else {
                result += line + "\n";
            }
#else
    #error Not Implemented!
#endif
            // inject defines
            if (defines) result += defines;
        } else {
            result += line + "\n";
        }
    }
    return result;
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
    if (name!=nullptr) {
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
    if (name!=nullptr) {
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
void Shader::uniform(unsigned int slot, const glm::vec2& v) const {
    if (m_attr[slot] == -1) return;
    glUniform2f(m_uniform[slot], v[0], v[1]);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::vec3& v) const {
    if (m_attr[slot] == -1) return;
    glUniform3f(m_uniform[slot], v[0], v[1], v[2]);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::vec4& v) const {
    if (m_attr[slot] == -1) return;
    glUniform4f(m_uniform[slot], v[0], v[1], v[2], v[3]);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::mat4& m) const {
    if (m_attr[slot]==-1) return;
    glUniformMatrix4fv (m_uniform[slot], 1, GL_FALSE, glm::value_ptr(m));
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, GLfloat* f, size_t count) const {
    if (m_attr[slot] == -1) return;
    glUniform1fv(m_uniform[slot], (GLsizei)count, f);
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::vec2* v, size_t count) const {
    if (m_attr[slot] == -1) return;
    glUniform2fv(m_uniform[slot], (GLsizei)count, glm::value_ptr(v[0]));
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::vec3* v, size_t count) const {
    if (m_attr[slot] == -1) return;
    glUniform3fv(m_uniform[slot], (GLsizei)count, glm::value_ptr(v[0]));
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::vec4* v, size_t count) const {
    if (m_attr[slot] == -1) return;
    glUniform4fv(m_uniform[slot], (GLsizei)count, glm::value_ptr(v[0]));
}
// ----------------------------------------------------------------------------
void Shader::uniform(unsigned int slot, const glm::mat4* m, size_t count) const {
    if (m_attr[slot] == -1) return;
    glUniformMatrix4fv(m_uniform[slot], (GLsizei)count, GL_FALSE, glm::value_ptr(m[0]));
}
// ----------------------------------------------------------------------------
