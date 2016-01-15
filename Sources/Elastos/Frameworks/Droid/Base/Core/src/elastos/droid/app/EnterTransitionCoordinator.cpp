
#include "elastos/droid/app/EnterTransitionCoordinator.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/utility/CArrayMap.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Transition::ITransitionManager;
using Elastos::Droid::Transition::CTransitionManager;
using Elastos::Droid::Transition::ITransitionListener;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace App {

//============================================================================
// EnterTransitionCoordinator::DecorViewOnPreDrawListener
//============================================================================
CAR_INTERFACE_IMPL(EnterTransitionCoordinator::DecorViewOnPreDrawListener, Object, IOnPreDrawListener)

EnterTransitionCoordinator::DecorViewOnPreDrawListener::DecorViewOnPreDrawListener(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IView* decorView)
    : mHost(host)
    , mDecorView(decorView)
{}

ECode EnterTransitionCoordinator::DecorViewOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mHost->mIsReadyForTransition) {
        AutoPtr<IViewTreeObserver> vto;
        mDecorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
        vto->RemoveOnPreDrawListener(IOnPreDrawListener::Probe(this));
    }
    *result = mHost->mIsReadyForTransition;
    return NOERROR;
}

ECode EnterTransitionCoordinator::DecorViewOnPreDrawListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::DecorViewOnPreDrawListener");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::SharedElementOnPreDrawListener
//============================================================================
CAR_INTERFACE_IMPL(EnterTransitionCoordinator::SharedElementOnPreDrawListener, Object, IOnPreDrawListener)

EnterTransitionCoordinator::SharedElementOnPreDrawListener::SharedElementOnPreDrawListener(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IArrayMap* sharedElements,
    /* [in] */ IView* sharedElement)
    : mHost(host)
    , mSharedElements(sharedElements)
    , mSharedElement(sharedElement)
{}

ECode EnterTransitionCoordinator::SharedElementOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IViewTreeObserver> vto;
    mSharedElement->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(IOnPreDrawListener::Probe(this));
    mHost->ViewsReady(mSharedElements);
    *result = TRUE;
    return NOERROR;
}

ECode EnterTransitionCoordinator::SharedElementOnPreDrawListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::SharedElementOnPreDrawListener");
    return NOERROR;
}


//============================================================================
// EnterTransitionCoordinator::DestinationDecorViewOnPreDrawListener
//============================================================================
CAR_INTERFACE_IMPL(EnterTransitionCoordinator::DestinationDecorViewOnPreDrawListener, Object, IOnPreDrawListener)

EnterTransitionCoordinator::DestinationDecorViewOnPreDrawListener::DestinationDecorViewOnPreDrawListener(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IView* decorView)
    : mHost(host)
    , mDecorView(decorView)
{}

ECode EnterTransitionCoordinator::DestinationDecorViewOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IViewTreeObserver> vto;
    mDecorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(IOnPreDrawListener::Probe(this));

    if (mHost->mResultReceiver != NULL) {
        AutoPtr<IBundle> state = mHost->CaptureSharedElementState();
        mHost->SetSharedElementMatrices();
        mHost->MoveSharedElementsToOverlay();
        mHost->mResultReceiver->Send(
            IActivityTransitionCoordinator::MSG_SHARED_ELEMENT_DESTINATION, state);
    }

    *result = TRUE;
    return NOERROR;
}

ECode EnterTransitionCoordinator::DestinationDecorViewOnPreDrawListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::DestinationDecorViewOnPreDrawListener");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::OnAnimationRunnable
//============================================================================
EnterTransitionCoordinator::OnAnimationRunnable::OnAnimationRunnable(
    /* [in] */ EnterTransitionCoordinator* host)
    : mAnimations(0)
    , mHost(host)
{
}

ECode EnterTransitionCoordinator::OnAnimationRunnable::Run()
{
    if (mAnimations++ < EnterTransitionCoordinator::MIN_ANIMATION_FRAMES) {
        AutoPtr<IViewGroup> vg;
        mHost->GetDecor((IViewGroup**)&vg);
        IView* decorView = IView::Probe(vg);
        if (decorView != NULL) {
            decorView->PostOnAnimation(IRunnable::Probe(this));
        }
    }
    else if (mHost->mResultReceiver != NULL) {
        mHost->mResultReceiver->Send(
            IActivityTransitionCoordinator::MSG_HIDE_SHARED_ELEMENTS, NULL);
        mHost->mResultReceiver = NULL; // all done sending messages.
    }
    return NOERROR;
}

