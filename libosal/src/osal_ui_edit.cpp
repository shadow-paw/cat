#include "osal_ui_edit.h"
#include "osal_gfx_renderer.h"
#include "osal_ui_service.h"
#if defined(PLATFORM_MAC)
  #import <Cocoa/Cocoa.h>
#elif defined(PLATFORM_IOS)
  #import <UIKit/UIKit.h>
#endif

using namespace osal;

// ----------------------------------------------------------------------------
Editbox::Editbox(KernelApi* kernel, const Rect2i& rect, unsigned int id) : Widget(kernel, rect, id) {
    m_texrefs.resize(1);
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    m_font = nullptr;
    m_native_ctrl = CreateWindowEx(0, L"EDIT", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|ES_AUTOHSCROLL,
        0, 0, 0, 0,
        kernel->psd()->rootview, NULL,
        (HINSTANCE)GetWindowLongPtr(get_psd()->rootview, GWLP_HINSTANCE),
        NULL);
#elif defined(PLATFORM_MAC)
    NSView* rootview = (__bridge NSView*)kernel->psd()->rootview;
    NSTextField* tv = [[NSTextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
    tv.bezeled = NO;
    tv.bordered = NO;
    tv.drawsBackground = NO;
    [tv.cell setWraps:NO];
    [tv.cell setScrollable:YES];
    [rootview addSubview:tv];
    m_native_ctrl = (__bridge void*)tv;
#elif defined(PLATFORM_IOS)
    UIView* rootview = (__bridge UIView*)kernel->psd()->rootview;
    UITextField* tv = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
    [rootview addSubview:tv];
    m_native_ctrl = (__bridge void*)tv;
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // jcontext = rootview.getContext();
    jobject jcontext = jni.call_object(kernel->psd()->rootview, "getContext", "()Landroid/content/Context;");
    // lp = new LayoutParam(0, 0);
    jobject jlp = jni.new_object("android/view/ViewGroup$LayoutParams", "(II)V", 0, 0);
    // jedit = new EditText(context);
    jobject jedit = jni.new_object("android/widget/EditText", "(Landroid/content/Context;)V", jcontext);
    // jedit.setBackground(null);
    jni.call_void(jedit, "setBackground", "(Landroid/graphics/drawable/Drawable;)V", nullptr);
    // jedit.setPadding(0,0,0,0);
    jni.call_void(jedit, "setPadding", "(IIII)V", 0, 0, 0, 0);
    // jedit.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
    const int TYPE_TEXT_FLAG_NO_SUGGESTIONS = 0x00080000;
    jni.call_void(jedit, "setInputType", "(I)V", TYPE_TEXT_FLAG_NO_SUGGESTIONS);
    // jedit.setSingleLine();
    jni.call_void(jedit, "setSingleLine", "()V");
    // rootview.addView(jedit, lp);
    jni.call_void(kernel->psd()->rootview, "addView", "(Landroid/view/View;Landroid/view/ViewGroup$LayoutParams;)V", jedit, jlp);
    // Retain Reference
    m_native_ctrl = jni.NewGlobalRef(jedit);
#else
  #error Not Implemented!
#endif
    update_font();
    update_textcolor();
}
// ----------------------------------------------------------------------------
Editbox::~Editbox() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (m_native_ctrl) {
        DestroyWindow(m_native_ctrl);
        m_native_ctrl = nullptr;
    }
    if (m_font) {
        DeleteObject(m_font);
        m_font = nullptr;
    }
#elif defined(PLATFORM_MAC)
    NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;
    [tv removeFromSuperview];
    m_native_ctrl = nullptr;
#elif defined(PLATFORM_IOS)
    UITextField* tv = (__bridge UITextField*)m_native_ctrl;
    [tv removeFromSuperview];
    m_native_ctrl = nullptr;
#elif defined(PLATFORM_ANDROID)
    if (m_native_ctrl) {
        JNIHelper jni;
        // rootview.removeView(edit);
        jni.call_void(kernel()->psd()->rootview, "removeView", "(Landroid/view/View;)V", m_native_ctrl);
        jni.DeleteGlobalRef(m_native_ctrl);
        m_native_ctrl = nullptr;
    }
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void Editbox::cb_visible(bool b) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    if (b) {
        ShowWindow(m_native_ctrl, SW_SHOW);
        SetFocus(m_native_ctrl);
    } else {
        ShowWindow(m_native_ctrl, SW_HIDE);
    }
#elif defined(PLATFORM_MAC)
    NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;
    [tv setHidden:!b];
#elif defined(PLATFORM_IOS)
    UITextField* tv = (__bridge UITextField*)m_native_ctrl;
    [tv setHidden:!b];
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    const int VISIBLE = 0;
    const int INVISIBLE = 4;
    // ctrl.setVisibility( b ? VISIBLE : INVISIBLE);
    jni.call_void(m_native_ctrl, "setVisibility", "(I)V", b ? VISIBLE : INVISIBLE);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void Editbox::cb_uiscale() {
    cb_resize();
}
// ----------------------------------------------------------------------------
void Editbox::cb_resize() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    Rect2i scaled_rect = m_absrect * kernel()->ui()->get_scale();
    MoveWindow(m_native_ctrl,
        scaled_rect.origin.x, scaled_rect.origin.y,
        scaled_rect.size.width + 1, scaled_rect.size.height + 1,
        TRUE);
#elif defined(PLATFORM_MAC)
    Rect2i scaled_rect = m_absrect * kernel()->ui()->get_scale();
    scaled_rect.origin.y = kernel()->ui()->get_height() - scaled_rect.origin.y - scaled_rect.size.height; // invert Y-coordinate
    NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;
    [tv setFrame:CGRectMake(scaled_rect.origin.x, scaled_rect.origin.y, scaled_rect.size.width + 1, scaled_rect.size.height + 1)];
#elif defined(PLATFORM_IOS)
    Rect2i scaled_rect = m_absrect * kernel()->ui()->get_scale();
    UITextField* tv = (__bridge UITextField*)m_native_ctrl;
    [tv setFrame:CGRectMake(scaled_rect.origin.x, scaled_rect.origin.y, scaled_rect.size.width + 1, scaled_rect.size.height + 1)];
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    Rect2i scaled_rect = m_absrect * kernel()->ui()->get_scale();
    // lp = ctrl.getLayoutParams();
    jobject jlp = jni.call_object(m_native_ctrl, "getLayoutParams", "()Landroid/view/ViewGroup$LayoutParams;");
    // lp.leftMargin = x; lp.topMargin = y;
    // lp.width = width; lp.height = height;
    jclass jlp_class = (jclass)jni.env()->FindClass("android/view/ViewGroup$MarginLayoutParams");
    jni.env()->SetIntField(jlp, jni.env()->GetFieldID(jlp_class, "leftMargin", "I"), scaled_rect.origin.x);
    jni.env()->SetIntField(jlp, jni.env()->GetFieldID(jlp_class, "topMargin", "I"), scaled_rect.origin.y);
    jni.env()->SetIntField(jlp, jni.env()->GetFieldID(jlp_class, "width", "I"), scaled_rect.size.width);
    jni.env()->SetIntField(jlp, jni.env()->GetFieldID(jlp_class, "height", "I"), scaled_rect.size.height);
    // ctrl.setLayoutParams(lp);
    jni.call_void(m_native_ctrl, "setLayoutParams", "(Landroid/view/ViewGroup$LayoutParams;)V", jlp);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void Editbox::cb_render(Renderer* r, unsigned long now) {
    r->draw2d.fill(m_absrect, m_bgcolor, m_texrefs[TexBackground], now);
}
// ----------------------------------------------------------------------------
void Editbox::set_text(const std::string& s) {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    size_t len = s.length() +4;
    TCHAR* text = new TCHAR[len];
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, text, (int)len);
    SetWindowText(m_native_ctrl, text);
    delete text;
#elif defined(PLATFORM_MAC)
    NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;
    [tv setStringValue:[NSString stringWithUTF8String:s.c_str()]];
#elif defined(PLATFORM_IOS)
    UITextField* tv = (__bridge UITextField*)m_native_ctrl;
    [tv setText:[NSString stringWithUTF8String:s.c_str()]];
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // jedit.setText(s);
    //jclass jedit_class = (jclass)jni.env()->GetObjectClass(m_native_ctrl);
    //jmethodID jedit_setText = jni.env()->GetMethodID(jedit_class, "setText", "(Ljava/lang/CharSequence;)V");
    //jni.env()->CallVoidMethod(m_native_ctrl, jedit_setText, jni.env()->NewStringUTF(s.c_str()));
    jni.call_void(m_native_ctrl, "setText", "(Ljava/lang/CharSequence;)V", jni.env()->NewStringUTF(s.c_str()));
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
std::string Editbox::get_text() const {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    TCHAR	text[512];
    char des[512] = { 0 };
    GetWindowText(m_native_ctrl, text, 512);
    WideCharToMultiByte(CP_UTF8, 0, text, -1, des, (int)sizeof(des), NULL, NULL);
    return std::string(des);
#elif defined(PLATFORM_MAC)
    NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;
    return [[tv stringValue] UTF8String];
#elif defined(PLATFORM_IOS)
    UITextField* tv = (__bridge UITextField*)m_native_ctrl;
    return [tv.text UTF8String];
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // jseq = jedit.getText();
    jobject jseq = jni.call_object(m_native_ctrl, "getText", "()Ljava/lang/CharSequence;");
    // jstr = jseq.toString();
    jstring jstr = (jstring)jni.call_object(jseq, "toString", "()Ljava/lang/String;");
    const char* s = jni.env()->GetStringUTFChars(jstr, 0);
    return std::string(s);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void Editbox::update_font() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    LOGFONT lf = { 0 };
    lf.lfHeight = -m_textstyle.fontsize;  // negative = pixel
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = (m_textstyle.appearance & TextStyle::Bold) ? FW_HEAVY : FW_SEMIBOLD;
    lf.lfItalic = (m_textstyle.appearance & TextStyle::Italic) ? TRUE : FALSE;
    lf.lfUnderline = (m_textstyle.appearance & TextStyle::Underline) ? TRUE : FALSE;
    lf.lfStrikeOut = (m_textstyle.appearance & TextStyle::Strike) ? TRUE : FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = NONANTIALIASED_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    wcsncpy_s(lf.lfFaceName, L"Verdana", sizeof(lf.lfFaceName) / sizeof(lf.lfFaceName[0]));
    HFONT hFont;
    hFont = CreateFontIndirect(&lf);
    if (hFont == NULL) return;
    SendMessage(m_native_ctrl, WM_SETFONT, (WPARAM)hFont, TRUE);
    if (m_font) DeleteObject(m_font);
    m_font = hFont;
#elif defined(PLATFORM_MAC)
    // NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;

#elif defined(PLATFORM_IOS)
    // UITextField* tv = (__bridge UITextField*)m_native_ctrl;

#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // jedit.setTextSize(s);
    jni.call_void(m_native_ctrl, "setTextSize", "(F)V", (float)m_textstyle.fontsize);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void Editbox::update_textcolor() {
#if defined(PLATFORM_WIN32) || defined(PLATFORM_WIN64)
    // TODO: Need to hijack WM_CTLCOLOREDIT
#elif defined(PLATFORM_MAC)
    NSTextField* tv = (__bridge NSTextField*)m_native_ctrl;
    NSColor* color = [NSColor colorWithRed:((CGFloat)(m_textstyle.color & 0xFF)) / 255.0f
                              green:((CGFloat)((m_textstyle.color >> 8) & 0xFF)) / 255.0f
                              blue :((CGFloat)((m_textstyle.color >> 16) & 0xFF)) / 255.0f
                              alpha:((CGFloat)((m_textstyle.color >> 24) & 0xFF)) / 255.0f];
    [tv setTextColor:color];
    // cursor color
    NSTextView *editor = (NSTextView*)[tv.window fieldEditor:YES forObject:tv];
    editor.insertionPointColor = color;
#elif defined(PLATFORM_IOS)
    UITextField* tv = (__bridge UITextField*)m_native_ctrl;
    UIColor* color = [UIColor colorWithRed:((CGFloat)(m_textstyle.color & 0xFF)) / 255.0f
                              green:((CGFloat)((m_textstyle.color >> 8) & 0xFF)) / 255.0f
                              blue :((CGFloat)((m_textstyle.color >> 16) & 0xFF)) / 255.0f
                              alpha:((CGFloat)((m_textstyle.color >> 24) & 0xFF)) / 255.0f];
    tv.textColor = color;
    [tv setTintColor:color];    // cursor color
#elif defined(PLATFORM_ANDROID)
    JNIHelper jni;
    // jedit.setTextColor(c);
    jni.call_void(m_native_ctrl, "setTextColor", "(I)V", m_textstyle.color);
#else
  #error Not Implemented!
#endif
}
// ----------------------------------------------------------------------------
void Editbox::set_textstyle(const TextStyle& style) {
    if (m_textstyle.appearance != style.appearance || m_textstyle.fontsize != style.fontsize) {
        m_textstyle.appearance = style.appearance;
        m_textstyle.fontsize = style.fontsize;
        update_font();
    }
}
// ----------------------------------------------------------------------------
void Editbox::set_textsize(int size) {
    if (m_textstyle.fontsize==size) return;
    m_textstyle.fontsize = size;
    update_font();
}
// ----------------------------------------------------------------------------
void Editbox::set_textcolor(uint32_t color) {
    if (m_textstyle.color == color) return;
    m_textstyle.color = color;
    update_textcolor();
}
// ----------------------------------------------------------------------------
