
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDVIEW_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/inputmethodservice/KeyboardView.h"

using Elastos::Droid::InputMethodService::KeyboardView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PasswordEntryKeyboardView
    : public KeyboardView
    , public IPasswordEntryKeyboardView
{
public:
    CAR_INTERFACE_DECL()

    PasswordEntryKeyboardView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetShifted(
        /* [in] */ Boolean shifted,
        /* [out] */ Boolean* res);

protected:
    static const Int32 KEYCODE_OPTIONS;
    static const Int32 KEYCODE_SHIFT_LONGPRESS;
    static const Int32 KEYCODE_VOICE;
    static const Int32 KEYCODE_F1;
    static const Int32 KEYCODE_NEXT_LANGUAGE;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDVIEW_H__