ECode EnterTransitionCoordinator::OnAnimationRunnable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::OnAnimationRunnable");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::TakeSharedElementsOnPreDrawListener
//============================================================================
CAR_INTERFACE_IMPL(EnterTransitionCoordinator::TakeSharedElementsOnPreDrawListener, Object, IOnPreDrawListener)

EnterTransitionCoordinator::TakeSharedElementsOnPreDrawListener::TakeSharedElementsOnPreDrawListener(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IView* decorView,
    /* [in] */ IBundle* sharedElementState)
    : mHost(host)
    , mDecorView(decorView)
    , mSharedElementState(sharedElementState)
{}

ECode EnterTransitionCoordinator::TakeSharedElementsOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IViewTreeObserver> vto;
    mDecorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(IOnPreDrawListener::Probe(this));

    AutoPtr<IRunnable> runnable = new TransitionRunnable(mHost, mSharedElementState);
    mHost->StartTransition(runnable);

    *result = FALSE;
    return NOERROR;
}

ECode EnterTransitionCoordinator::TakeSharedElementsOnPreDrawListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::TakeSharedElementsOnPreDrawListener");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::TransitionRunnable
//============================================================================
EnterTransitionCoordinator::TransitionRunnable::TransitionRunnable(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IBundle* sharedElementState)
    : mHost(host)
    , mSharedElementState(sharedElementState)
{
}

ECode EnterTransitionCoordinator::TransitionRunnable::Run()
{
    mHost->StartSharedElementTransition(mSharedElementState);
    return NOERROR;
}

ECode EnterTransitionCoordinator::TransitionRunnable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::TransitionRunnable");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::SharedElementTransitionListener
//============================================================================
EnterTransitionCoordinator::SharedElementTransitionListener::SharedElementTransitionListener(
    /* [in] */ EnterTransitionCoordinator* host)
{}

ECode EnterTransitionCoordinator::SharedElementTransitionListener::OnTransitionStart(
    /* [in] */ ITransition* transition)
{
    mHost->SharedElementTransitionStarted();
    return NOERROR;
}

ECode EnterTransitionCoordinator::SharedElementTransitionListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    transition->RemoveListener(ITransitionListener::Probe(this));
    mHost->SharedElementTransitionComplete();
    return NOERROR;
}

ECode EnterTransitionCoordinator::SharedElementTransitionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::SharedElementTransitionListener");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::MyContinueTransitionListener
//============================================================================
EnterTransitionCoordinator::MyContinueTransitionListener::MyContinueTransitionListener(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IArrayList* transitioningViews)
    : ContinueTransitionListener(host)
    , mHost(host)
    , mTransitioningViews(transitioningViews)
{}

ECode EnterTransitionCoordinator::MyContinueTransitionListener::OnTransitionStart(
    /* [in] */ ITransition* transition)
{
    mHost->mEnterViewsTransition = transition;
    if (mTransitioningViews != NULL) {
        mHost->ShowViews(mTransitioningViews, FALSE);
    }
    return ActivityTransitionCoordinator::ContinueTransitionListener::OnTransitionStart(transition);
}

ECode EnterTransitionCoordinator::MyContinueTransitionListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    mHost->mEnterViewsTransition = NULL;
    transition->RemoveListener(ITransitionListener::Probe(this));
    mHost->ViewTransitionComplete();
    return ActivityTransitionCoordinator::ContinueTransitionListener::OnTransitionEnd(transition);
}

ECode EnterTransitionCoordinator::MyContinueTransitionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::MyContinueTransitionListener");
    return NOERROR;
}

//========================================================================
// EnterTransitionCoordinator::MyAnimatorListenerAdapter
//========================================================================
EnterTransitionCoordinator::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ EnterTransitionCoordinator* host)
    : mHost(host)
{
}

ECode EnterTransitionCoordinator::MyAnimatorListenerAdapter::OnAnimationEnd(
            /* [in] */ IAnimator* animation)
{
    mHost->MakeOpaque();
    return NOERROR;
}

ECode EnterTransitionCoordinator::MyAnimatorListenerAdapter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::MyAnimatorListenerAdapter");
    return NOERROR;
}

//========================================================================
// EnterTransitionCoordinator::EnterTransitionListener
//========================================================================
EnterTransitionCoordinator::EnterTransitionListener::EnterTransitionListener(
    /* [in] */ EnterTransitionCoordinator* host)
    : mHost(host)
{
}

ECode EnterTransitionCoordinator::EnterTransitionListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    transition->RemoveListener(ITransitionListener::Probe(this));
    mHost->MakeOpaque();
    return NOERROR;
}

ECode EnterTransitionCoordinator::EnterTransitionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::EnterTransitionListener");
    return NOERROR;
}

