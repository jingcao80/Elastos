#ifndef __ELASTOS_DROID_VIEW_VIEWPROPERTYANIMATOR_H__
#define __ELASTOS_DROID_VIEW_VIEWPROPERTYANIMATOR_H__

#include "Elastos.Droid.Animation.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewPropertyAnimatorRT.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::View;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IHashMap;


namespace Elastos {
namespace Droid {
namespace View {

class ViewPropertyAnimator
    : public Object
    , public IViewPropertyAnimator
{
private:
    friend class RenderNodeAnimator;
private:
    class AnimatorEventListener
        : public Object
        , public IAnimatorUpdateListener
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorEventListener(
            /* [in] */ ViewPropertyAnimator* host);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        ViewPropertyAnimator* mHost;
    };

    class PropertyBundle
        : public Object
    {
    public:
        PropertyBundle(
            /* [in] */ Int32 propertyMask,
            /* [in] */ List<AutoPtr<NameValuesHolder> >* nameValuesHolder);

        ~PropertyBundle();

        CARAPI_(Boolean) Cancel(
            /* [in] */ Int32 propertyConstant);

        Int32 mPropertyMask;
        const AutoPtr< List<AutoPtr<NameValuesHolder> > > mNameValuesHolder;
    };

    class InnerStartAnimationRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerStartAnimationRunnable(
            /* [in] */ ViewPropertyAnimator* owner);

        CARAPI Run();

    private:
        ViewPropertyAnimator* mOwner;
    };

    class InnerBuildLayerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerBuildLayerRunnable(
            /* [in] */ ViewPropertyAnimator* owner);

        CARAPI Run();

    private:
        ViewPropertyAnimator* mOwner;
    };

    class InnerSetLayerTypeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSetLayerTypeRunnable(
            /* [in] */ ViewPropertyAnimator* owner,
            /* [in] */ Int32 type);

        CARAPI Run();

    private:
        ViewPropertyAnimator* mOwner;
        Int32 mType;
    };

public:
    CAR_INTERFACE_DECL()

    ViewPropertyAnimator();

    ~ViewPropertyAnimator();

    CARAPI constructor(
        /* [in] */ IView* v);

    CARAPI SetDuration(
        /* [in] */ Int64 duration);

    CARAPI GetDuration(
        /* [out] */ Int64* result);

    CARAPI GetStartDelay(
        /* [out] */ Int64* result);

    CARAPI SetStartDelay(
        /* [in] */ Int64 delay);

    CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* interpolator);

    CARAPI GetInterpolator(
        /* [out] */ ITimeInterpolator** result);

    CARAPI SetListener(
        /* [in] */ IAnimatorListener* listener);

    CARAPI GetListener(
        /* [out] */ IAnimatorListener** result);

    CARAPI SetUpdateListener(
        /* [in] */ IAnimatorUpdateListener* listener);

    CARAPI GetUpdateListener(
        /* [out] */ IAnimatorUpdateListener** result);

    CARAPI Start();

    CARAPI Cancel();

    CARAPI X(
        /* [in] */ Float value);

    CARAPI XBy(
        /* [in] */ Float value);

    CARAPI Y(
        /* [in] */ Float value);

    CARAPI YBy(
        /* [in] */ Float value);

    CARAPI Z(
        /* [in] */ Float value);

    CARAPI ZBy(
        /* [in] */ Float value);

    CARAPI Rotation(
        /* [in] */ Float value);

    CARAPI RotationBy(
        /* [in] */ Float value);

    CARAPI RotationX(
        /* [in] */ Float value);

    CARAPI RotationXBy(
        /* [in] */ Float value);

    CARAPI RotationY(
        /* [in] */ Float value);

    CARAPI RotationYBy(
        /* [in] */ Float value);

    CARAPI TranslationX(
        /* [in] */ Float value);

    CARAPI TranslationXBy(
        /* [in] */ Float value);

    CARAPI TranslationY(
        /* [in] */ Float value);

    CARAPI TranslationYBy(
        /* [in] */ Float value);

    CARAPI TranslationZ(
        /* [in] */ Float value);

    CARAPI TranslationZBy(
        /* [in] */ Float value);

    CARAPI ScaleX(
        /* [in] */ Float value);

    CARAPI ScaleXBy(
        /* [in] */ Float value);

    CARAPI ScaleY(
        /* [in] */ Float value);

    CARAPI ScaleYBy(
        /* [in] */ Float value);

    CARAPI Alpha(
        /* [in] */ Float value);

    CARAPI AlphaBy(
        /* [in] */ Float value);

    CARAPI WithLayer();

    CARAPI WithStartAction(
        /* [in] */ IRunnable* runnable);

    CARAPI WithEndAction(
        /* [in] */ IRunnable* runnable);

    CARAPI HasActions(
        /* [out] */ Boolean* result);

private:
    CARAPI StartAnimation();

    CARAPI AnimateProperty(
        /* [in] */ Int32 constantName,
        /* [in] */ Float toValue);

    CARAPI AnimatePropertyBy(
        /* [in] */ Int32 constantName,
        /* [in] */ Float byValue);

    CARAPI AnimatePropertyBy(
        /* [in] */ Int32 constantName,
        /* [in] */ Float startValue,
        /* [in] */ Float byValue);

    CARAPI SetValue(
        /* [in] */ Int32 propertyConstant,
        /* [in] */ Float value);

    CARAPI_(Float) GetValue(
        /* [in] */ Int32 propertyConstant);

public:
    AutoPtr<IView> mView;
    const static Int32 NONE           = 0x0000;
    const static Int32 TRANSLATION_X  = 0x0001;
    const static Int32 TRANSLATION_Y  = 0x0002;
    const static Int32 TRANSLATION_Z  = 0x0004;
    const static Int32 SCALE_X        = 0x0008;
    const static Int32 SCALE_Y        = 0x0010;
    const static Int32 ROTATION       = 0x0020;
    const static Int32 ROTATION_X     = 0x0040;
    const static Int32 ROTATION_Y     = 0x0080;
    const static Int32 _X              = 0x0100;
    const static Int32 _Y              = 0x0200;
    const static Int32 _Z              = 0x0400;
    const static Int32 ALPHA          = 0x0800;

private:
    friend class ViewPropertyAnimatorRT;

    Int64 mDuration;
    Boolean mDurationSet;
    Int64 mStartDelay;
    Boolean mStartDelaySet;
    AutoPtr<ITimeInterpolator> mInterpolator;
    Boolean mInterpolatorSet;
    AutoPtr<IAnimatorListener> mListener;
    AutoPtr<IAnimatorUpdateListener> mUpdateListener;
    AutoPtr<IValueAnimator> mTempValueAnimator;
    AutoPtr<ViewPropertyAnimatorRT> mRTBackend;
    AutoPtr<AnimatorEventListener> mAnimatorEventListener;
    AutoPtr<List<AutoPtr<NameValuesHolder> > > mPendingAnimations;
    AutoPtr<IRunnable> mPendingSetupAction;
    AutoPtr<IRunnable> mPendingCleanupAction;
    AutoPtr<IRunnable> mPendingOnStartAction;
    AutoPtr<IRunnable> mPendingOnEndAction;

    const static Int32 TRANSFORM_MASK = TRANSLATION_X | TRANSLATION_Y | SCALE_X | SCALE_Y | TRANSLATION_Z |
            ROTATION | ROTATION_X | ROTATION_Y | _X | _Y | _Z;
    AutoPtr<IRunnable> mAnimationStarter;
    AutoPtr<IHashMap> mAnimatorMap;
    AutoPtr<IHashMap> mAnimatorSetupMap;
    AutoPtr<IHashMap> mAnimatorCleanupMap;
    AutoPtr<IHashMap> mAnimatorOnStartMap;
    AutoPtr<IHashMap> mAnimatorOnEndMap;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_VIEWPROPERTYANIMATOR_H__

