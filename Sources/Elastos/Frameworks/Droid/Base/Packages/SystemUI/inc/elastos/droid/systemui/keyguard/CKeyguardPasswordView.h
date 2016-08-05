
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPASSWORDVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPASSWORDVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardPasswordView.h"
#include "elastos/droid/systemui/keyguard/KeyguardAbsKeyInputView.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Displays an alphanumeric (latin-1) key entry for the user to enter
 * an unlock password
 */
CarClass(CKeyguardPasswordView)
    , public KeyguardAbsKeyInputView
    , public IOnEditorActionListener
    , public ITextWatcher
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyguardPasswordView::MyRunnable")

        MyRunnable(
            /* [in] */ CKeyguardPasswordView* host,
            /* [in] */ Int32 reason)
            : mHost(host)
            , mReason(reason)
        {}

        //@Override
        CARAPI Run();

    private:
        CKeyguardPasswordView* mHost;
        Int32 mReason;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CKeyguardPasswordView::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CKeyguardPasswordView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardPasswordView* mHost;
    };

    class MyTextWatcher
        : public Object
        , public ITextWatcher
    {
    public:
        TO_STRING_IMPL("CKeyguardPasswordView::MyTextWatcher")

        CAR_INTERFACE_DECL()

        MyTextWatcher(
            /* [in] */ CKeyguardPasswordView* host)
            : mHost(host)
        {}

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

    private:
        CKeyguardPasswordView* mHost;
    };

    class MyOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CKeyguardPasswordView::MyOnClickListener2")

        CAR_INTERFACE_DECL()

        MyOnClickListener2(
            /* [in] */ CKeyguardPasswordView* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardPasswordView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardPasswordView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI NeedsInput(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnResume(
        /* [in] */ Int32 reason);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI Reset();

    //@Override
    CARAPI ShowUsabilityHint();

    //@Override
    CARAPI GetWrongPasswordStringId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI StartAppearAnimation();

    //@Override
    CARAPI StartDisappearAnimation(
        /* [in] */ IRunnable* finishRunnable,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    //@Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    //@Override
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

    //@Override
    CARAPI OnEditorAction(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 actionId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    CARAPI ResetState();

    //@Override
    CARAPI GetPasswordTextViewId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ResetPasswordText(
        /* [in] */ Boolean animate);

    //@Override
    CARAPI GetPasswordText(
        /* [out] */ String* str);

    //@Override
    CARAPI SetPasswordEntryEnabled(
        /* [in] */ Boolean enabled);

private:
    /**
     * Method adapted from com.android.inputmethod.latin.Utils
     *
     * @param imm The input method manager
     * @param shouldIncludeAuxiliarySubtypes
     * @return true if we have multiple IMEs to choose from
     */
    CARAPI_(Boolean) HasMultipleEnabledIMEsOrSubtypes(
        /* [in] */ IInputMethodManager* imm,
        /* [in] */ Boolean shouldIncludeAuxiliarySubtypes);

private:
    Boolean mShowImeAtScreenOn;
    Int32 mDisappearYTranslation;

    AutoPtr<IInputMethodManager> mImm;
    AutoPtr<ITextView> mPasswordEntry;
    AutoPtr<IInterpolator> mLinearOutSlowInInterpolator;
    AutoPtr<IInterpolator> mFastOutLinearInInterpolator;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDPASSWORDVIEW_H__
