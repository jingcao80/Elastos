
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPLEHOSTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPLEHOSTVIEW_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSimpleHostView.h"
#include <elastos/droid/widget/ImageButton.h>

using Elastos::Droid::Widget::ImageButton;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardSimpleHostView)
    , public KeyguardViewBase
{
private:
    class MyKeyguardUpdateMonitorCallback
        : public KeyguardUpdateMonitorCallback
    {
    public:
        TO_STRING_IMPL("CKeyguardSimpleHostView::MyKeyguardUpdateMonitorCallback")

        MyKeyguardUpdateMonitorCallback(
            /* [in] */ CKeyguardSimpleHostView* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 userId);

        //@Override
        CARAPI OnTrustInitiatedByUser(
            /* [in] */ Int32 userId);

    private:
        CKeyguardSimpleHostView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CKeyguardSimpleHostView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI CleanUp();

    //@Override
    CARAPI GetUserActivityTimeout(
        /* [out] */ Int64* time);

protected:
    //@Override
    CARAPI ShowBouncer(
        /* [in] */ Boolean show);

    //@Override
    CARAPI OnUserSwitching(
        /* [in] */ Boolean switching);

    //@Override
    CARAPI OnCreateOptions(
        /* [in] */ IBundle* options);

    //@Override
    CARAPI OnExternalMotionEvent(
        /* [in] */ IMotionEvent* event);

private:
    AutoPtr<IKeyguardUpdateMonitorCallback> mUpdateCallback;

};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSIMPLEHOSTVIEW_H__
