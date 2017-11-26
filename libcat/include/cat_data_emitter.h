#ifndef __CAT_DATA_EMITTER_H__
#define __CAT_DATA_EMITTER_H__

#include <mutex>
#include <functional>
#include <unordered_map>
#include <vector>

namespace cat {
// ----------------------------------------------------------------------------
template <class... ARG>
class Emitter {
public:
    typedef std::function<void(int, const ARG&...)> HANDLER;
    bool on(int ev, HANDLER handler);
    void emit(int ev, const ARG&...);
private:
    std::mutex m_mutex;
    std::unordered_map<int,std::vector<HANDLER>> m_handlers;
};
// ----------------------------------------------------------------------------
// Emitter
// ----------------------------------------------------------------------------
template <class... ARG>
bool Emitter<ARG...>::on(int ev, HANDLER handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_handlers.find(ev);
    if (it == m_handlers.end()) {
        std::vector<HANDLER> list;
        auto em = m_handlers.emplace(ev, list);
        if (!em.second) return false;
        em.first->second.push_back(handler);
    } else {
        it->second.push_back(handler);
    } return true;
}
template <class... ARG>
void Emitter<ARG...>::emit(int ev, const ARG&... arg) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_handlers.find(ev);
    if (it == m_handlers.end()) return;
    for (auto& handler : it->second) {
        handler(ev, arg...);
    }
}
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_EMITTER_H__
