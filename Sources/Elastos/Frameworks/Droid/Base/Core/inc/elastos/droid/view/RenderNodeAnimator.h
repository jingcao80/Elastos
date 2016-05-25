#ifndef __ELASTOS_DROID_VIEW_RENDERNODEANIMATOR_H__
#define __ELASTOS_DROID_VIEW_RENDERNODEANIMATOR_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/Animator.h"
#include "elastos/droid/graphics/CanvasProperty.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <utils/RefBase.h>

using Elastos::Droid::Graphics::CanvasProperty;
using Elastos::Droid::Graphics::ICanvasProperty;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Animation::Animator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

using android::VirtualLightRefBase;

namespace Elastos {
namespace Droid {
namespace View {

class VirtualRefBasePtr
    : public Object
{
public:
    VirtualRefBasePtr(Int64 ptr) {
        mNativePtr = ptr;
        nIncStrong(mNativePtr);
    }
    ~VirtualRefBasePtr() {
        release();
    }

    Int64 get() {
        return mNativePtr;
    }

    void release() {
        if (mNativePtr != 0) {
            nDecStrong(mNativePtr);
            mNativePtr = 0;
        }
    }

private:
    void nIncStrong(Int64 ptr)
    {
        VirtualLightRefBase* obj = reinterpret_cast<VirtualLightRefBase*>(ptr);
        obj->incStrong(0);
    }

    void nDecStrong(Int64 ptr)
    {
        VirtualLightRefBase* obj = reinterpret_cast<VirtualLightRefBase*>(ptr);
        obj->decStrong(0);
    }

private:
    Int64 mNativePtr;
};



class RenderNodeAnimator
    : public Animator
    , public IRenderNodeAnimator
{
private:
    friend void ThreadDestructor(void* st);
    friend Boolean InitTLS();
    friend class AnimationListenerBridge;

    class DelayedAnimationHelper
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        DelayedAnimationHelper();

        CARAPI AddDelayedAnimation(
            /* [in] */ IRenderNodeAnimator* animator);

        CARAPI RemoveDelayedAnimation(
            /* [in] */ IRenderNodeAnimator* animator);

        CARAPI Run();

    private:
        CARAPI ScheduleCallback();

        List<AutoPtr<IRenderNodeAnimator> > mDelayedAnims;
        AutoPtr<IChoreographer> mChoreographer;
        Boolean mCallbackScheduled;
    };

public:
    CAR_INTERFACE_DECL()

    RenderNodeAnimator();

    CARAPI constructor(
        /* [in] */ Int32 property,
        /* [in] */ Float finalValue);

    CARAPI constructor(
        /* [in] */ ICanvasProperty* property,
        /* [in] */ Float finalValue);

    /**
     * Creates a new render node animator for a field on a Paint property.
     *
     * @param property The paint property to target
     * @param paintField Paint field to animate, one of {@link #PAINT_ALPHA} or
     *            {@link #PAINT_STROKE_WIDTH}
     * @param finalValue The target value for the property
     */
    CARAPI constructor(
        /* [in] */ ICanvasProperty* property,
        /* [in] */ Int32 paintField,
        /* [in] */ Float finalValue);

    CARAPI constructor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Float startRadius,
        /* [in] */ Float endRadius);

    CARAPI Start();

    CARAPI Cancel();

    CARAPI End();

    CARAPI Pause();

    CARAPI Resume();

    CARAPI SetTarget(
        /* [in] */ IView* view);

    CARAPI SetTarget(
        /* [in] */ ICanvas* canvas);

    CARAPI SetStartValue(
        /* [in] */ Float startValue);

    CARAPI SetStartDelay(
        /* [in] */ Int64 startDelay);

    CARAPI GetStartDelay(
        /* [out] */ Int64* startDelay);

    CARAPI SetDuration(
        /* [in] */ Int64 duration);

    CARAPI GetDuration(
        /* [out] */ Int64* duration);

    CARAPI IsRunning(
        /* [out] */ Boolean* result);

    CARAPI IsStarted(
        /* [out] */ Boolean* result);

    CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* interpolator);

    CARAPI GetInterpolator(
        /* [out] */ ITimeInterpolator** interpolator);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI SetAllowRunningAsynchronously(
        /* [in] */ Boolean mayRunAsync);

