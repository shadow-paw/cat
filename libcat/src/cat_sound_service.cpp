#include "cat_sound_service.h"

using namespace cat;

// ----------------------------------------------------------------------------
SoundService::SoundService() {
}
// ----------------------------------------------------------------------------
SoundService::~SoundService() {
}
// ----------------------------------------------------------------------------
bool SoundService::init(const PlatformSpecificData* psd) {
    if (!m_effects.init(psd)) return false;
    return true;
}
// ----------------------------------------------------------------------------
void SoundService::fini() {
    m_effects.fini();
}
// ----------------------------------------------------------------------------
void SoundService::pause() {
    m_effects.pause();
}
// ----------------------------------------------------------------------------
void SoundService::resume() {
    m_effects.resume();
}
// ----------------------------------------------------------------------------
const SoundEffect* SoundService::load_effect(const std::string& name) {
    return m_effects.load(name);
}
// ----------------------------------------------------------------------------
bool SoundService::unload(const SoundEffect* sound) {
    return m_effects.unload(sound);
}
// ----------------------------------------------------------------------------
void SoundService::play(const SoundEffect* sound, float left_volume, float right_volume) {
    return m_effects.play(sound, left_volume, right_volume);
}
// ----------------------------------------------------------------------------
