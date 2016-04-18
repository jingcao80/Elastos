
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/app/ActivityTransitionState.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/app/EnterTransitionCoordinator.h"
#include "elastos/droid/app/ExitTransitionCoordinator.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindow;

namespace Elastos {
namespace Droid {
namespace App {

const String ActivityTransitionState::ENTERING_SHARED_ELEMENTS("android:enteringSharedElements");
const String ActivityTransitionState::EXITING_MAPPED_FROM("android:exitingMappedFrom");
const String ActivityTransitionState::EXITING_MAPPED_TO("android:exitingMappedTo");

CAR_INTERFACE_IMPL(ActivityTransitionState, Object, IActivityTransitionState)

ActivityTransitionState::ActivityTransitionState()
    : mHasExited(FALSE)
    , mIsEnterPostponed(FALSE)
    , mExitTransitionCoordinatorsKey(1)
    , mIsEnterTriggered(FALSE)
{}

ActivityTransitionState::~ActivityTransitionState()
{
}

ECode ActivityTransitionState::constructor()
{
    return NOERROR;
}

ECode ActivityTransitionState::AddExitTransitionCoordinator(
    /* [in] */ IExitTransitionCoordinator* exitTransitionCoordinator,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mExitTransitionCoordinators == NULL) {
        mExitTransitionCoordinators = new HashMap<Int32, AutoPtr<IWeakReference> >();
    }


    // clean up old references:
    HashMap<Int32, AutoPtr<IWeakReference> >::Iterator it = mExitTransitionCoordinators->Begin();
    for (; it != mExitTransitionCoordinators->End();) {
        AutoPtr<IWeakReference> oldRef = it->mSecond;
        if (oldRef) {
            mExitTransitionCoordinators->Erase(it++);
        }
        else {
            ++it;
        }
    }

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(exitTransitionCoordinator);
    AutoPtr<IWeakReference> ref;
    wrs->GetWeakReference((IWeakReference**)&ref);
    Int32 newKey = mExitTransitionCoordinatorsKey++;
    (*mExitTransitionCoordinators)[newKey] = ref;
    *result = newKey;
    return NOERROR;
}

ECode ActivityTransitionState::ReadState(
    /* [in] */ IBundle* bundle)
{
    if (bundle != NULL) {
        Boolean bval;
        if (mEnterTransitionCoordinator == NULL
            || (mEnterTransitionCoordinator->IsReturning(&bval), bval)) {
            mEnteringNames = NULL;
            bundle->GetStringArrayList(ENTERING_SHARED_ELEMENTS, (IArrayList**)&mEnteringNames);
        }
        if (mEnterTransitionCoordinator == NULL) {
            mExitingFrom = NULL;
            mExitingTo = NULL;
            bundle->GetStringArrayList(EXITING_MAPPED_FROM, (IArrayList**)&mExitingFrom);
            bundle->GetStringArrayList(EXITING_MAPPED_TO, (IArrayList**)&mExitingTo);
        }
    }
    return NOERROR;
}

ECode ActivityTransitionState::SaveState(
    /* [in] */ IBundle* bundle)
{
    if (mEnteringNames != NULL) {
        bundle->PutStringArrayList(ENTERING_SHARED_ELEMENTS, mEnteringNames);
    }
    if (mExitingFrom != NULL) {
        bundle->PutStringArrayList(EXITING_MAPPED_FROM, mExitingFrom);
        bundle->PutStringArrayList(EXITING_MAPPED_TO, mExitingTo);
    }
    return NOERROR;
}

ECode ActivityTransitionState::SetEnterActivityOptions(
    /* [in] */ IActivity* activity,
    /* [in] */ IActivityOptions* options)
{
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);
    Boolean hasFeature;
    window->HasFeature(IWindow::FEATURE_ACTIVITY_TRANSITIONS, &hasFeature);
    if (hasFeature && options != NULL && mEnterActivityOptions == NULL
            && mEnterTransitionCoordinator == NULL) {
        Int32 type;
        options->GetAnimationType(&type);
        if (type == IActivityOptions::ANIM_SCENE_TRANSITION) {
            mEnterActivityOptions = options;
            mIsEnterTriggered = FALSE;
            Boolean bval;
            mEnterActivityOptions->IsReturning(&bval);
            if (bval) {
                RestoreExitedViews();
                Int32 result;
                mEnterActivityOptions->GetResultCode(&result);
                if (result != 0) {
                    AutoPtr<IIntent> intent;
                    mEnterActivityOptions->GetResultData((IIntent**)&intent);
                    activity->OnActivityReenter(result, intent);
                }
            }
        }
    }
    return NOERROR;
}

ECode ActivityTransitionState::EnterReady(
    /* [in] */ IActivity* activity)
{
    if (mEnterActivityOptions == NULL || mIsEnterTriggered) {
        return NOERROR;
    }
    mIsEnterTriggered = TRUE;
    mHasExited = FALSE;
    AutoPtr<IArrayList> sharedElementNames;
    mEnterActivityOptions->GetSharedElementNames((IArrayList**)&sharedElementNames);
    AutoPtr<IResultReceiver> resultReceiver;
    mEnterActivityOptions->GetResultReceiver((IResultReceiver**)&resultReceiver);
    Boolean bval;
    mEnterActivityOptions->IsReturning(&bval);
    if (bval) {
        RestoreExitedViews();
        AutoPtr<IWindow> window;
        activity->GetWindow((IWindow**)&window);
        AutoPtr<IView> view;
        window->GetDecorView((IView**)&view);
        view->SetVisibility(IView::VISIBLE);
    }
    mEnterActivityOptions->IsReturning(&bval);
    AutoPtr<EnterTransitionCoordinator> etc = new EnterTransitionCoordinator();
    etc->constructor(activity, resultReceiver, sharedElementNames, bval);
    mEnterTransitionCoordinator = (IEnterTransitionCoordinator*)etc.Get();
    if (!mIsEnterPostponed) {
        StartEnter();
    }
    return NOERROR;
}

ECode ActivityTransitionState::PostponeEnterTransition()
{
    mIsEnterPostponed = TRUE;
    return NOERROR;
}

ECode ActivityTransitionState::StartPostponedEnterTransition()
{
    if (mIsEnterPostponed) {
        mIsEnterPostponed = FALSE;
        if (mEnterTransitionCoordinator != NULL) {
            StartEnter();
        }
    }
    return NOERROR;
}

void ActivityTransitionState::StartEnter()
{
    Boolean bval;
    mEnterActivityOptions->IsReturning(&bval);
    if (bval) {
        if (mExitingToView != NULL) {
            mEnterTransitionCoordinator->ViewInstancesReady(mExitingFrom, mExitingTo, mExitingToView);
        }
        else {
            mEnterTransitionCoordinator->NamedViewsReady(mExitingFrom, mExitingTo);
        }
    } else {
        mEnterTransitionCoordinator->NamedViewsReady(NULL, NULL);
        mEnteringNames = NULL;
        IActivityTransitionCoordinator::Probe(mEnterTransitionCoordinator)->GetAllSharedElementNames((IArrayList**)&mEnteringNames);
    }

    mExitingFrom = NULL;
    mExitingTo = NULL;
    mExitingToView = NULL;
    mEnterActivityOptions = NULL;
}

ECode ActivityTransitionState::OnStop()
{
    RestoreExitedViews();
    if (mEnterTransitionCoordinator != NULL) {
        mEnterTransitionCoordinator->Stop();
        mEnterTransitionCoordinator = NULL;
    }
    if (mReturnExitCoordinator != NULL) {
        mReturnExitCoordinator->Stop();
        mReturnExitCoordinator = NULL;
    }
    return NOERROR;
}

ECode ActivityTransitionState::OnResume()
{
    RestoreExitedViews();
    return NOERROR;
}

ECode ActivityTransitionState::Clear()
{
    mEnteringNames = NULL;
    mExitingFrom = NULL;
    mExitingTo = NULL;
    mExitingToView = NULL;
    mCalledExitCoordinator = NULL;
    mEnterTransitionCoordinator = NULL;
    mEnterActivityOptions = NULL;
    mExitTransitionCoordinators = NULL;
    return NOERROR;
}

void ActivityTransitionState::RestoreExitedViews() {
    if (mCalledExitCoordinator != NULL) {
        mCalledExitCoordinator->ResetViews();
        mCalledExitCoordinator = NULL;
    }
}

ECode ActivityTransitionState::StartExitBackTransition(
    /* [in] */ IActivity* activity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (mEnteringNames != NULL) {
        if (!mHasExited) {
            mHasExited = TRUE;
            AutoPtr<ITransition> enterViewsTransition;
            AutoPtr<IViewGroup> decor;
            if (mEnterTransitionCoordinator != NULL) {
                IActivityTransitionCoordinator* atc = IActivityTransitionCoordinator::Probe(mEnterTransitionCoordinator);
                mEnterTransitionCoordinator->GetEnterViewsTransition((ITransition**)&enterViewsTransition);
                atc->GetDecor((IViewGroup**)&decor);
                mEnterTransitionCoordinator->CancelEnter();
                mEnterTransitionCoordinator = NULL;
                if (enterViewsTransition != NULL && decor != NULL) {
                    enterViewsTransition->Pause(IView::Probe(decor));
                }
            }

            AutoPtr<ExitTransitionCoordinator> etc = new ExitTransitionCoordinator();
            etc->constructor(activity, mEnteringNames, NULL, NULL, TRUE);

            if (enterViewsTransition != NULL && decor != NULL) {
                enterViewsTransition->Resume(IView::Probe(decor));
            }

            Activity* act = (Activity*)activity;
            etc->StartExit(act->mResultCode, act->mResultData);
            mReturnExitCoordinator = (IExitTransitionCoordinator*)etc.Get();
        }

        *result = TRUE;
    }
    return NOERROR;
}

ECode ActivityTransitionState::StartExitOutTransition(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* options)
{
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);
    Boolean hasFeature;
    window->HasFeature(IWindow::FEATURE_ACTIVITY_TRANSITIONS, &hasFeature);
    if (!hasFeature) {
        return NOERROR;
    }

    AutoPtr<IActivityOptions> activityOptions;
    CActivityOptions::New((IActivityOptions**)&activityOptions);
    mEnterTransitionCoordinator = NULL;
    Int32 type;
    activityOptions->GetAnimationType(&type);
    if (type == IActivityOptions::ANIM_SCENE_TRANSITION) {
        Int32 key;
        activityOptions->GetExitCoordinatorKey(&key);
        HashMap<Int32, AutoPtr<IWeakReference> >::Iterator it = mExitTransitionCoordinators->Find(key);
        if (it != mExitTransitionCoordinators->End()) {
            AutoPtr<IWeakReference> wr = it->mSecond;
            AutoPtr<IExitTransitionCoordinator> cec;
            wr->Resolve(EIID_IExitTransitionCoordinator, (IInterface**)&cec);
            mCalledExitCoordinator = cec;
            mExitTransitionCoordinators->Erase(it);
            if (mCalledExitCoordinator != NULL) {
                mExitingFrom = NULL;
                mExitingTo = NULL;
                mExitingToView = NULL;
                IActivityTransitionCoordinator* atc = IActivityTransitionCoordinator::Probe(mCalledExitCoordinator);
                atc->GetAcceptedNames((IArrayList**)&mExitingFrom);
                atc->GetMappedNames((IArrayList**)&mExitingTo);
                atc->CopyMappedViews((IArrayList**)&mExitingToView);
                mCalledExitCoordinator->StartExit();
            }
        }
    }
    return NOERROR;
}

ECode ActivityTransitionState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("ActivityTransitionState");
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