    CARAPI GetNativeAnimator(
        /* [out] */ Int64* ptr);

    static CARAPI_(Int32) MapViewPropertyToRenderProperty(
        /* [in] */ Int32 viewProperty);

    static CARAPI_(Boolean) InitHashMap();

    static CARAPI_(Boolean) IsNativeInterpolator(
        /* [in] */ ITimeInterpolator* interpolator);

protected:

    CARAPI_(void) OnFinished();

    CARAPI_(Int64) GetNativeAnimator();

private:
    CARAPI_(void) Init(
        /* [in] */ Int64 ptr);

    CARAPI CheckMutable();

    CARAPI_(void) ApplyInterpolator();

    CARAPI_(void) DoStart();

    CARAPI_(void) NotifyStartListeners();

    CARAPI SetTarget(
        /* [in] */ IRenderNode* node);

    CARAPI_(AutoPtr<ArrayOf<IAnimatorListener*> >) CloneListeners();

    CARAPI_(Boolean) ProcessDelayed(
        /* [in] */ Int64 frameTimeMs);

    static CARAPI_(void) CallOnFinished(
        /* [in] */ IRenderNodeAnimator* animator);

    static CARAPI_(Int64) nCreateAnimator(
        /* [in] */ Int32 property,
        /* [in] */ Float finalValue);

    static CARAPI_(Int64) nCreateCanvasPropertyFloatAnimator(
        /* [in] */ Int64 canvasProperty,
        /* [in] */ Float finalValue);

    static CARAPI_(Int64) nCreateCanvasPropertyPaintAnimator(
        /* [in] */ Int64 canvasProperty,
        /* [in] */ Int32 paintField,
        /* [in] */ Float finalValue);

    static CARAPI_(Int64) nCreateRevealAnimator(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Float startRadius,
        /* [in] */ Float endRadius);

    static CARAPI_(void) nSetStartValue(
        /* [in] */ Int64 nativePtr,
        /* [in] */ Float startValue);

    static CARAPI_(void) nSetDuration(
        /* [in] */ Int64 nativePtr,
        /* [in] */ Int64 duration);

    static CARAPI_(Int64) nGetDuration(
        /* [in] */ Int64 nativePtr);

    static CARAPI_(void) nSetStartDelay(
        /* [in] */ Int64 nativePtr,
        /* [in] */ Int64 startDelay);

    static CARAPI_(void) nSetInterpolator(
        /* [in] */ Int64 animPtr,
        /* [in] */ Int64 interpolatorPtr);

    static CARAPI_(void) nSetAllowRunningAsync(
        /* [in] */ Int64 animPtr,
        /* [in] */ Boolean mayRunAsync);

    static CARAPI_(void) nStart(
        /* [in] */ Int64 animPtr,
        /* [in] */ IRenderNodeAnimator* finishListener);

    static CARAPI_(void) nEnd(
        /* [in] */ Int64 animPtr);

    static CARAPI_(AutoPtr<DelayedAnimationHelper>) GetHelper();

private:
    AutoPtr<VirtualRefBasePtr> mNativePtr;

    AutoPtr<IRenderNode> mTarget;
    AutoPtr<IView> mViewTarget;
    Int32 mRenderProperty;// = -1;
    Float mFinalValue;
    AutoPtr<ITimeInterpolator> mInterpolator;

    Int32 mState;// = STATE_PREPARE;

    Float mUnscaledDuration;// = 300;
    Float mUnscaledStartDelay;// = 0;
    // If this is true, we will run any start delays on the UI thread. This is
    // the safe default, and is necessary to ensure start listeners fire at
    // the correct time. Animators created by RippleDrawable (the
    // CanvasProperty<> ones) do not have this expectation, and as such will
    // set this to false so that the renderthread handles the startdelay instead
    Boolean mUiThreadHandlesDelay;
    Float mStartDelay;
    Float mStartTime;

    static const Int32 STATE_PREPARE = 0;
    static const Int32 STATE_DELAYED = 1;
    static const Int32 STATE_RUNNING = 2;
    static const Int32 STATE_FINISHED = 3;

    static HashMap<Int32, Int32> sViewPropertyAnimatorMap;

    static pthread_key_t sAnimationHelper;
    static Boolean sHaveKey;
    static Boolean sHasInitMap;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif
