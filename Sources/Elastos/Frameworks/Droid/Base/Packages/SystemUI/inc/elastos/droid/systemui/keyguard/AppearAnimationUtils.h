
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_APPEARANIMATIONUTILS_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_APPEARANIMATIONUTILS_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class AppearAnimationUtils
    : public Object
    , public IAppearAnimationCreator
{
public:
    class AppearAnimationProperties
        : public Object
    {
    public:
        TO_STRING_IMPL("AppearAnimationUtils::AppearAnimationProperties")

        AppearAnimationProperties();

    public:
        AutoPtr<ArrayOf<ArrayOf<Int64>*> > mDelays;
        Int32 mMaxDelayRowIndex;
        Int32 mMaxDelayColIndex;
    };

public:
    CAR_INTERFACE_DECL()

    AppearAnimationUtils();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ Int64 duration,
        /* [in] */ Float translationScaleFactor,
        /* [in] */ Float delayScaleFactor,
        /* [in] */ IInterpolator* interpolator);

    CARAPI StartAppearAnimation(
        /* [in] */ ArrayOf<ArrayOf<IView*>* >* objects,
        /* [in] */ IRunnable* finishListener);

    CARAPI StartAppearAnimation(
        /* [in] */ ArrayOf<IView*>* objects,
        /* [in] */ IRunnable* finishListener);

    CARAPI StartAppearAnimation(
        /* [in] */ ArrayOf<ArrayOf<IInterface*>* >* objects,
        /* [in] */ IRunnable* finishListener,
        /* [in] */ IAppearAnimationCreator* creator);

    CARAPI StartAppearAnimation(
        /* [in] */ ArrayOf<IInterface*>* objects,
        /* [in] */ IRunnable* finishListener,
        /* [in] */ IAppearAnimationCreator* creator);

    CARAPI GetInterpolator(
        /* [out] */ IInterpolator** interpolator);

    CARAPI GetStartTranslation(
        /* [out] */ Float* translation);

    //@Override
    CARAPI CreateAnimation(
        /* [in] */ IInterface* view,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration,
        /* [in] */ Float startTranslationY,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ IRunnable* endRunnable);

private:
    CARAPI_(void) StartAnimations(
        /* [in] */ AppearAnimationProperties* properties,
        /* [in] */ ArrayOf<IInterface*>* objects,
        /* [in] */ IRunnable* finishListener,
        /* [in] */ IAppearAnimationCreator* creator);

    CARAPI_(void) StartAnimations(
        /* [in] */ AppearAnimationProperties* properties,
        /* [in] */ ArrayOf<ArrayOf<IInterface*>* >* objects,
        /* [in] */ IRunnable* finishListener,
        /* [in] */ IAppearAnimationCreator* creator);

    CARAPI_(AutoPtr<AppearAnimationProperties>) GetDelays(
        /* [in] */ ArrayOf<IInterface*>* items);

    CARAPI_(AutoPtr<AppearAnimationProperties>) GetDelays(
        /* [in] */ ArrayOf<ArrayOf<IInterface*>* >* items);

    CARAPI_(Int64) CalculateDelay(
        /* [in] */ Int32 row,
        /* [in] */ Int32 col);

public:
    static const Int64 DEFAULT_APPEAR_DURATION;

private:
    AutoPtr<IInterpolator> mInterpolator;
    const Float mStartTranslation;
    AutoPtr<AppearAnimationProperties> mProperties;
    const Float mDelayScale;
    const Int64 mDuration;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_APPEARANIMATIONUTILS_H__