//========================================================================
// EnterTransitionCoordinator::RejectedAnimatorListenerAdapter
//========================================================================
EnterTransitionCoordinator::RejectedAnimatorListenerAdapter::RejectedAnimatorListenerAdapter(
    /* [in] */ EnterTransitionCoordinator* host,
    /* [in] */ IView* decorView,
    /* [in] */ IArrayList* rejectedSnapshots)
    : mHost(host)
    , mDecorView(decorView)
    , mRejectedSnapshots(rejectedSnapshots)
{}

ECode EnterTransitionCoordinator::RejectedAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IViewOverlay> vo;
    mDecorView->GetOverlay((IViewOverlay**)&vo);
    IViewGroupOverlay* overlay = IViewGroupOverlay::Probe(vo);
    Int32 numRejected;
    mRejectedSnapshots->GetSize(&numRejected);
    for (Int32 i = 0; i < numRejected; i++) {
        AutoPtr<IInterface> obj;
        mRejectedSnapshots->Get(i, (IInterface**)&obj);
        IView* view = IView::Probe(obj);
        overlay->Remove(view);
    }
    return NOERROR;
}

ECode EnterTransitionCoordinator::RejectedAnimatorListenerAdapter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::RejectedAnimatorListenerAdapter");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator::TransitionOnlyRunnable
//============================================================================
EnterTransitionCoordinator::TransitionOnlyRunnable::TransitionOnlyRunnable(
    /* [in] */ EnterTransitionCoordinator* host)
    : mHost(host)
{
}

ECode EnterTransitionCoordinator::TransitionOnlyRunnable::Run()
{
    Boolean startEnterTransition = TRUE;
    Boolean startSharedElementTransition = FALSE;
    AutoPtr<IViewGroup> decorView;
    mHost->GetDecor((IViewGroup**)&decorView);
    if (decorView != NULL) {
        AutoPtr<ITransition> transition = mHost->BeginTransition(
            decorView, startEnterTransition, startSharedElementTransition);
        mHost->StartEnterTransition(transition);
    }
    return NOERROR;
}

ECode EnterTransitionCoordinator::TransitionOnlyRunnable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("EnterTransitionCoordinator::TransitionOnlyRunnable");
    return NOERROR;
}

//============================================================================
// EnterTransitionCoordinator
//============================================================================
const String EnterTransitionCoordinator::TAG("EnterTransitionCoordinator");

const Int32 EnterTransitionCoordinator::MIN_ANIMATION_FRAMES = 2;

CAR_INTERFACE_IMPL(EnterTransitionCoordinator, ActivityTransitionCoordinator, IEnterTransitionCoordinator)

EnterTransitionCoordinator::EnterTransitionCoordinator()
    : mSharedElementTransitionStarted(FALSE)
    , mHasStopped(FALSE)
    , mIsCanceled(FALSE)
    , mIsExitTransitionComplete(FALSE)
    , mIsReadyForTransition(FALSE)
    , mWasOpaque(FALSE)
    , mAreViewsReady(FALSE)
    , mIsViewsTransitionStarted(FALSE)
    , mIsViewsTransitionComplete(FALSE)
    , mIsSharedElementTransitionComplete(FALSE)
{}

EnterTransitionCoordinator::~EnterTransitionCoordinator()
{}

ECode EnterTransitionCoordinator::constructor(
    /* [in] */ IActivity* activity,
    /* [in] */ IResultReceiver* resultReceiver,
    /* [in] */ IArrayList* sharedElementNames,
    /* [in] */ Boolean isReturning)
{
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);

    FAIL_RETURN(ActivityTransitionCoordinator::constructor(
        window, sharedElementNames, GetListener(activity, isReturning), isReturning))
    mActivity = activity;
    SetResultReceiver(resultReceiver);
    PrepareEnter();
    AutoPtr<IBundle> resultReceiverBundle;
    CBundle::New((IBundle**)&resultReceiverBundle);
    resultReceiverBundle->PutParcelable(
        ActivityTransitionCoordinator::KEY_REMOTE_RECEIVER, IParcelable::Probe(this));

    mResultReceiver->Send(
        IActivityTransitionCoordinator::MSG_SET_REMOTE_RECEIVER, resultReceiverBundle);

    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (decorView != NULL) {
        AutoPtr<IViewTreeObserver> vto;
        decorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
        AutoPtr<IOnPreDrawListener> listener = new DecorViewOnPreDrawListener(this, decorView);
        vto->AddOnPreDrawListener(listener);
    }
    return NOERROR;
}

ECode EnterTransitionCoordinator::ViewInstancesReady(
    /* [in] */ IArrayList* accepted,    //ArrayList<String>
    /* [in] */ IArrayList* localNames,  //ArrayList<String>
    /* [in] */ IArrayList* localViews)  //ArrayList<View>
{
    Boolean remap = FALSE;
    Int32 size;
    localViews->GetSize(&size);
    Boolean attached;
    String transitionNmae;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj, localNameObj;
        localViews->Get(i, (IInterface**)&obj);
        IView* view = IView::Probe(obj);
        view->GetTransitionName(&transitionNmae);
        localNames->Get(i, (IInterface**)&localNameObj);
        if (!TextUtils::Equals(transitionNmae, Object::ToString(localNameObj))
                || (view->IsAttachedToWindow(&attached), !attached)) {
            remap = TRUE;
            break;
        }
    }
    if (remap) {
        return TriggerViewsReady(MapNamedElements(accepted, localNames));
    }
    else {
        return TriggerViewsReady(MapSharedElements(accepted, localViews));
    }
}

ECode EnterTransitionCoordinator::NamedViewsReady(
    /* [in] */ IArrayList* accepted,    //ArrayList<String>
    /* [in] */ IArrayList* localNames)
{
    return TriggerViewsReady(MapNamedElements(accepted, localNames));
}

ECode EnterTransitionCoordinator::GetEnterViewsTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    *transition = mEnterViewsTransition;
    REFCOUNT_ADD(*transition)
    return NOERROR;
}

ECode EnterTransitionCoordinator::ViewsReady(
    /* [in] */ IArrayMap* sharedElements) //ArrayMap<String, View>
{
    FAIL_RETURN(ActivityTransitionCoordinator::ViewsReady(sharedElements))
    mIsReadyForTransition = TRUE;
    HideViews(mSharedElements);

    if (GetViewsTransition() != NULL && mTransitioningViews != NULL) {
        HideViews(mTransitioningViews);
    }
    if (mIsReturning) {
        SendSharedElementDestination();
    }
    else {
        SetSharedElementMatrices();
        MoveSharedElementsToOverlay();
    }
    if (mSharedElementsBundle != NULL) {
        OnTakeSharedElements();
    }
    return NOERROR;
}

ECode EnterTransitionCoordinator::TriggerViewsReady(
    /* [in] */ IArrayMap* sharedElements) //ArrayMap<String, View>
{
    if (mAreViewsReady) {
        return NOERROR;
    }

    mAreViewsReady = TRUE;
    // Ensure the views have been laid out before capturing the views -- we need the epicenter.
    Boolean isEmpty;
    sharedElements->IsEmpty(&isEmpty);
    if (isEmpty) {
        ViewsReady(sharedElements);
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    sharedElements->Get(0, (IInterface**)&obj);
    IView* sharedElement = IView::Probe(obj);
    Boolean requested;
    sharedElement->IsLayoutRequested(&requested);

    if (!requested) {
        ViewsReady(sharedElements);
    }
    else {
        AutoPtr<IOnPreDrawListener> listener = new SharedElementOnPreDrawListener(
            this, sharedElements, sharedElement);
        AutoPtr<IViewTreeObserver> vto;
        sharedElement->GetViewTreeObserver((IViewTreeObserver**)&vto);
        vto->AddOnPreDrawListener(listener);
    }

    return NOERROR;
}

AutoPtr<IArrayMap> EnterTransitionCoordinator::MapNamedElements( //ArrayMap<String, View>
    /* [in] */ IArrayList* accepted,    //<String>
    /* [in] */ IArrayList* localNames)  //<String>
{
    AutoPtr<IArrayMap> sharedElements;
    CArrayMap::New((IArrayMap**)&sharedElements);
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (decorView != NULL) {
        decorView->FindNamedViews(IMap::Probe(sharedElements));
    }
    if (accepted != NULL) {
        Int32 size;
        localNames->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> localNameObj, acceptedObj;
            localNames->Get(i, (IInterface**)&localNameObj);
            accepted->Get(i, (IInterface**)&acceptedObj);

            if (localNameObj != NULL && !Object::Equals(localNameObj, acceptedObj)) {
                AutoPtr<IInterface> viewObj;
                sharedElements->Remove(localNameObj, (IInterface**)&viewObj);
                if (viewObj != NULL) {
                    sharedElements->Put(acceptedObj, viewObj);
                }
            }
        }
    }
    return sharedElements;
}

void EnterTransitionCoordinator::SendSharedElementDestination()
{
    Boolean allReady, bval;
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    AutoPtr<ITransition> evt;
    GetEnterViewsTransition((ITransition**)&evt);
    if (AllowOverlappingTransitions() && evt != NULL) {
        allReady = FALSE;
    }
    else if (decorView == NULL) {
        allReady = TRUE;
    }
    else {
        decorView->IsLayoutRequested(&allReady);
        allReady = !allReady;
        if (allReady) {
            Int32 size;
            mSharedElements->GetSize(&size);
            IView* view;
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mSharedElements->Get(i, (IInterface**)&obj);
                view = IView::Probe(obj);
                view->IsLayoutRequested(&bval);
                if (bval) {
                    allReady = FALSE;
                    break;
                }
            }
        }
    }

    if (allReady) {
        AutoPtr<IBundle> state = CaptureSharedElementState();
        SetSharedElementMatrices();
        MoveSharedElementsToOverlay();
        mResultReceiver->Send(
            IActivityTransitionCoordinator::MSG_SHARED_ELEMENT_DESTINATION, state);
    }
    else if (decorView != NULL) {
        AutoPtr<IViewTreeObserver> vto;
        decorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
        AutoPtr<IOnPreDrawListener> listener = new DestinationDecorViewOnPreDrawListener(this, decorView);
        vto->AddOnPreDrawListener(listener);
    }
    if (AllowOverlappingTransitions()) {
        StartEnterTransitionOnly();
    }
}

AutoPtr<ISharedElementCallback> EnterTransitionCoordinator::GetListener(
    /* [in] */ IActivity* activity,
    /* [in] */ Boolean isReturning)
{
    Activity* act = (Activity*)activity;
    return isReturning ? act->mExitTransitionListener : act->mEnterTransitionListener;
}

void EnterTransitionCoordinator::OnReceiveResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData)
{
    switch (resultCode) {
        case IActivityTransitionCoordinator::MSG_TAKE_SHARED_ELEMENTS:
            if (!mIsCanceled) {
                mSharedElementsBundle = resultData;
                OnTakeSharedElements();
            }
            break;
        case IActivityTransitionCoordinator::MSG_EXIT_TRANSITION_COMPLETE:
            if (!mIsCanceled) {
                mIsExitTransitionComplete = TRUE;
                if (mSharedElementTransitionStarted) {
                    OnRemoteExitTransitionComplete();
                }
            }
            break;
        case IActivityTransitionCoordinator::MSG_CANCEL:
            Cancel();
            break;
    }
}

void EnterTransitionCoordinator::Cancel()
{
    if (!mIsCanceled) {
        mIsCanceled = TRUE;
        if (GetViewsTransition() == NULL || mIsViewsTransitionStarted) {
            ShowViews(mSharedElements, TRUE);
        }
        else if (mTransitioningViews != NULL) {
            mTransitioningViews->AddAll(ICollection::Probe(mSharedElements));
        }
        mSharedElementNames->Clear();
        mSharedElements->Clear();
        mAllSharedElementNames->Clear();
        StartSharedElementTransition(NULL);
        OnRemoteExitTransitionComplete();
    }
}

ECode EnterTransitionCoordinator::IsReturning(
    /* [out] */ Boolean* returning)
{
    VALIDATE_NOT_NULL(returning)
    *returning = mIsReturning;
    return NOERROR;
}

void EnterTransitionCoordinator::PrepareEnter()
{
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (mActivity == NULL || decorView == NULL) {
        return;
    }
    mActivity->OverridePendingTransition(0, 0);
    if (!mIsReturning) {
        mActivity->ConvertToTranslucent(NULL, NULL, &mWasOpaque);
        AutoPtr<IDrawable> background;
        decorView->GetBackground((IDrawable**)&background);
        if (background != NULL) {
            AutoPtr<IWindow> window;
            window->SetBackgroundDrawable(NULL);
            AutoPtr<IDrawable> tmp = background;
            background = NULL;
            tmp->Mutate((IDrawable**)&background);
            background->SetAlpha(0);
            window->SetBackgroundDrawable(background);
        }
    }
    else {
        mActivity = NULL; // all done with it now.
    }
}

AutoPtr<ITransition> EnterTransitionCoordinator::GetViewsTransition()
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    if (window == NULL) {
        return NULL;
    }

    AutoPtr<ITransition> transition;
    if (mIsReturning) {
        window->GetReenterTransition((ITransition**)&transition);
    }
    else {
        window->GetEnterTransition((ITransition**)&transition);
    }
    return transition;
}

AutoPtr<ITransition> EnterTransitionCoordinator::GetSharedElementTransition()
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    if (window == NULL) {
        return NULL;
    }

    AutoPtr<ITransition> transition;
    if (mIsReturning) {
        window->GetSharedElementReenterTransition((ITransition**)&transition);
    }
    else {
        window->GetSharedElementEnterTransition((ITransition**)&transition);
    }
    return transition;
}

void EnterTransitionCoordinator::StartSharedElementTransition(
    /* [in] */ IBundle* sharedElementState)
{
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (decorView == NULL) {
        return;
    }
    // Remove rejected shared elements
    AutoPtr<IArrayList> rejectedNames;
    CArrayList::New(ICollection::Probe(mAllSharedElementNames), (IArrayList**)&rejectedNames);
    rejectedNames->RemoveAll(ICollection::Probe(mSharedElementNames));
    AutoPtr<IArrayList> rejectedSnapshots = CreateSnapshots(
        sharedElementState, ICollection::Probe(rejectedNames));
    if (mListener != NULL) {
        mListener->OnRejectSharedElements(IList::Probe(rejectedSnapshots));
    }
    StartRejectedAnimations(rejectedSnapshots);

    // Now start shared element transition
    AutoPtr<IArrayList> sharedElementSnapshots = CreateSnapshots(
        sharedElementState, ICollection::Probe(mSharedElementNames));
    ShowViews(mSharedElements, TRUE);
    ScheduleSetSharedElementEnd(sharedElementSnapshots);
    AutoPtr<IArrayList> originalImageViewState = SetSharedElementState(
        sharedElementState, sharedElementSnapshots);
    RequestLayoutForSharedElements();

    Boolean startEnterTransition = AllowOverlappingTransitions() && !mIsReturning;
    Boolean startSharedElementTransition = TRUE;
    SetGhostVisibility(IView::INVISIBLE);
    ScheduleGhostVisibilityChange(IView::INVISIBLE);
    AutoPtr<ITransition> transition = BeginTransition(
        vg, startEnterTransition, startSharedElementTransition);
    ScheduleGhostVisibilityChange(IView::VISIBLE);
    SetGhostVisibility(IView::VISIBLE);

    if (startEnterTransition) {
        StartEnterTransition(transition);
    }

    SetOriginalSharedElementState(mSharedElements, originalImageViewState);

    if (mResultReceiver != NULL) {
        // We can't trust that the view will disappear on the same frame that the shared
        // element appears here. Assure that we get at least 2 frames for double-buffering.
        AutoPtr<IRunnable> runnable = new OnAnimationRunnable(this);
        decorView->PostOnAnimation(runnable);
    }
}

void EnterTransitionCoordinator::OnTakeSharedElements()
{
    if (!mIsReadyForTransition || mSharedElementsBundle == NULL) {
        return;
    }
    AutoPtr<IBundle> sharedElementState = mSharedElementsBundle;
    mSharedElementsBundle = NULL;
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (decorView != NULL) {
        AutoPtr<IViewTreeObserver> vto;
        decorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
        AutoPtr<IOnPreDrawListener> listener = new TakeSharedElementsOnPreDrawListener(
            this, decorView, sharedElementState);
        vto->AddOnPreDrawListener(listener);
        decorView->Invalidate();
    }
}

void EnterTransitionCoordinator::RequestLayoutForSharedElements()
{
    Int32 numSharedElements;
    mSharedElements->GetSize(&numSharedElements);
    for (Int32 i = 0; i < numSharedElements; i++) {
        AutoPtr<IInterface> obj;
        mSharedElements->Get(i, (IInterface**)&obj);
        IView::Probe(obj)->RequestLayout();
    }
}

AutoPtr<ITransition> EnterTransitionCoordinator::BeginTransition(
    /* [in] */ IViewGroup* decorView,
    /* [in] */ Boolean startEnterTransition,
    /* [in] */ Boolean startSharedElementTransition)
{
    AutoPtr<ITransition> sharedElementTransition = NULL;
    if (startSharedElementTransition) {
        Boolean isEmpty;
        mSharedElementNames->IsEmpty(&isEmpty);
        if (!isEmpty) {
            sharedElementTransition = ConfigureTransition(GetSharedElementTransition(), FALSE);
        }
        if (sharedElementTransition == NULL) {
            SharedElementTransitionStarted();
            SharedElementTransitionComplete();
        }
        else {
            AutoPtr<ITransitionListener> listener = new SharedElementTransitionListener(this);
            sharedElementTransition->AddListener(listener);
        }
    }

    AutoPtr<ITransition> viewsTransition;
    if (startEnterTransition) {
        mIsViewsTransitionStarted = TRUE;
        Boolean bval;
        if (mTransitioningViews != NULL && (mTransitioningViews->IsEmpty(&bval), !bval)) {
            viewsTransition = ConfigureTransition(GetViewsTransition(), TRUE);
            if (viewsTransition != NULL && !mIsReturning) {
                StripOffscreenViews();
            }
        }
        if (viewsTransition == NULL) {
            ViewTransitionComplete();
        }
        else {
            viewsTransition->ForceVisibility(IView::INVISIBLE, TRUE);
            AutoPtr<IArrayList> transitioningViews = mTransitioningViews;
            AutoPtr<ITransitionListener> listener = new MyContinueTransitionListener(this, transitioningViews);
            viewsTransition->AddListener(listener);
        }
    }

    AutoPtr<ITransition> transition = MergeTransitions(sharedElementTransition, viewsTransition);
    if (transition != NULL) {
        AutoPtr<ITransitionListener> listener = new ActivityTransitionCoordinator::ContinueTransitionListener(this);
        transition->AddListener(listener);
        CTransitionManager::BeginDelayedTransition(decorView, transition);
        Boolean bval;
        if (startSharedElementTransition && (mSharedElementNames->IsEmpty(&bval), !bval)) {
            AutoPtr<IInterface> obj;
            mSharedElements->Get(0, (IInterface**)&obj);
            IView::Probe(obj)->Invalidate();
        }
        else if (startEnterTransition && mTransitioningViews != NULL
            && (mTransitioningViews->IsEmpty(&bval), !bval)) {
            AutoPtr<IInterface> obj;
            mTransitioningViews->Get(0, (IInterface**)&obj);
            IView::Probe(obj)->Invalidate();
        }
    } else {
        TransitionStarted();
    }
    return transition;
}

void EnterTransitionCoordinator::ViewTransitionComplete()
{
    mIsViewsTransitionComplete = TRUE;
    if (mIsSharedElementTransitionComplete) {
        MoveSharedElementsFromOverlay();
    }
}

void EnterTransitionCoordinator::SharedElementTransitionComplete()
{
    mIsSharedElementTransitionComplete = TRUE;
    if (mIsViewsTransitionComplete) {
        MoveSharedElementsFromOverlay();
    }
}

void EnterTransitionCoordinator::SharedElementTransitionStarted()
{
    mSharedElementTransitionStarted = TRUE;
    if (mIsExitTransitionComplete) {
        Send(IActivityTransitionCoordinator::MSG_EXIT_TRANSITION_COMPLETE, NULL);
    }
}

void EnterTransitionCoordinator::StartEnterTransition(
    /* [in] */ ITransition* transition)
{
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (!mIsReturning && decorView != NULL) {
        AutoPtr<IDrawable> background;
        decorView->GetBackground((IDrawable**)&background);
        if (background != NULL) {
            AutoPtr<IDrawable> tmp = background;
            background = NULL;
            tmp->Mutate((IDrawable**)&background);
            AutoPtr<IWindow> window;
            GetWindow((IWindow**)&window);
            window->SetBackgroundDrawable(background);
            AutoPtr<ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(1);
            args->Set(0, 255);
            mBackgroundAnimator = ObjectAnimator::OfInt32(
                IInterface::Probe(background), String("alpha"), args);
            IAnimator* va = IAnimator::Probe(mBackgroundAnimator);
            va->SetDuration(GetFadeDuration());
            AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter(this);
            va->AddListener(listener);
            va->Start();
        }
        else if (transition != NULL) {
            AutoPtr<ITransitionListener> listener = new EnterTransitionListener(this);
            transition->AddListener(listener);
        }
        else {
            MakeOpaque();
        }
    }
}

ECode EnterTransitionCoordinator::Stop()
{
    // Restore the background to its previous state since the
    // Activity is stopping.
    if (mBackgroundAnimator != NULL) {
        IAnimator* va = IAnimator::Probe(mBackgroundAnimator);
        va->End();
        mBackgroundAnimator = NULL;
    }
    else if (mWasOpaque) {
        AutoPtr<IViewGroup> vg;
        GetDecor((IViewGroup**)&vg);
        IView* decorView = IView::Probe(vg);
        if (decorView != NULL) {
            AutoPtr<IDrawable> drawable;
            decorView->GetBackground((IDrawable**)&drawable);
            if (drawable != NULL) {
                drawable->SetAlpha(1);
            }
        }
    }
    MakeOpaque();
    mIsCanceled = TRUE;
    mResultReceiver = NULL;
    mActivity = NULL;
    MoveSharedElementsFromOverlay();
    if (mTransitioningViews != NULL) {
        ShowViews(mTransitioningViews, TRUE);
    }
    ShowViews(mSharedElements, TRUE);
    ClearState();
    return NOERROR;
}

