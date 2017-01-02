//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/ExitTransitionCoordinator.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Transition::ITransitionManager;
using Elastos::Droid::Transition::CTransitionManager;
using Elastos::Droid::Transition::ITransitionListener;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace App {

//===================================================================================
// ExitTransitionCoordinator::SharedElementExitRunnable
//===================================================================================
ExitTransitionCoordinator::SharedElementExitRunnable::SharedElementExitRunnable(
    /* [in] */ ExitTransitionCoordinator* host,
    /* [in] */ IViewGroup* decorView)
    : mHost(host)
    , mDecorView(decorView)
{}

ECode ExitTransitionCoordinator::SharedElementExitRunnable::Run()
{
    mHost->StartSharedElementExit(mDecorView);
    return NOERROR;
}

ECode ExitTransitionCoordinator::SharedElementExitRunnable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::SharedElementExitRunnable");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::SharedElementExitTransitionListener
//===================================================================================

ExitTransitionCoordinator::SharedElementExitTransitionListener::SharedElementExitTransitionListener(
    /* [in] */ ExitTransitionCoordinator* host)
    : mHost(host)
{}

ECode ExitTransitionCoordinator::SharedElementExitTransitionListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    transition->RemoveListener(this);
    if (mHost->mExitComplete) {
        mHost->DelayCancel();
    }
    return NOERROR;
}

ECode ExitTransitionCoordinator::SharedElementExitTransitionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::SharedElementExitTransitionListener");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::BeginTransitionRunnable
//===================================================================================
ExitTransitionCoordinator::BeginTransitionRunnable::BeginTransitionRunnable(
    /* [in] */ ExitTransitionCoordinator* host)
    : mHost(host)
{
}

ECode ExitTransitionCoordinator::BeginTransitionRunnable::Run()
{
    mHost->BeginTransitions();
    return NOERROR;
}

ECode ExitTransitionCoordinator::BeginTransitionRunnable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::BeginTransitionRunnable");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::StartExitTransitionRunnable
//===================================================================================
ExitTransitionCoordinator::StartExitTransitionRunnable::StartExitTransitionRunnable(
    /* [in] */ ExitTransitionCoordinator* host)
    : mHost(host)
{
}

ECode ExitTransitionCoordinator::StartExitTransitionRunnable::Run()
{
    mHost->StartExitTransition();
    return NOERROR;
}

ECode ExitTransitionCoordinator::StartExitTransitionRunnable::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::StartExitTransitionRunnable");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::TranslucentConversionListener
//===================================================================================
CAR_INTERFACE_IMPL(ExitTransitionCoordinator::TranslucentConversionListener, Object, ITranslucentConversionListener)

ExitTransitionCoordinator::TranslucentConversionListener::TranslucentConversionListener(
    /* [in] */ ExitTransitionCoordinator* host)
    : mHost(host)
{}

ECode ExitTransitionCoordinator::TranslucentConversionListener::OnTranslucentConversionComplete(
    /* [in] */ Boolean drawComplete)
{
    if (!mHost->mIsCanceled) {
        mHost->FadeOutBackground();
    }
    return NOERROR;
}

ECode ExitTransitionCoordinator::TranslucentConversionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::TranslucentConversionListener");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::SharedElementExitOnPreDrawListener
//===================================================================================
CAR_INTERFACE_IMPL(ExitTransitionCoordinator::SharedElementExitOnPreDrawListener, Object, IOnPreDrawListener)

ExitTransitionCoordinator::SharedElementExitOnPreDrawListener::SharedElementExitOnPreDrawListener(
    /* [in] */ ExitTransitionCoordinator* host,
    /* [in] */ IArrayList* sharedElementSnapshots,
    /* [in] */ IView* decorView)
    : mHost(host)
    , mSharedElementSnapshots(sharedElementSnapshots)
    , mDecorView(decorView)
{}

ECode ExitTransitionCoordinator::SharedElementExitOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IViewTreeObserver> vto;
    mDecorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);
    mHost->SetSharedElementState(mHost->mExitSharedElementBundle, mSharedElementSnapshots);
    *result = TRUE;
    return NOERROR;
}

ECode ExitTransitionCoordinator::SharedElementExitOnPreDrawListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::SharedElementExitOnPreDrawListener");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::ExitContinueTransitionListener
//===================================================================================

ExitTransitionCoordinator::ExitContinueTransitionListener::ExitContinueTransitionListener(
    /* [in] */ ExitTransitionCoordinator* host,
    /* [in] */ IArrayList* transitioningViews)
    : ActivityTransitionCoordinator::ContinueTransitionListener((ActivityTransitionCoordinator*)host)
    , mHost(host)
    , mTransitioningViews(transitioningViews)
{
}

ECode ExitTransitionCoordinator::ExitContinueTransitionListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    transition->RemoveListener(this);
    mHost->ExitTransitionComplete();
    if (mHost->mIsHidden && mTransitioningViews != NULL) {
        mHost->ShowViews(mTransitioningViews, TRUE);
    }
    if (mHost->mSharedElementBundle != NULL) {
        mHost->DelayCancel();
    }
    return ActivityTransitionCoordinator::ContinueTransitionListener::OnTransitionEnd(transition);
}

ECode ExitTransitionCoordinator::ExitContinueTransitionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::ExitContinueTransitionListener");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator::SharedElementContinueTransitionListener
//===================================================================================
ExitTransitionCoordinator::SharedElementContinueTransitionListener::SharedElementContinueTransitionListener(
    /* [in] */ ExitTransitionCoordinator* host)
    : ActivityTransitionCoordinator::ContinueTransitionListener((ActivityTransitionCoordinator*)host)
    , mHost(host)
{}

ECode ExitTransitionCoordinator::SharedElementContinueTransitionListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    transition->RemoveListener(this);
    mHost->SharedElementTransitionComplete();
    if (mHost->mIsHidden) {
        mHost->ShowViews(mHost->mSharedElements, TRUE);
    }
    return NOERROR;
}

ECode ExitTransitionCoordinator::SharedElementContinueTransitionListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::SharedElementContinueTransitionListener");
    return NOERROR;
}

//===================================================================================
// ExitTransitionCoordinator
//===================================================================================
const String ExitTransitionCoordinator::TAG("ExitTransitionCoordinator");
const Int64 ExitTransitionCoordinator::MAX_WAIT_MS = 1000;

CAR_INTERFACE_IMPL(ExitTransitionCoordinator, ActivityTransitionCoordinator, IExitTransitionCoordinator)

ExitTransitionCoordinator::ExitTransitionCoordinator()
    : mExitComplete(FALSE)
    , mExitNotified(FALSE)
    , mSharedElementNotified(FALSE)
    , mIsBackgroundReady(FALSE)
    , mIsCanceled(FALSE)
    , mIsHidden(FALSE)
    , mIsExitStarted(FALSE)
    , mSharedElementsHidden(FALSE)
{}

ExitTransitionCoordinator::~ExitTransitionCoordinator()
{}

ECode ExitTransitionCoordinator::constructor(
    /* [in] */ IActivity* activity,
    /* [in] */ IArrayList* names, //ArrayList<String>
    /* [in] */ IArrayList* accepted, //ArrayList<String>
    /* [in] */ IArrayList* mapped,//ArrayList<View>
    /* [in] */ Boolean isReturning)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    FAIL_RETURN(ActivityTransitionCoordinator::constructor(
        window, names, GetListener(activity, isReturning), isReturning))
    ViewsReady(MapSharedElements(accepted, mapped));
    StripOffscreenViews();
    mIsBackgroundReady = !isReturning;
    mActivity = activity;
    return NOERROR;
}

AutoPtr<ISharedElementCallback> ExitTransitionCoordinator::GetListener(
    /* [in] */ IActivity* activity,
    /* [in] */ Boolean isReturning)
{
    Activity* act = (Activity*)activity;
    return isReturning ? act->mEnterTransitionListener : act->mExitTransitionListener;
}

void ExitTransitionCoordinator::OnReceiveResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData)
{
    switch (resultCode) {
        case IActivityTransitionCoordinator::MSG_SET_REMOTE_RECEIVER: {
            StopCancel();
            AutoPtr<IParcelable> p;
            resultData->GetParcelable(KEY_REMOTE_RECEIVER, (IParcelable**)&p);
            mResultReceiver = IResultReceiver::Probe(p);
            if (mIsCanceled) {
                mResultReceiver->Send(IActivityTransitionCoordinator::MSG_CANCEL, NULL);
                mResultReceiver = NULL;
            }
            else {
                NotifyComplete();
            }
            break;
        }
        case IActivityTransitionCoordinator::MSG_HIDE_SHARED_ELEMENTS:
            StopCancel();
            if (!mIsCanceled) {
                HideSharedElements();
            }
            break;
        case IActivityTransitionCoordinator::MSG_START_EXIT_TRANSITION:
            mHandler->RemoveMessages(IActivityTransitionCoordinator::MSG_CANCEL);
            StartExit();
            break;
        case IActivityTransitionCoordinator::MSG_SHARED_ELEMENT_DESTINATION:
            mExitSharedElementBundle = resultData;
            SharedElementExitBack();
            break;
    }
}

void ExitTransitionCoordinator::StopCancel()
{
    if (mHandler != NULL) {
        mHandler->RemoveMessages(IActivityTransitionCoordinator::MSG_CANCEL);
    }
}

void ExitTransitionCoordinator::DelayCancel()
{
    if (mHandler != NULL) {
        Boolean bval;
        mHandler->SendEmptyMessageDelayed(
            IActivityTransitionCoordinator::MSG_CANCEL, MAX_WAIT_MS, &bval);
    }
}

ECode ExitTransitionCoordinator::ResetViews()
{
    if (mTransitioningViews != NULL) {
        ShowViews(mTransitioningViews, TRUE);
    }
    ShowViews(mSharedElements, TRUE);
    mIsHidden = TRUE;
    AutoPtr<IViewGroup> decorView;
    GetDecor((IViewGroup**)&decorView);
    if (!mIsReturning && decorView != NULL) {
        decorView->SuppressLayout(FALSE);
    }
    MoveSharedElementsFromOverlay();
    ClearState();
    return NOERROR;
}

void ExitTransitionCoordinator::SharedElementExitBack()
{
    AutoPtr<IViewGroup> decorView;
    GetDecor((IViewGroup**)&decorView);
    if (decorView != NULL) {
        decorView->SuppressLayout(TRUE);
    }
    Boolean isEmpty, bval;
    mSharedElements->IsEmpty(&isEmpty);
    if (decorView != NULL && mExitSharedElementBundle != NULL
        && (mExitSharedElementBundle->IsEmpty(&bval), !bval)
        && !isEmpty && GetSharedElementTransition() != NULL) {
        AutoPtr<IRunnable> runnable = new SharedElementExitRunnable(this, decorView);
        StartTransition(runnable);
    } else {
        SharedElementTransitionComplete();
    }
}

void ExitTransitionCoordinator::StartSharedElementExit(
    /* [in] */ IViewGroup* vgDecorView)
{
    IView* decorView = IView::Probe(vgDecorView);
    AutoPtr<ITransition> transition = GetSharedElementExitTransition();
    AutoPtr<ITransitionListener> listener = new SharedElementExitTransitionListener(this);
    transition->AddListener(listener);

    AutoPtr<IArrayList> sharedElementSnapshots = CreateSnapshots(
        mExitSharedElementBundle, ICollection::Probe(mSharedElementNames));
    AutoPtr<IViewTreeObserver> vto;
    decorView->GetViewTreeObserver((IViewTreeObserver**)&vto);
    AutoPtr<IOnPreDrawListener> onPreDrawlistener = new SharedElementExitOnPreDrawListener(this, sharedElementSnapshots, decorView);
    vto->AddOnPreDrawListener(onPreDrawlistener);

    SetGhostVisibility(IView::INVISIBLE);
    ScheduleGhostVisibilityChange(IView::INVISIBLE);
    if (mListener != NULL) {
        mListener->OnSharedElementEnd(
            IList::Probe(mSharedElementNames),
            IList::Probe(mSharedElements),
            IList::Probe(sharedElementSnapshots));
    }
    CTransitionManager::BeginDelayedTransition(vgDecorView, transition);
    ScheduleGhostVisibilityChange(IView::VISIBLE);
    SetGhostVisibility(IView::VISIBLE);
    decorView->Invalidate();
}

void ExitTransitionCoordinator::HideSharedElements()
{
    MoveSharedElementsFromOverlay();
    if (!mIsHidden) {
        HideViews(mSharedElements);
    }
    mSharedElementsHidden = TRUE;
    FinishIfNecessary();
}

ECode ExitTransitionCoordinator::StartExit()
{
    if (!mIsExitStarted) {
        mIsExitStarted = TRUE;
        AutoPtr<IViewGroup> decorView;
        GetDecor((IViewGroup**)&decorView);
        if (decorView != NULL) {
            decorView->SuppressLayout(TRUE);
        }
        MoveSharedElementsToOverlay();
        AutoPtr<IRunnable> runnable = new BeginTransitionRunnable(this);
        StartTransition(runnable);
    }
    return NOERROR;
}

ExitTransitionCoordinator::MyHandler::MyHandler(
    /* [in] */ ExitTransitionCoordinator* host)
    : mHost(host)
{
}

ECode ExitTransitionCoordinator::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    mHost->mIsCanceled = TRUE;
    mHost->Finish();
    return NOERROR;
}

ECode ExitTransitionCoordinator::StartExit(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (!mIsExitStarted) {
        mIsExitStarted = TRUE;
        AutoPtr<IViewGroup> vg;
        GetDecor((IViewGroup**)&vg);
        if (vg != NULL) {
            vg->SuppressLayout(TRUE);
        }
        AutoPtr<MyHandler> myHandler = new MyHandler(this);
        myHandler->constructor();
        mHandler = (IHandler*)myHandler.Get();

        DelayCancel();
        MoveSharedElementsToOverlay();
        AutoPtr<IDrawable> background;
        IView* decorView = IView::Probe(vg);
        if (decorView != NULL) {
            decorView->GetBackground((IDrawable**)&background);
        }

        if (background == NULL) {
            AutoPtr<IWindow> window;
            GetWindow((IWindow**)&window);
            AutoPtr<IDrawable> cd;
            CColorDrawable::New(IColor::BLACK, (IDrawable**)&cd);
            window->SetBackgroundDrawable(cd);
        }

        AutoPtr<IActivityOptions> options;
        CActivityOptions::MakeSceneTransitionAnimation(mActivity, this,
            mAllSharedElementNames, resultCode, data, (IActivityOptions**)&options);
        AutoPtr<ITranslucentConversionListener> listener = new TranslucentConversionListener(this);
        Boolean bval;
        mActivity->ConvertToTranslucent(listener, options, &bval);

        AutoPtr<IRunnable> runnable = new StartExitTransitionRunnable(this);
        StartTransition(runnable);
    }

    return NOERROR;
}

ECode ExitTransitionCoordinator::Stop()
{
    if (mIsReturning && mActivity != NULL) {
        // Override the previous ActivityOptions. We don't want the
        // activity to have options since we're essentially canceling the
        // transition and finishing right now.
        Boolean bval;
        mActivity->ConvertToTranslucent(NULL, NULL, &bval);
        Finish();
    }
    return NOERROR;
}

void ExitTransitionCoordinator::StartExitTransition()
{
    AutoPtr<ITransition> transition = GetExitTransition();
    AutoPtr<IViewGroup> decorView;
    GetDecor((IViewGroup**)&decorView);
    if (transition != NULL && decorView != NULL && mTransitioningViews != NULL) {
        CTransitionManager::BeginDelayedTransition(decorView, transition);
        AutoPtr<IInterface> obj;
        mTransitioningViews->Get(0, (IInterface**)&obj);
        IView::Probe(obj)->Invalidate();
    }
    else {
        TransitionStarted();
    }
}

ExitTransitionCoordinator::BackgroundAnimatorListenerAdapter::BackgroundAnimatorListenerAdapter(
    /* [in] */ ExitTransitionCoordinator* host)
    : mHost(host)
{}

ECode ExitTransitionCoordinator::BackgroundAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mBackgroundAnimator = NULL;
    if (!mHost->mIsCanceled) {
        mHost->mIsBackgroundReady = TRUE;
        mHost->NotifyComplete();
    }
    return NOERROR;
}

ECode ExitTransitionCoordinator::BackgroundAnimatorListenerAdapter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ExitTransitionCoordinator::BackgroundAnimatorListenerAdapter");
    return NOERROR;
}


void ExitTransitionCoordinator::FadeOutBackground()
{
    if (mBackgroundAnimator == NULL) {
        AutoPtr<IViewGroup> decor;
        GetDecor((IViewGroup**)&decor);
        IView* decorView = IView::Probe(decor);
        AutoPtr<IDrawable> background;
        if (decorView != NULL && (decorView->GetBackground((IDrawable**)&background), background) != NULL) {
            background->Mutate();
            AutoPtr<IWindow> window;
            GetWindow((IWindow**)&window);
            window->SetBackgroundDrawable(background);
            AutoPtr<ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(1);
            args->Set(0, 0);
            mBackgroundAnimator = ObjectAnimator::OfInt32(background, String("alpha"), args);
            AutoPtr<IAnimatorListener> listener = new BackgroundAnimatorListenerAdapter(this);
            IAnimator* animator = IAnimator::Probe(mBackgroundAnimator);
            animator->AddListener(listener);
            animator->SetDuration(GetFadeDuration());
            animator->Start();
        }
        else {
            mIsBackgroundReady = TRUE;
        }
    }
}

AutoPtr<ITransition> ExitTransitionCoordinator::GetExitTransition()
{
    AutoPtr<ITransition> viewsTransition;
    Boolean bval;
    if (mTransitioningViews != NULL && (mTransitioningViews->IsEmpty(&bval), !bval)) {
        viewsTransition = ConfigureTransition(GetViewsTransition(), TRUE);
    }
    if (viewsTransition == NULL) {
        ExitTransitionComplete();
    }
    else {
        AutoPtr<IArrayList> transitioningViews = mTransitioningViews;
        AutoPtr<ITransitionListener> listener = new ExitContinueTransitionListener(this, transitioningViews);
        viewsTransition->AddListener(listener);
        viewsTransition->ForceVisibility(IView::INVISIBLE, FALSE);
    }
    return viewsTransition;
}

AutoPtr<ITransition> ExitTransitionCoordinator::GetSharedElementExitTransition()
{
    AutoPtr<ITransition> sharedElementTransition;
    Boolean bval;
    if (!mSharedElements->IsEmpty(&bval)) {
        sharedElementTransition = ConfigureTransition(GetSharedElementTransition(), FALSE);
    }
    if (sharedElementTransition == NULL) {
        SharedElementTransitionComplete();
    }
    else {
        AutoPtr<ITransitionListener> listener = new SharedElementContinueTransitionListener(this);
        sharedElementTransition->AddListener(listener);
        AutoPtr<IInterface> obj;
        mSharedElements->Get(0, (IInterface**)&obj);
        IView::Probe(obj)->Invalidate();
    }
    return sharedElementTransition;
}

void ExitTransitionCoordinator::BeginTransitions()
{
    AutoPtr<ITransition> sharedElementTransition = GetSharedElementExitTransition();
    AutoPtr<ITransition> viewsTransition = GetExitTransition();

    AutoPtr<ITransition> transition = MergeTransitions(sharedElementTransition, viewsTransition);
    AutoPtr<IViewGroup> decorView;
    GetDecor((IViewGroup**)&decorView);
    if (transition != NULL && decorView != NULL) {
        SetGhostVisibility(IView::INVISIBLE);
        ScheduleGhostVisibilityChange(IView::INVISIBLE);
        CTransitionManager::BeginDelayedTransition(decorView, transition);
        ScheduleGhostVisibilityChange(IView::VISIBLE);
        SetGhostVisibility(IView::VISIBLE);
        IView::Probe(decorView)->Invalidate();
    }
    else {
        TransitionStarted();
    }
}

void ExitTransitionCoordinator::ExitTransitionComplete()
{
    mExitComplete = TRUE;
    NotifyComplete();
}

Boolean ExitTransitionCoordinator::IsReadyToNotify()
{
    return mSharedElementBundle != NULL && mResultReceiver != NULL && mIsBackgroundReady;
}

void ExitTransitionCoordinator::SharedElementTransitionComplete()
{
    mSharedElementBundle = mExitSharedElementBundle == NULL
            ? CaptureSharedElementState() : CaptureExitSharedElementsState();
    NotifyComplete();
}

AutoPtr<IBundle> ExitTransitionCoordinator::CaptureExitSharedElementsState()
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    AutoPtr<IRectF> bounds;
    CRectF::New((IRectF**)&bounds);
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    Int32 size;
    mSharedElements->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mSharedElementNames->Get(i, (IInterface**)&obj);
        String name = Object::ToString(obj);
        AutoPtr<IBundle> sharedElementState;
        mExitSharedElementBundle->GetBundle(name, (IBundle**)&sharedElementState);
        if (sharedElementState != NULL) {
            bundle->PutBundle(name, sharedElementState);
        }
        else {
            AutoPtr<IInterface> viewObj;
            mSharedElements->Get(i, (IInterface**)&viewObj);
            IView* view = IView::Probe(viewObj);
            CaptureSharedElementState(view, name, bundle, matrix, bounds);
        }
    }
    return bundle;
}

void ExitTransitionCoordinator::NotifyComplete()
{
    if (IsReadyToNotify()) {
        if (!mSharedElementNotified) {
            mSharedElementNotified = TRUE;
            DelayCancel();
            mResultReceiver->Send(IActivityTransitionCoordinator::MSG_TAKE_SHARED_ELEMENTS, mSharedElementBundle);
        }
        if (!mExitNotified && mExitComplete) {
            mExitNotified = TRUE;
            mResultReceiver->Send(IActivityTransitionCoordinator::MSG_EXIT_TRANSITION_COMPLETE, NULL);
            mResultReceiver = NULL; // done talking
            AutoPtr<IViewGroup> decorView;
            GetDecor((IViewGroup**)&decorView);
            if (!mIsReturning && decorView != NULL) {
                decorView->SuppressLayout(FALSE);
            }
            FinishIfNecessary();
        }
    }
}

void ExitTransitionCoordinator::FinishIfNecessary()
{
    Boolean bval;
    if (mIsReturning && mExitNotified && mActivity != NULL
        && (mSharedElements->IsEmpty(&bval) || mSharedElementsHidden)) {
        Finish();
    }
    if (!mIsReturning && mExitNotified) {
        mActivity = NULL; // don't need it anymore
    }
}

void ExitTransitionCoordinator::Finish()
{
    StopCancel();
    if (mActivity != NULL) {
        ((Activity*)mActivity.Get())->mActivityTransitionState->Clear();
        mActivity->Finish();
        mActivity->OverridePendingTransition(0, 0);
        mActivity = NULL;
    }
    // Clear the state so that we can't hold any references accidentally and leak memory.
    mHandler = NULL;
    mSharedElementBundle = NULL;
    if (mBackgroundAnimator != NULL) {
        IAnimator* animator = IAnimator::Probe(mBackgroundAnimator);
        animator->Cancel();
        mBackgroundAnimator = NULL;
    }
    mExitSharedElementBundle = NULL;
    ClearState();
}

Boolean ExitTransitionCoordinator::MoveSharedElementWithParent()
{
    return !mIsReturning;
}

AutoPtr<ITransition> ExitTransitionCoordinator::GetViewsTransition()
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    if (window == NULL) {
        return NULL;
    }

    AutoPtr<ITransition> transition;
    if (mIsReturning) {
        window->GetReturnTransition((ITransition**)&transition);
    }
    else {
        window->GetExitTransition((ITransition**)&transition);
    }
    return transition;
}

AutoPtr<ITransition> ExitTransitionCoordinator::GetSharedElementTransition()
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    if (window == NULL) {
        return NULL;
    }

    AutoPtr<ITransition> transition;
    if (mIsReturning) {
        window->GetSharedElementReturnTransition((ITransition**)&transition);
    }
    else {
        window->GetSharedElementExitTransition((ITransition**)&transition);
    }
    return transition;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
