#include "cat_ui_animator.h"
#include <new>
#include "cat_time_service.h"
#include "cat_ui_widget.h"

using namespace cat;

// ----------------------------------------------------------------------------
Animator::Animator() {
    m_duration = 250;
    m_started = false;
}
// ----------------------------------------------------------------------------
Animator::~Animator() {
}
// ----------------------------------------------------------------------------
void Animator::set_interpolator(std::shared_ptr<Interpolator> interpolator) {
    m_interpolator = interpolator;
}
// ----------------------------------------------------------------------------
void Animator::set_duration(Timestamp duration) {
    m_duration = duration;
}
// ----------------------------------------------------------------------------
void Animator::set_callback(std::function<void()> cb) {
    m_cb = cb;
}
// ----------------------------------------------------------------------------
void Animator::start() {
    if (!m_interpolator) {
        auto default_interpolator = new (std::nothrow) LinearInterpolator();
        if (!default_interpolator) return;
        m_interpolator = std::shared_ptr<Interpolator>(default_interpolator);
    }
    m_starttime = TimeService::now();
    m_started = true;
}
// ----------------------------------------------------------------------------
void Animator::stop() {
    if (m_started) {
        m_started = false;
        if (m_cb) m_cb();
    }
}
// ----------------------------------------------------------------------------
void Animator::cancel() {
    if (m_started) {
        m_started = false;
        if (m_cb) m_cb();
    }
}
// ----------------------------------------------------------------------------
bool Animator::update(Timestamp now) {
    if (m_started) {
        cb_animate(now);
        return true;
    } return false;
}
// ----------------------------------------------------------------------------
// TranslateAnimator
// ----------------------------------------------------------------------------
TranslateAnimator::TranslateAnimator(Widget* widget) {
    m_widget = widget;
}
// ----------------------------------------------------------------------------
void TranslateAnimator::start(const Point2i& from, const Point2i& to) {
    m_from = from;
    m_to = to;
    Animator::start();
}
// ----------------------------------------------------------------------------
void TranslateAnimator::cb_animate(Timestamp now) {
    Timestamp dt = now - m_starttime;
    if (dt < m_duration) {
        float value = m_interpolator->get((float)dt / m_duration);
        m_widget->set_pos((int)(m_from.x + (m_to.x - m_from.x) * value),
            (int)(m_from.y + (m_to.y - m_from.y) * value));
    } else {
        m_widget->set_pos(m_to);
        stop();
    }
}
// ----------------------------------------------------------------------------
// OpacityAnimator
// ----------------------------------------------------------------------------
OpacityAnimator::OpacityAnimator(Widget* widget) {
    m_widget = widget;
}
// ----------------------------------------------------------------------------
void OpacityAnimator::start(float from, float to) {
    m_from = from;
    m_to = to;
    Animator::start();
}
// ----------------------------------------------------------------------------
void OpacityAnimator::cb_animate(Timestamp now) {
    Timestamp dt = now - m_starttime;
    if (dt < m_duration) {
        float value = m_interpolator->get((float)dt / m_duration);
        float opacity =m_from + (m_to - m_from) * value;
        m_widget->set_opacity(opacity);
    } else {
        m_widget->set_opacity(m_to);
        stop();
    }
}
// ----------------------------------------------------------------------------
