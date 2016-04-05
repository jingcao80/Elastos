#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMUTILS_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMUTILS_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IOnDrawListener;
using Elastos::Droid::Os::Runnable;
using Elastos::Core::Object;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherAnimUtils
{
private:
    class MyAnimatorListener
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);
    };

    class MyOnDrawListener
        : public Object
        , public IOnDrawListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnDrawListener(
            /* [in] */ IAnimator* animator,
            /* [in] */ IView* view);

        CARAPI OnDraw();

    private:
        Boolean mStarted;
        AutoPtr<IAnimator> mAnimator;
        AutoPtr<IView> mView;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IView* view,
            /* [in] */ IOnDrawListener* listener);

        CARAPI Run();

    private:
        AutoPtr<IView> mView;
        AutoPtr<IOnDrawListener> mListener;
    };

public:
    static CARAPI_(void) CancelOnDestroyActivity(
        /* [in] */ IAnimator* a);

    // Helper method. Assumes a draw is pending, and that if the animation's duration is 0
    // it should be cancelled
    static CARAPI_(void) StartAnimationAfterNextDraw(
        /* [in] */ IAnimator* animator,
        /* [in] */ IView* view);

    static CARAPI_(void) OnDestroyActivity();

    static CARAPI_(AutoPtr<IAnimatorSet>) CreateAnimatorSet();

    static CARAPI_(AutoPtr<IValueAnimator>) OfFloat(
        /* [in] */ IView* target,
        /* [in] */ ArrayOf<Float>* values);

    static CARAPI_(AutoPtr<IObjectAnimator>) OfFloat(
        /* [in] */ IView* target,
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Float>* values);

    static CARAPI_(AutoPtr<IObjectAnimator>) OfPropertyValuesHolder(
        /* [in] */ IView* target,
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values);

    static CARAPI_(AutoPtr<IObjectAnimator>) OfPropertyValuesHolder(
        /* [in] */ IInterface* target,
        /* [in] */ IView* view,
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values);

private:
    static CARAPI_(Boolean) InitStaticBlock();

public:
    static AutoPtr<IHashSet> sAnimators;
    static AutoPtr<IAnimatorListener> sEndAnimListener;

    static Boolean mInitStaticBlock;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERANIMUTILS_H__