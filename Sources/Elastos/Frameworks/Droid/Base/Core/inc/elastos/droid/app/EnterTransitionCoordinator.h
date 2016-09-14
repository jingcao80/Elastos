
#ifndef __ELASTOS_DROID_APP_ENTER_TRANSITION_COORDINATOR_H__
#define __ELASTOS_DROID_APP_ENTER_TRANSITION_COORDINATOR_H__

#include "elastos/droid/app/ActivityTransitionCoordinator.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/transition/Transition.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/utility/etl/List.h>


using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * This ActivityTransitionCoordinator is created by the Activity to manage
 * the enter scene and shared element transfer into the Scene, either during
 * launch of an Activity or returning from a launched Activity.
 */
class EnterTransitionCoordinator
    : public ActivityTransitionCoordinator
    , public IEnterTransitionCoordinator
{
private:
    class DecorViewOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        DecorViewOnPreDrawListener(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IView* decorView);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        IView* mDecorView;
    };

    class DestinationDecorViewOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        DestinationDecorViewOnPreDrawListener(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IView* decorView);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        IView* mDecorView;
    };

    class SharedElementOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        SharedElementOnPreDrawListener(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IArrayMap* sharedElements,
            /* [in] */ IView* sharedElement);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        AutoPtr<IArrayMap> mSharedElements;
        IView* mSharedElement;
    };

    class TakeSharedElementsOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        TakeSharedElementsOnPreDrawListener(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IView* decorView,
            /* [in] */ IBundle* sharedElementState);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        IView* mDecorView;
        AutoPtr<IBundle> mSharedElementState;
    };

    class OnAnimationRunnable
        : public Runnable
    {
    public:
        OnAnimationRunnable(
            /* [in] */ EnterTransitionCoordinator* host);

        CARAPI Run();

        CARAPI ToString(
            /* [out] */ String* str);
    public:
        Int32 mAnimations;
    private:
        EnterTransitionCoordinator* mHost;
    };

    class TransitionRunnable
        : public Runnable
    {
    public:
        TransitionRunnable(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IBundle* sharedElementState);

        CARAPI Run();

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        AutoPtr<IBundle> mSharedElementState;
    };

    class SharedElementTransitionListener
        : public Elastos::Droid::Transition::Transition::TransitionListenerAdapter
    {
    public:
        SharedElementTransitionListener(
            /* [in] */ EnterTransitionCoordinator* host);

        CARAPI OnTransitionStart(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
    };

    class MyContinueTransitionListener
        : public ActivityTransitionCoordinator::ContinueTransitionListener
    {
    public:
        MyContinueTransitionListener(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IArrayList* transitioningViews);

        CARAPI OnTransitionStart(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        AutoPtr<IArrayList> mTransitioningViews;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ EnterTransitionCoordinator* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
    };

    class EnterTransitionListener
        : public Elastos::Droid::Transition::Transition::TransitionListenerAdapter
    {
    public:
        EnterTransitionListener(
            /* [in] */ EnterTransitionCoordinator* host);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
    };

    class RejectedAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        RejectedAnimatorListenerAdapter(
            /* [in] */ EnterTransitionCoordinator* host,
            /* [in] */ IView* decorView,
            /* [in] */ IArrayList* rejectedSnapshots);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
        IView* mDecorView;
        AutoPtr<IArrayList> mRejectedSnapshots;
    };

    class TransitionOnlyRunnable
        : public Runnable
    {
    public:
        TransitionOnlyRunnable(
            /* [in] */ EnterTransitionCoordinator* host);

        CARAPI Run();

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        EnterTransitionCoordinator* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    EnterTransitionCoordinator();

    virtual ~EnterTransitionCoordinator();

    CARAPI constructor(
        /* [in] */ IActivity* activity,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [in] */ IArrayList* sharedElementNames,
        /* [in] */ Boolean isReturning);

    CARAPI ViewInstancesReady(
        /* [in] */ IArrayList* accepted,    //ArrayList<String>
        /* [in] */ IArrayList* localNames,  //ArrayList<String>
        /* [in] */ IArrayList* localViews);  //ArrayList<View>

    CARAPI NamedViewsReady(
        /* [in] */ IArrayList* accepted,    //ArrayList<String>
        /* [in] */ IArrayList* localNames);

    CARAPI GetEnterViewsTransition(
        /* [out] */ ITransition** transition);

    CARAPI IsReturning(
        /* [out] */ Boolean* returning);

    CARAPI Stop();

    CARAPI CancelEnter();

protected:
    //@Override
    CARAPI ViewsReady(
        /* [in] */ IArrayMap* sharedElements); //ArrayMap<String, View>

    //@Override
    void OnReceiveResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* resultData);

    void PrepareEnter();

    //@Override
    AutoPtr<ITransition> GetViewsTransition();

    AutoPtr<ITransition> GetSharedElementTransition();

    void OnRemoteExitTransitionComplete();

    //@Override
    void GetSharedElementParentMatrix(
        /* [in] */ IView* view,
        /* [in] */ IMatrix* matrix);

private:

    CARAPI TriggerViewsReady(
        /* [in] */ IArrayMap* sharedElements); //ArrayMap<String, View>

    AutoPtr<IArrayMap> MapNamedElements( //ArrayMap<String, View>
        /* [in] */ IArrayList* accepted,    //<String>
        /* [in] */ IArrayList* localNames);  //<String>

    void SendSharedElementDestination();

    static AutoPtr<ISharedElementCallback> GetListener(
        /* [in] */ IActivity* activity,
        /* [in] */ Boolean isReturning);

    void Cancel();

    void StartSharedElementTransition(
        /* [in] */ IBundle* sharedElementState);

    void OnTakeSharedElements();

    void RequestLayoutForSharedElements();

    AutoPtr<ITransition> BeginTransition(
        /* [in] */ IViewGroup* decorView,
        /* [in] */ Boolean StartEnterTransition,
        /* [in] */ Boolean StartSharedElementTransition);

    void ViewTransitionComplete();

    void SharedElementTransitionComplete();

    void SharedElementTransitionStarted();

    void StartEnterTransition(
        /* [in] */ ITransition* transition);

    void MakeOpaque();

    Boolean AllowOverlappingTransitions();

    void StartRejectedAnimations(
        /* [in] */ IArrayList* rejectedSnapshots); //ArrayList<View>

    void StartEnterTransitionOnly();

    void SetSharedElementMatrices();

private:
    friend class DecorViewOnPreDrawListener;
    friend class OnAnimationRunnable;

    static const String TAG;
    static const Int32 MIN_ANIMATION_FRAMES;

    Boolean mSharedElementTransitionStarted;
    AutoPtr<IActivity> mActivity;
    Boolean mHasStopped;
    Boolean mIsCanceled;
    AutoPtr<IObjectAnimator> mBackgroundAnimator;
    Boolean mIsExitTransitionComplete;
    Boolean mIsReadyForTransition;
    AutoPtr<IBundle> mSharedElementsBundle;
    Boolean mWasOpaque;
    Boolean mAreViewsReady;
    Boolean mIsViewsTransitionStarted;
    Boolean mIsViewsTransitionComplete;
    Boolean mIsSharedElementTransitionComplete;
    AutoPtr<List<AutoPtr<IMatrix> > > mSharedElementParentMatrices;
    AutoPtr<ITransition> mEnterViewsTransition;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ENTER_TRANSITION_COORDINATOR_H__