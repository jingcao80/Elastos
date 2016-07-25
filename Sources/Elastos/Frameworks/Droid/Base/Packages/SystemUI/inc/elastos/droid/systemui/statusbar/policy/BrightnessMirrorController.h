
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BRIGHTNESSMIRRORCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BRIGHTNESSMIRRORCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::SystemUI::StatusBar::IScrimView;
using Elastos::Droid::SystemUI::StatusBar::Phone::IStatusBarWindowView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Controls showing and hiding of the brightness mirror.
 */
class BrightnessMirrorController
    : public Object
    , public IBrightnessMirrorController
{
private:
    class Runnable1 : public Runnable
    {
    public:
        Runnable1(
            /* [in] */ BrightnessMirrorController* host);

        // @Override
        CARAPI Run();

    private:
        BrightnessMirrorController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("BrightnessMirrorController")

    BrightnessMirrorController();

    CARAPI constructor(
        /* [in] */ IStatusBarWindowView* statusBarWindow);

    CARAPI ShowMirror();

    CARAPI HideMirror();

    CARAPI SetLocation(
        /* [in] */ IView* original);

    CARAPI GetMirror(
        /* [out] */ IView** view);

    CARAPI UpdateResources();

private:
    CARAPI_(AutoPtr<IViewPropertyAnimator>) OutAnimation(
        /* [in] */ IViewPropertyAnimator* a);

    CARAPI_(AutoPtr<IViewPropertyAnimator>) InAnimation(
        /* [in] */ IViewPropertyAnimator* a);

public:
    Int64 TRANSITION_DURATION_OUT;
    Int64 TRANSITION_DURATION_IN;

private:
    AutoPtr<IScrimView> mScrimBehind;
    AutoPtr<IView> mBrightnessMirror;
    AutoPtr<IView> mPanelHolder;
    AutoPtr<ArrayOf<Int32> > mInt2Cache;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_BRIGHTNESSMIRRORCONTROLLER_H__
