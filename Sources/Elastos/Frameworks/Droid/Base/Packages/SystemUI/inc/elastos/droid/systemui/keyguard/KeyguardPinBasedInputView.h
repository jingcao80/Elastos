
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDPINBASEDINPUTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDPINBASEDINPUTVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * A Pin based Keyguard input view
 */
class KeyguardPinBasedInputView
    : public KeyguardAbsKeyInputView
    , public IViewOnKeyListener
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyButton::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CEmergencyButton* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEmergencyButton* mHost;
    };

    class MyOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyButton::MyOnClickListener2")

        CAR_INTERFACE_DECL()

        MyOnClickListener2(
            /* [in] */ CEmergencyButton* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEmergencyButton* mHost;
    };

    class MyOnClickListener3
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyButton::MyOnClickListener3")

        CAR_INTERFACE_DECL()

        MyOnClickListener3(
            /* [in] */ CEmergencyButton* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEmergencyButton* mHost;
    };

    class MyViewOnLongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        TO_STRING_IMPL("CEmergencyButton::MyViewOnLongClickListener")

        MyViewOnLongClickListener(
            /* [in] */ CEmergencyButton* host)
            : mHost(host)
        {}

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        CEmergencyButton* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardPinBasedInputView")

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI Reset();

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    CARAPI ResetState();

    //@Override
    CARAPI SetPasswordEntryEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI ResetPasswordText(
        /* [in] */ Boolean animate);

    //@Override
    CARAPI GetPasswordText(
        /* [out] */ String* text);

    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) PerformClick(
        /* [in] */ IView* view);

    CARAPI_(void) PerformNumberClick(
        /* [in] */ Int32 number);

protected:
    AutoPtr<IPasswordTextView> mPasswordEntry;

private:
    AutoPtr<IView> mOkButton;
    AutoPtr<IView> mDeleteButton;
    AutoPtr<IView> mButton0;
    AutoPtr<IView> mButton1;
    AutoPtr<IView> mButton2;
    AutoPtr<IView> mButton3;
    AutoPtr<IView> mButton4;
    AutoPtr<IView> mButton5;
    AutoPtr<IView> mButton6;
    AutoPtr<IView> mButton7;
    AutoPtr<IView> mButton8;
    AutoPtr<IView> mButton9;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDPINBASEDINPUTVIEW_H__