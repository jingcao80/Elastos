
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYVIEWHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYVIEWHELPER_H__

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
 * Some common functions that are useful for KeyguardSecurityViews.
 */
class KeyguardSecurityViewHelper
    : public Object
{
private:
    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("KeyguardSecurityViewHelper::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ IView* ecaView)
            : mEcaView(ecaView)
        {}

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mEcaView;

        Boolean mCanceled;
    };

public:
    static CARAPI ShowBouncer(
        /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
        /* [in] */ IView* ecaView,
        /* [in] */ IDrawable* bouncerFrame,
        /* [in] */ Int32 duration);

    static CARAPI HideBouncer(
        /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
        /* [in] */ IView* ecaView,
        /* [in] */ IDrawable* bouncerFrame,
        /* [in] */ Int32 duration);
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSECURITYVIEWHELPER_H__