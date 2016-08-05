
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYBUTTON_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYBUTTON_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CEmergencyButton.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"
#include <elastos/droid/widget/Button.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::Button;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CEmergencyButton)
    , public Button
    , public IEmergencyButton
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CEmergencyButton::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CEmergencyButton* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnSimStateChanged(
            /* [in] */ IccCardConstantsState simState);

        //@Override
        CARAPI OnPhoneStateChanged(
            /* [in] */ Int32 phoneState);

    private:
        CEmergencyButton* mHost;
    };

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

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CEmergencyButton();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Shows the emergency dialer or returns the user to the existing call.
     */
    CARAPI TakeEmergencyCallAction();

protected:
    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) UpdateEmergencyCallButton(
        /* [in] */ IccCardConstantsState simState,
        /* [in] */ Int32 phoneState);

public:
    AutoPtr<KeyguardUpdateMonitorCallback> mInfoCallback;

private:
    static const Int32 EMERGENCY_CALL_TIMEOUT; // screen timeout after starting e.d.
    static const String ACTION_EMERGENCY_DIAL;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IPowerManager> mPowerManager;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CEMERGENCYBUTTON_H__
