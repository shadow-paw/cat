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
    Emitter() = default;
    Emitter(const Emitter& o) = delete;
    Emitter(Emitter&& o) = delete;

    typedef std::function<void(int, const ARG&...)> HANDLER;
    bool on(int ev, HANDLER handler);
    bool once(int ev, HANDLER handler);
    bool remove(int ev, HANDLER handler);
    void emit(int ev, const ARG&...);
private:
    struct NODE {
        HANDLER handler;
        bool active, once;
    };
    std::recursive_mutex m_mutex;
    std::unordered_map<int,std::vector<NODE>> m_map;
};
// ----------------------------------------------------------------------------
// Emitter
// ----------------------------------------------------------------------------
template <class... ARG>
bool Emitter<ARG...>::on(int ev, HANDLER handler) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto map_it = m_map.find(ev);
    if (map_it == m_map.end()) {
        std::vector<NODE> list;
        auto em = m_map.emplace(ev, list);
        if (!em.second) return false;
        em.first->second.push_back({handler, true, false});
    } else {
        map_it->second.push_back({handler, true, false});
    } return true;
}
template <class... ARG>
bool Emitter<ARG...>::once(int ev, HANDLER handler) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto map_it = m_map.find(ev);
    if (map_it == m_map.end()) {
        std::vector<NODE> list;
        auto em = m_map.emplace(ev, list);
        if (!em.second) return false;
        em.first->second.push_back({ handler, true, true });
    } else {
        map_it->second.push_back({ handler, true, true });
    } return true;
}
template <class... ARG>
bool Emitter<ARG...>::remove(int ev, HANDLER handler) {
    auto handler_p = handler.target<HANDLER>();
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto map_it = m_map.find(ev);
    if (map_it == m_map.end()) return false;
    auto& list = map_it->second;
    for (auto& node: list) {
        if (handler_p == node.handler.target<HANDLER>()) {
            node.active = false;
            return true;
        }
    } return false;
}
template <class... ARG>
void Emitter<ARG...>::emit(int ev, const ARG&... arg) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto map_it = m_map.find(ev);
    if (map_it == m_map.end()) return;
    auto& list = map_it->second;
    for (auto it = list.begin(); it!=list.end(); ) {
        if (it->active) {
            it->handler(ev, arg...);
            if (it->once) {
                it = list.erase(it);
            } else {
                ++it;
            }
        } else {
            it = list.erase(it);
        }
    }
}
// ----------------------------------------------------------------------------
} // namespace cat

#endif // __CAT_DATA_EMITTER_H__
