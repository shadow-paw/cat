#include "pane1.h"

using namespace app;
using namespace cat;

// ----------------------------------------------------------------------------
Pane1::Pane1(KernelApi* kernel_api, const Rect2i& rect, unsigned int id) : Pane(kernel_api, rect, id) {
    m_sfx = kernel()->sound()->load_effect("/assets/sound/click.m4a"); // NOTE: Android SoundPool has an internal 1M limit on decoded buffer
                                                                       //       long sound file will get chopped around 6sec.
    m_audio = kernel()->sound()->load_audio("/assets/sound/audio.m4a");// AudioPlayer does not have size limitation.

    const char* ui_image = "/assets/ui/default_ui.png";
    // Pane properties
    this->set_texture(Pane::TexBackground, ui_image, 0, 0, 92, 38, 6, 6);
    this->set_bounded(true);
    this->set_draggable(false);
    // Childs
    auto button1 = new Button(kernel(), Rect2i(0, 0, 120, 40));
    button1->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button1->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button1->set_text("Effect");
    this->attach(button1);
    auto button2 = new Button(kernel(), Rect2i(0, 0, 120, 40));
    button2->set_texture(Button::TexNormal, ui_image, 0, 40, 92, 78, 6, 6);
    button2->set_texture(Button::TexPressed, ui_image, 94, 40, 186, 78, 6, 6);
    button2->set_text("Audio");
    this->attach(button2);

    // relayout
    this->ev_layout += [button1, button2](Widget* w) -> void {
        button1->set_origin((w->get_size().width - button1->get_size().width)/2,
                            (w->get_size().height - (button1->get_size().height + button2->get_size().height + 16)) /2);
        button2->set_origin((w->get_size().width - button2->get_size().width) / 2,
                            (w->get_size().height + (button1->get_size().height + button2->get_size().height + 16)) /2);
    };
    // Events
    button1->ev_click += [this](Widget* w) -> void {
        Logger::d("App", "Pane1::button1 Clicked!");
        kernel()->sound()->play(m_sfx, 1.0f, 1.0f);
    };
    button2->ev_check += [this](Widget* w, bool checked) -> void {
        Logger::d("App", "Pane1::button2 Clicked!");
        if (m_audio) {
            if (m_audio->is_playing()) {
                m_audio->pause();
            } else {
                m_audio->play();
            }
        }
    };
    if (m_audio) {
        m_audio->ev_status += [](AudioPlayer* player, AudioPlayer::Status status) -> void {
            Logger::d("App", "AudioPlayer status = %d", status);
        };
    }
}
// ----------------------------------------------------------------------------
Pane1::~Pane1() {
    if (m_sfx) {
        kernel()->sound()->unload(m_sfx);
        m_sfx = nullptr;
    }
    if (m_audio) {
        m_audio->release();
        m_audio = nullptr;
    }
}
// ----------------------------------------------------------------------------
void Pane1::cb_pause(bool paused) {
    if (m_audio) m_audio->pause();
}
// ----------------------------------------------------------------------------
