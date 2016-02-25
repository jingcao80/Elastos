#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_INPUTMETHODBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_INPUTMETHODBUTTON_H__

#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class InputMethodButton : public ImageView
{
public:
    InputMethodButton();

    InputMethodButton(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    ECode SetIconImage(
        /* [in] */ Int32 resId);

    ECode SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean showButton);

    ECode SetHardKeyboardStatus(
        /* [in] */ Boolean available);

    ECode SetScreenLocked(
        /* [in] */ Boolean locked);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    virtual CARAPI OnAttachedToWindow();

private:
    // Refer to InputMethodManagerService.needsToShowImeSwitchOngoingNotification()
    Boolean NeedsToShowIMEButtonWhenVisibilityAuto();

    Boolean NeedsToShowIMEButton();

    void RefreshStatusIcon();

    Int32 LoadInputMethodSelectorVisibility();

private:
    static const String TAG;

   // These values are defined in Settings application.
    static const Int32 ID_IME_BUTTON_VISIBILITY_AUTO;// = 0;
    static const Int32 ID_IME_BUTTON_VISIBILITY_ALWAYS_SHOW;// = 1;
    static const Int32 ID_IME_BUTTON_VISIBILITY_ALWAYS_HIDE;// = 2;

    // other services we wish to talk to
    AutoPtr<IInputMethodManager> mImm;
    Int32 mId;
    AutoPtr<IImageView> mIcon;
    AutoPtr<IBinder> mToken;
    Boolean mShowButton;// = false;
    Boolean mScreenLocked;// = false;
    Boolean mHardKeyboardAvailable;

    // Please refer to InputMethodManagerService.TAG_TRY_SUPPRESSING_IME_SWITCHER
    static const String TAG_TRY_SUPPRESSING_IME_SWITCHER;// = "TrySuppressingImeSwitcher";

};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_INPUTMETHODBUTTON_H__
