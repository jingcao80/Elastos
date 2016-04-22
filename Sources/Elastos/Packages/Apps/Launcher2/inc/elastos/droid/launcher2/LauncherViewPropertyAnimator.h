#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERVIEWPROPERTYANIMATOR_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERVIEWPROPERTYANIMATOR_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/FirstFrameAnimatorHelper.h"
#include "elastos/droid/animation/Animator.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Animation::Animator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IEnumSet;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class LauncherViewPropertyAnimator
    : public Animator
    , public ILauncherViewPropertyAnimator
    , public IAnimatorListener
{
public:
    CAR_INTERFACE_DECL();

    LauncherViewPropertyAnimator(
        /* [in] */ IView* target);

    //@Override
    CARAPI AddListener(
        /* [in] */ IAnimatorListener* listener);

    //@Override
    CARAPI Cancel();

    //@Override
    CARAPI Clone(
        /* [out] */ IInterface** result);

    //@Override
    CARAPI End();

    //@Override
    CARAPI GetDuration(
        /* [out] */ Int64* duration);

    //@Override
    CARAPI GetListeners(
        /* [out, callee] */ IArrayList** listeners);

    //@Override
    CARAPI GetStartDelay(
        /* [out] */ Int64* delay);

    //@Override
    CARAPI OnAnimationCancel(
        /* [in] */ IAnimator* animation);

    //@Override
    CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    //@Override
    CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    //@Override
    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    //@Override
    CARAPI IsRunning(
        /* [out] */ Boolean* running);

    //@Override
    CARAPI IsStarted(
        /* [out] */ Boolean* started);

    //@Override
    CARAPI RemoveAllListeners();

    //@Override
    CARAPI RemoveListener(
        /* [in] */ IAnimatorListener* listener);

    //@Override
    CARAPI SetDuration(
        /* [in] */ Int64 duration);

    //@Override
    CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* value);

    //@Override
    CARAPI SetStartDelay(
        /* [in] */ Int64 startDelay);

    //@Override
    CARAPI SetTarget(
        /* [in] */ IInterface* target);

    //@Override
    CARAPI SetupEndValues();

    //@Override
    CARAPI SetupStartValues();

    //@Override
    CARAPI Start();

    CARAPI TranslationX(
        /* [in] */ Float value);

    CARAPI TranslationY(
        /* [in] */ Float value);

    CARAPI ScaleX(
        /* [in] */ Float value);

    CARAPI ScaleY(
        /* [in] */ Float value);

    CARAPI RotationY(
        /* [in] */ Float value);

    CARAPI Alpha(
        /* [in] */ Float value);

public:
    AutoPtr<IEnumSet> mPropertiesToSet;

    AutoPtr<IViewPropertyAnimator> mViewPropertyAnimator;
    AutoPtr<IView> mTarget;

    Float mTranslationX;
    Float mTranslationY;
    Float mScaleX;
    Float mScaleY;
    Float mRotationY;
    Float mAlpha;
    Int64 mStartDelay;
    Int64 mDuration;
    AutoPtr<ITimeInterpolator> mInterpolator;
    AutoPtr<IArrayList> mListeners;
    Boolean mRunning;
    AutoPtr<FirstFrameAnimatorHelper> mFirstFrameHelper;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERVIEWPROPERTYANIMATOR_H__