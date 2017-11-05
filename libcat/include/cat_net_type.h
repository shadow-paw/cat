#ifndef __CAT_NET_TYPE_H__
#define __CAT_NET_TYPE_H__

#include <unordered_map>
#include "cat_data_buffer.h"

namespace cat {
// ----------------------------------------------------------------------------
typedef unsigned int HTTP_ID;
struct HTTP_REQUEST {
    std::string url;
    std::unordered_multimap<std::string, std::string> headers;
    Buffer data;
};
struct HTTP_RESPONSE {
    int code;
    std::unordered_multimap<std::string, std::string> headers;
    Buffer body;
};
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_NET_TYPE_H__
