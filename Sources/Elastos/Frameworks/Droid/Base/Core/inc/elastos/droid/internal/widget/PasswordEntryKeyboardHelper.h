
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDHELPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/widget/CPasswordEntryKeyboard.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::InputMethodService::IKeyboardView;
using Elastos::Droid::InputMethodService::IOnKeyboardActionListener;

using Elastos::Core::ICharSequence;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PasswordEntryKeyboardHelper
    : public Object
    , public IPasswordEntryKeyboardHelper
{
private:
    class InnerListener
        : public Object
        , public IOnKeyboardActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ PasswordEntryKeyboardHelper* host);

        CARAPI OnKey(
            /* [in] */ Int32 primaryCode,
            /* [in] */ ArrayOf<Int32>* keyCodes);

        CARAPI OnPress(
            /* [in] */ Int32 primaryCode);

        CARAPI OnRelease(
            /* [in] */ Int32 primaryCode);

        CARAPI OnText(
            /* [in] */ ICharSequence* text);

        CARAPI SwipeDown();

        CARAPI SwipeLeft();

        CARAPI SwipeRight();

        CARAPI SwipeUp();

    private:
        PasswordEntryKeyboardHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PasswordEntryKeyboardHelper();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IKeyboardView* keyboardView,
        /* [in] */ IView* targetView);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IKeyboardView* keyboardView,
        /* [in] */ IView* targetView,
        /* [in] */ Boolean useFullScreenWidth);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IKeyboardView* keyboardView,
        /* [in] */ IView* targetView,
        /* [in] */ Boolean useFullScreenWidth,
        /* [in] */ ArrayOf<Int32>* layouts);

    CARAPI CreateKeyboards();

    CARAPI SetEnableHaptics(
        /* [in] */ Boolean enabled);

    CARAPI IsAlpha(
        /* [out] */ Boolean* alpha);

    CARAPI SetKeyboardMode(
        /* [in] */ Int32 mode);

    CARAPI SendDownUpKeyEvents(
        /* [in] */ Int32 keyEventCode);

    CARAPI OnKey(
        /* [in] */ Int32 primaryCode,
        /* [in] */ ArrayOf<Int32>* keyCodes);

    /**
     * Sets and enables vibrate pattern.  If id is 0 (or can't be loaded), vibrate is disabled.
     * @param id resource id for array containing vibrate pattern.
     */
    CARAPI SetVibratePattern(
        /* [in] */ Int32 id);

    CARAPI HandleBackspace();

    CARAPI OnPress(
        /* [in] */ Int32 primaryCode);

    CARAPI OnRelease(
        /* [in] */ Int32 primaryCode);

    CARAPI OnText(
        /* [in] */ ICharSequence* text);

    CARAPI SwipeDown();

    CARAPI SwipeLeft();

    CARAPI SwipeRight();

    CARAPI SwipeUp();

private:
    CARAPI_(void) CreateKeyboardsWithSpecificSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) CreateKeyboardsWithDefaultWidth();

    CARAPI_(void) SendKeyEventsToTarget(
        /* [in] */ Int32 character);

    CARAPI_(void) HandleModeChange();

    CARAPI_(void) HandleShift();

    CARAPI_(void) HandleCharacter(
        /* [in] */ Int32 primaryCode,
        /* [in] */ ArrayOf<Int32>* keyCodes);

    CARAPI_(void) HandleClose();

    CARAPI_(void) PerformHapticFeedback();

public:
    static const Int32 KEYBOARD_MODE_ALPHA;
    static const Int32 KEYBOARD_MODE_NUMERIC;

private:
    static const Int32 KEYBOARD_STATE_NORMAL;
    static const Int32 KEYBOARD_STATE_SHIFTED;
    static const Int32 KEYBOARD_STATE_CAPSLOCK;
    static const String TAG;
    Int32 mKeyboardMode;
    Int32 mKeyboardState;
    AutoPtr<CPasswordEntryKeyboard> mQwertyKeyboard;
    AutoPtr<CPasswordEntryKeyboard> mQwertyKeyboardShifted;
    AutoPtr<CPasswordEntryKeyboard> mSymbolsKeyboard;
    AutoPtr<CPasswordEntryKeyboard> mSymbolsKeyboardShifted;
    AutoPtr<CPasswordEntryKeyboard> mNumericKeyboard;
    AutoPtr<IContext> mContext;
    AutoPtr<IView> mTargetView;
    AutoPtr<IKeyboardView> mKeyboardView;
    AutoPtr<ArrayOf<Int64> > mVibratePattern;
    Boolean mEnableHaptics;

    static const Int32 NUMERIC;
    static const Int32 QWERTY;
    static const Int32 QWERTY_SHIFTED;
    static const Int32 SYMBOLS;
    static const Int32 SYMBOLS_SHIFTED;

    AutoPtr<ArrayOf<Int32> > mLayouts;

    Boolean mUsingScreenWidth;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDHELPER_H__
