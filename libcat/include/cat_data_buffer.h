#ifndef __CAT_DATA_BUFFER_H__
#define __CAT_DATA_BUFFER_H__

#include <stdint.h>
#include <stddef.h>
#include <string>

namespace cat {
// ----------------------------------------------------------------------------
//! Convenient Buffer
class Buffer {
public:
    //! Initialize with empty buffer
    Buffer();
    //! Copy buffer
    Buffer(const Buffer& o);
    //! Move buffer
    Buffer(Buffer&& o);
    //! Initialize by copying the memory region
    Buffer(const void* p, size_t size);
    //! Destroy and release buffer
    ~Buffer();

    //! Access the memory
    //! \return point to memory, or nullptr if not allocated
    uint8_t* ptr() const { return m_buffer; }
    //! Get size of the buffer
    //! \return length, in bytes, of the buffer
    size_t   size() const { return m_size; }
    //! Default cast to uint8_t*, i.e. uint8_t* p = buffer;
    operator uint8_t*() { return m_buffer; }
    operator const uint8_t*() const { return m_buffer; }

    //! Resize buffer
    //! The behaviour is as if calling C realloc()
    //! \param size New size of the buffer, in bytes
    //! \return true if success, false if failed and no side-effect
    bool realloc(size_t size);
    //! Free the underlying memory
    //! Buffer will reset to empty state
    void free();
    //! Reduce size of the buffer
    //! \return true if success, false if failed and no side-effect
    bool shrink(size_t s);
    //! Make this buffer point to a copy of the memory region
    //! If the buffer contain any data it will be free'd.
    //! \param mem Memory to copy
    //! \param size Size of the memory region, in bytes
    //! \return true if success, false if failed and no side-effect
    bool assign(const void* mem, size_t size);
    //! Copy memory region to this buffer
    //! i.e. memcpy(this->ptr()[offset], mem, size);
    //! \param offset Offset of this buffer
    //! \param mem Memory to copy
    //! \param size Size of the memory region, in bytes
    //! \return true if success, false if failed and no side-effect, eg. buffer is too small.
    bool copy(size_t offset, const void* mem, size_t size);

    //! Move Assignment
    //! \param o Buffer to move
    //! \return Buffer acquired the input buffer
    Buffer& operator=(Buffer&& o);
    //! Make this buffer point to a copy of the string's memory, including zero terminator
    //! \param o string to copy
    //! \return Buffer containing a copy of the string
    Buffer& operator=(const std::string& o);
private:
    uint8_t* m_buffer;
    size_t   m_size, m_allocated;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_BUFFER_H__
