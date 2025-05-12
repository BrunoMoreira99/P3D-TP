#pragma once

#include "Roose/Events/Event.h"
#include "Roose/Input/KeyCode.h"

namespace Roose {

    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(const KeyCode keycode)
            : m_KeyCode(keycode) {}

        KeyCode m_KeyCode;
    };

    class KeyDownEvent : public KeyEvent
    {
    public:
        KeyDownEvent(const KeyCode keycode, bool isRepeat = false)
            : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

        bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyDownEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyDown)
    private:
        bool m_IsRepeat;
    };

    class KeyUpEvent : public KeyEvent
    {
    public:
        KeyUpEvent(const KeyCode keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyUpEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyUp)
    };

    class KeyPressEvent : public KeyEvent
    {
    public:
        KeyPressEvent(const KeyCode keycode)
            : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPress)
    };
}
