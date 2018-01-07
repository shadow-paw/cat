#ifndef __CAT_DATA_COPYABLE_H__
#define __CAT_DATA_COPYABLE_H__

namespace cat {
// ----------------------------------------------------------------------------
template <class T>
class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable& operator = (const NonCopyable&) = delete;
};// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_COPYABLE_H__