ECode EnterTransitionCoordinator::CancelEnter()
{
    SetGhostVisibility(IView::INVISIBLE);
    mHasStopped = TRUE;
    mIsCanceled = TRUE;
    mResultReceiver = NULL;
    if (mBackgroundAnimator != NULL) {
        IAnimator* va = IAnimator::Probe(mBackgroundAnimator);
        va->Cancel();
        mBackgroundAnimator = NULL;
    }
    mActivity = NULL;
    ClearState();
    return NOERROR;
}

void EnterTransitionCoordinator::MakeOpaque()
{
    if (!mHasStopped && mActivity != NULL) {
        if (mWasOpaque) {
            mActivity->ConvertFromTranslucent();
        }
        mActivity = NULL;
    }
}

Boolean EnterTransitionCoordinator::AllowOverlappingTransitions()
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    Boolean bval;
    assert(0 && "TODO");
    // if (mIsReturning)
    //     window->GetAllowExitTransitionOverlap(&bval);
    // else
    //     window->GetAllowEnterTransitionOverlap(&bval);
    return NOERROR;
}

void EnterTransitionCoordinator::StartRejectedAnimations(
    /* [in] */ IArrayList* rejectedSnapshots) //ArrayList<View>
{
    Boolean bval;
    if (rejectedSnapshots == NULL || (rejectedSnapshots->IsEmpty(&bval), bval)) {
        return;
    }
    AutoPtr<IViewGroup> vg;
    GetDecor((IViewGroup**)&vg);
    IView* decorView = IView::Probe(vg);
    if (decorView != NULL) {
        AutoPtr<IViewOverlay> vo;
        decorView->GetOverlay((IViewOverlay**)&vo);
        IViewGroupOverlay* overlay = IViewGroupOverlay::Probe(vo);
        AutoPtr<IObjectAnimator> animator;
        Int32 numRejected;
        rejectedSnapshots->GetSize(&numRejected);
        AutoPtr<ArrayOf<Float> > args = ArrayOf<Float>::Alloc(2);
        args->Set(0, 0);
        args->Set(1, 0);
        for (Int32 i = 0; i < numRejected; i++) {
            AutoPtr<IInterface> obj;
            rejectedSnapshots->Get(i, (IInterface**)&obj);
            IView* snapshot = IView::Probe(obj);
            overlay->Add(snapshot);
            animator = ObjectAnimator::OfFloat(
                snapshot, Elastos::Droid::View::View::ALPHA, args);
            IAnimator::Probe(animator)->Start();
        }

        AutoPtr<IAnimatorListener> listener = new RejectedAnimatorListenerAdapter(
            this, decorView, rejectedSnapshots);
        IAnimator::Probe(animator)->AddListener(listener);
    }
}

void EnterTransitionCoordinator::OnRemoteExitTransitionComplete()
{
    if (!AllowOverlappingTransitions()) {
        StartEnterTransitionOnly();
    }
}

void EnterTransitionCoordinator::StartEnterTransitionOnly()
{
    AutoPtr<IRunnable> runnable = new TransitionOnlyRunnable(this);
    StartTransition(runnable);
}

void EnterTransitionCoordinator::SetSharedElementMatrices()
{
    Int32 numSharedElements;
    mSharedElements->GetSize(&numSharedElements);
    if (numSharedElements > 0) {
        mSharedElementParentMatrices = new List<AutoPtr<IMatrix> >(numSharedElements);
    }
    for (Int32 i = 0; i < numSharedElements; i++) {
        AutoPtr<IInterface> obj;
        mSharedElements->Get(i, (IInterface**)&obj);
        IView* view = IView::Probe(obj);

        // Find the location in the view's parent
        AutoPtr<IViewParent> vp;
        view->GetParent((IViewParent**)&vp);
        IView* parent = IView::Probe(vp);
        AutoPtr<IMatrix> matrix;
        CMatrix::New((IMatrix**)&matrix);
        parent->TransformMatrixToLocal(matrix);

        mSharedElementParentMatrices->PushBack(matrix);
    }
}

void EnterTransitionCoordinator::GetSharedElementParentMatrix(
    /* [in] */ IView* view,
    /* [in] */ IMatrix* matrix)
{
    Int32 index = -1;
    if (mSharedElementParentMatrices != NULL)
        mSharedElements->IndexOf(IInterface::Probe(view), &index);
    if (index < 0) {
        ActivityTransitionCoordinator::GetSharedElementParentMatrix(view, matrix);
    }
    else {
        matrix->Set((*mSharedElementParentMatrices)[index]);
    }
}



} // namespace App
} // namespace Droid
} // namespace Elastos