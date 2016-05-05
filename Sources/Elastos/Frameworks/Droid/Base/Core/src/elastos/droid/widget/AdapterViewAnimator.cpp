#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/animation/AnimatorInflater.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"
#include "elastos/droid/content/CIntentFilterComparison.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/widget/AdapterViewAnimator.h"
#include "elastos/droid/widget/CFrameLayout.h"
#include "elastos/droid/widget/CRemoteViewsAdapter.h"
#include "elastos/core/IntegralToString.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::AnimatorInflater;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::CFrameLayout;
using Elastos::Droid::Widget::CRemoteViewsAdapter;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IntegralToString;

namespace Elastos {
namespace Droid {
namespace Widget {

const String AdapterViewAnimator::TAG("AdapterViewAnimator");
const Int32 AdapterViewAnimator::DEFAULT_ANIMATION_DURATION;
const Int32 AdapterViewAnimator::TOUCH_MODE_NONE;
const Int32 AdapterViewAnimator::TOUCH_MODE_DOWN_IN_CURRENT_VIEW;
const Int32 AdapterViewAnimator::TOUCH_MODE_HANDLED;

//=====================================================================
//               AdapterViewAnimator::ViewAndMetaData
//=====================================================================
AdapterViewAnimator::ViewAndMetaData::ViewAndMetaData(
    /* [in] */ IView* v,
    /* [in] */ Int32 relativeIndex,
    /* [in] */ Int32 adapterPosition,
    /* [in] */ Int64 itemId)
    : mView(v)
    , mRelativeIndex(relativeIndex)
    , mAdapterPosition(adapterPosition)
    , mItemId(itemId)
{
}

//=====================================================================
//               AdapterViewAnimator::CheckForTap
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewAnimator::CheckForTap, Object, IRunnable)

AdapterViewAnimator::CheckForTap::CheckForTap(
    /* [in] */ AdapterViewAnimator* host)
    : mHost(host)
{
    assert(mHost);
}

ECode AdapterViewAnimator::CheckForTap::Run()
{
    if (mHost->mTouchMode == mHost->TOUCH_MODE_DOWN_IN_CURRENT_VIEW) {
        AutoPtr<IView> v;
        mHost->GetCurrentView((IView**)&v);
        mHost->ShowTapFeedback(v);
    }
    return NOERROR;
}

//=====================================================================
//               AdapterViewAnimator::ActionUpRun
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewAnimator::ActionUpRun, Object, IRunnable)

AdapterViewAnimator::ActionUpRun::ActionUpRun(
    /* [in] */ AdapterViewAnimator* host,
    /* [in] */ ViewAndMetaData* data,
    /* [in] */ IView* v)
    : mHost(host)
    , mData(data)
    , mView(v)
{
    assert(mHost);
}

ECode AdapterViewAnimator::ActionUpRun::Run()
{
    mHost->HideTapFeedback(mView);
    AutoPtr<IRunnable> run = new ActionUpInner(mHost, mData, mView);
    Boolean resTmp;
    mHost->Post(run, &resTmp);
    return NOERROR;
}

//=====================================================================
//               AdapterViewAnimator::ActionUpInner
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewAnimator::ActionUpInner, Object, IRunnable)

AdapterViewAnimator::ActionUpInner::ActionUpInner(
    /* [in] */ AdapterViewAnimator* host,
    /* [in] */ ViewAndMetaData* data,
    /* [in] */ IView* v)
    : mHost(host)
    , mData(data)
    , mView(v)
{
    assert(mHost);
}

ECode AdapterViewAnimator::ActionUpInner::Run()
{
    Boolean resTmp = FALSE;
    if (mData != NULL) {
        mHost->PerformItemClick(mView, mData->mAdapterPosition, mData->mItemId, &resTmp);
    }
    else {
        mHost->PerformItemClick(mView, 0, 0, &resTmp);
    }
    return NOERROR;
}

//=====================================================================
//               AdapterViewAnimator::CheckDataRun
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewAnimator::CheckDataRun, Object, IRunnable)

AdapterViewAnimator::CheckDataRun::CheckDataRun(
    /* [in] */ AdapterViewAnimator* host)
    : mHost(host)
{
    assert(mHost);
}

ECode AdapterViewAnimator::CheckDataRun::Run()
{
    mHost->HandleDataChanged();
    Int32 count = 0;
    mHost->GetCount(&count);
    if (mHost->mWhichChild >= mHost->GetWindowSize()) {
        mHost->mWhichChild = 0;
        mHost->ShowOnly(mHost->mWhichChild, FALSE);
    }
    else if (mHost->mOldItemCount != count) {
        mHost->ShowOnly(mHost->mWhichChild, FALSE);
    }

    mHost->RefreshChildren();
    mHost->RequestLayout();
    return NOERROR;
}

//=====================================================================
//               AdapterViewAnimator::InternalInsetsInfo
//=====================================================================
AdapterViewAnimator::SavedState::SavedState(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 whichChild)
    : mWhichChild(whichChild)
{
    View::BaseSavedState::constructor(superState);
}

/**
 * Constructor called from {@link #CREATOR}
 */
//private SavedState(Parcel in) {
//    super(in);
//    this.whichChild = in.readInt();
//}

ECode AdapterViewAnimator::SavedState::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    assert(0);
    //-- base class has corresponding func: ViewBaseSavedState::WriteToParcel(out, flags);
    out->WriteInt32(this->mWhichChild);
    return NOERROR;
}

ECode AdapterViewAnimator::SavedState::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String strWhichChild = IntegralToString::ToString(this->mWhichChild);
    *result = String("AdapterViewAnimator.SavedState{ whichChild = ") + strWhichChild + String(" }");
    return NOERROR;
}

//=====================================================================
//                      AdapterViewAnimator
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewAnimator, AdapterView, IAdapterViewAnimator)

AdapterViewAnimator::AdapterViewAnimator()
    : mWhichChild(0)
    , mAnimateFirstTime(TRUE)
    , mActiveOffset(0)
    , mMaxNumActiveViews(1)
    , mCurrentWindowStart(0)
    , mCurrentWindowEnd(-1)
    , mCurrentWindowStartUnbounded(0)
    , mDeferNotifyDataSetChanged(FALSE)
    , mFirstTime(TRUE)
    , mLoopViews(TRUE)
    , mReferenceChildWidth(-1)
    , mReferenceChildHeight(-1)
    , mRestoreWhichChild(-1)
    , mTouchMode(TOUCH_MODE_NONE)
{
}

ECode AdapterViewAnimator::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode AdapterViewAnimator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode AdapterViewAnimator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode AdapterViewAnimator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AdapterView::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::AdapterViewAnimator),
            ArraySize(R::styleable::AdapterViewAnimator));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, 0, (ITypedArray**)&a);
    Int32 resource = 0;
    a->GetResourceId(R::styleable::AdapterViewAnimator_inAnimation, 0, &resource);
    if (resource > 0) {
        SetOutAnimation(context, resource);
    }
    else {
        SetOutAnimation(GetDefaultOutAnimation());
    }

    Boolean flag = FALSE;
    a->GetBoolean(R::styleable::AdapterViewAnimator_animateFirstView, TRUE, &flag);
    SetAnimateFirstView(flag);
    a->GetBoolean(R::styleable::AdapterViewAnimator_loopViews, FALSE, &mLoopViews);
    a->Recycle();
    InitViewAnimator();
    return NOERROR;
}

ECode AdapterViewAnimator::SetDisplayedChild(
    /* [in] */ Int32 whichChild)
{
    SetDisplayedChild(whichChild, TRUE);
    return NOERROR;
}

ECode AdapterViewAnimator::GetDisplayedChild(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mWhichChild;
    return NOERROR;
}

ECode AdapterViewAnimator::ShowNext()
{
    SetDisplayedChild(mWhichChild + 1);
    return NOERROR;
}

ECode AdapterViewAnimator::ShowPrevious()
{
    SetDisplayedChild(mWhichChild - 1);
    return NOERROR;
}

ECode AdapterViewAnimator::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    Int32 action = 0;
    event->GetAction(&action);
    Boolean handled = FALSE;
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            {
                AutoPtr<IView> v;
                GetCurrentView((IView**)&v);
                if (v) {
                    Float x = 0, y = 0;
                    event->GetX(&x);
                    event->GetY(&y);
                    if (IsTransformedTouchPointInView(x, y, v, NULL)) {
                        if (mPendingCheckForTap == NULL) {
                            mPendingCheckForTap = new CheckForTap(this);
                        }
                        mTouchMode = TOUCH_MODE_DOWN_IN_CURRENT_VIEW;
                        Boolean resTmp;
                        PostDelayed(mPendingCheckForTap, CViewConfiguration::GetTapTimeout(), &resTmp);
                    }
                }
            }
            break;
        case IMotionEvent::ACTION_MOVE: break;
        case IMotionEvent::ACTION_POINTER_UP: break;
        case IMotionEvent::ACTION_UP:
            {
                if (mTouchMode == TOUCH_MODE_DOWN_IN_CURRENT_VIEW) {
                    AutoPtr<IView> v;
                    GetCurrentView((IView**)&v);
                    AutoPtr<ViewAndMetaData> viewData = GetMetaDataForChild(v);
                    if (v) {
                        Float x = 0, y = 0;
                        event->GetX(&x);
                        event->GetY(&y);
                        if (IsTransformedTouchPointInView(x, y, v, NULL)) {
                            AutoPtr<IHandler> handler;
                            GetHandler((IHandler**)&handler);
                            if (handler) {
                                handler->RemoveCallbacks(mPendingCheckForTap);
                                ShowTapFeedback(v);
                                AutoPtr<IRunnable> run = new ActionUpRun(this, viewData, v);
                                Boolean resTmp;
                                PostDelayed(run, CViewConfiguration::GetTapTimeout(), &resTmp);
                            }

                            handled = TRUE;
                        }
                    }
                    mTouchMode = TOUCH_MODE_NONE;
                }
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
            {
                AutoPtr<IView> v;
                GetCurrentView((IView**)&v);
                if (v) {
                    HideTapFeedback(v);
                }
                mTouchMode = TOUCH_MODE_NONE;
            }
            break;
    }

    *result = handled;
    return NOERROR;
}

ECode AdapterViewAnimator::OnSaveInstanceState(
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IParcelable> superState = AdapterView::OnSaveInstanceState();
    if (mRemoteViewsAdapter) {
        mRemoteViewsAdapter->SaveRemoteViewsCache();
    }

    assert(0);
    //*result = new SavedState(superState, mWhichChild);
    //REFCOUNT_ADD(*result);
    return NOERROR;
}

void AdapterViewAnimator::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    SavedState* ss = (SavedState*)state;
    assert(0);
    //--: AdapterView::OnRestoreInstanceState(ss->GetSuperState());

    // Here we set mWhichChild in addition to setDisplayedChild
    // We do the former in case mAdapter is null, and hence setDisplayedChild won't
    // set mWhichChild
    mWhichChild = ss->mWhichChild;

    // When using RemoteAdapters, the async connection process can lead to
    // onRestoreInstanceState to be called before setAdapter(), so we need to save the previous
    // values to restore the list position after we connect, and can skip setting the displayed
    // child until then.
    if (mRemoteViewsAdapter != NULL && mAdapter == NULL) {
        mRestoreWhichChild = mWhichChild;
    }
    else {
        SetDisplayedChild(mWhichChild, FALSE);
    }
}

ECode AdapterViewAnimator::GetCurrentView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> temp = GetViewAtRelativeIndex(mActiveOffset);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdapterViewAnimator::GetInAnimation(
    /* [out] */ IObjectAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInAnimation;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdapterViewAnimator::SetInAnimation(
    /* [in] */ IObjectAnimator* inAnimation)
{
    mInAnimation = inAnimation;
    return NOERROR;
}

ECode AdapterViewAnimator::SetInAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    AutoPtr<IAnimator> anim;
    AnimatorInflater::LoadAnimator(context, resourceID, (IAnimator**)&anim);
    SetInAnimation(IObjectAnimator::Probe(anim));
    return NOERROR;
}

ECode AdapterViewAnimator::GetOutAnimation(
    /* [out] */ IObjectAnimator** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOutAnimation;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdapterViewAnimator::SetOutAnimation(
    /* [in] */ IObjectAnimator* outAnimation)
{
    mOutAnimation = outAnimation;
    return NOERROR;
}

ECode AdapterViewAnimator::SetOutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    AutoPtr<IAnimator> anim;
    AnimatorInflater::LoadAnimator(context, resourceID, (IAnimator**)&anim);
    SetInAnimation(IObjectAnimator::Probe(anim));
    return NOERROR;
}

ECode AdapterViewAnimator::SetAnimateFirstView(
    /* [in] */ Boolean animate)
{
    mAnimateFirstTime = animate;
    return NOERROR;
}

ECode AdapterViewAnimator::GetBaseline(
    /* [out] */ Int32* baseline)
{
    VALIDATE_NOT_NULL(baseline);
    AutoPtr<IView> currentView;
    GetCurrentView((IView**)&currentView);
    if (currentView != NULL) {
        currentView->GetBaseline(baseline);
    }
    else {
        AdapterView::GetBaseline(baseline);
    }
    return NOERROR;
}

ECode AdapterViewAnimator::GetAdapter(
    /* [out] */ IAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdapterViewAnimator::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    VALIDATE_NOT_NULL(adapter);
    if (mAdapter && mDataSetObserver) {
        mAdapter->UnregisterDataSetObserver(mDataSetObserver);
    }

    mAdapter = adapter;
    CheckFocus();

    if (mAdapter) {
        mDataSetObserver = new AdapterDataSetObserver(this);
        mAdapter->RegisterDataSetObserver(mDataSetObserver);
        mAdapter->GetCount(&mItemCount);
    }

    SetFocusable(true);
    mWhichChild = 0;
    ShowOnly(mWhichChild, FALSE);
    return NOERROR;
}

ECode AdapterViewAnimator::SetRemoteViewsAdapter(
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);
    if (mRemoteViewsAdapter) {
        AutoPtr<IIntentFilterComparison> fcNew, fcOld;
        CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&fcNew);
        AutoPtr<IIntent> old;
        mRemoteViewsAdapter->GetRemoteViewsServiceIntent((IIntent**)&old);
        CIntentFilterComparison::New(old, (IIntentFilterComparison**)&fcOld);
        Boolean res = FALSE;
        fcNew->Equals(fcOld, &res);
        if (res) {
            return NOERROR;
        }
    }
    mDeferNotifyDataSetChanged = FALSE;
    assert(0 && "TODO");
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CRemoteViewsAdapter::New(context, intent, IRemoteAdapterConnectionCallback::Probe(this),
        (IRemoteViewsAdapter**)&mRemoteViewsAdapter);
    Boolean ready = FALSE;
    mRemoteViewsAdapter->IsDataReady(&ready);
    if (ready) {
        SetAdapter(IAdapter::Probe(mRemoteViewsAdapter));
    }
    return NOERROR;
}

ECode AdapterViewAnimator::SetRemoteViewsOnClickHandler(
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    VALIDATE_NOT_NULL(handler);
    if (mRemoteViewsAdapter) {
        mRemoteViewsAdapter->SetRemoteViewsOnClickHandler(handler);
    }
    return NOERROR;
}

ECode AdapterViewAnimator::SetSelection(
    /* [in] */ Int32 position)
{
    SetDisplayedChild(position);
    return NOERROR;
}

ECode AdapterViewAnimator::GetSelectedView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> temp = GetViewAtRelativeIndex(mActiveOffset);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AdapterViewAnimator::DeferNotifyDataSetChanged()
{
    mDeferNotifyDataSetChanged = TRUE;
    return NOERROR;
}

ECode AdapterViewAnimator::OnRemoteAdapterConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (TO_IINTERFACE(mRemoteViewsAdapter) != TO_IINTERFACE(mAdapter)) {
        SetAdapter(IAdapter::Probe(mRemoteViewsAdapter));

        if (mDeferNotifyDataSetChanged) {
            IBaseAdapter* baseAdapterTmp = IBaseAdapter::Probe(mRemoteViewsAdapter);
            baseAdapterTmp->NotifyDataSetChanged();
            mDeferNotifyDataSetChanged = FALSE;
        }

        if (mRestoreWhichChild > -1) {
            SetDisplayedChild(mRestoreWhichChild, FALSE);
            mRestoreWhichChild = -1;
        }

        *result = FALSE;
        return NOERROR;
    }
    else if (mRemoteViewsAdapter) {
        mRemoteViewsAdapter->SuperNotifyDataSetChanged();
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode AdapterViewAnimator::OnRemoteAdapterDisconnected()
{
    return NOERROR;
}

ECode AdapterViewAnimator::Advance()
{
    ShowNext();
    return NOERROR;
}

ECode AdapterViewAnimator::FyiWillBeAdvancedByHostKThx()
{
    return NOERROR;
}

ECode AdapterViewAnimator::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    AdapterView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> csq;
    CString::New(String("AdapterViewAnimator"), (ICharSequence**)&csq);
    IAccessibilityRecord* recordTmp = IAccessibilityRecord::Probe(event);
    recordTmp->SetClassName(csq);
    return NOERROR;
}

ECode AdapterViewAnimator::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    AdapterView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> csq;
    CString::New(String("AdapterViewAnimator"), (ICharSequence**)&csq);
    info->SetClassName(csq);
    return NOERROR;
}

void AdapterViewAnimator::ConfigureViewAnimator(
    /* [in] */ Int32 numVisibleViews,
    /* [in] */ Int32 activeOffset)
{
    if (activeOffset > numVisibleViews - 1) {
        // Throw an exception here.
        return ;
    }
    mMaxNumActiveViews = numVisibleViews;
    mActiveOffset = activeOffset;
    mPreviousViews.Clear();
    mViewsMap.Clear();
    RemoveAllViewsInLayout();
    mCurrentWindowStart = 0;
    mCurrentWindowEnd = -1;
}

void AdapterViewAnimator::TransformViewForTransition(
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int64 toIndex,
    /* [in] */ IView* v,
    /* [in] */ Boolean animate)
{
    IAnimator* animationTmp = IAnimator::Probe(mInAnimation);
    if (fromIndex == -1) {
        animationTmp->SetTarget(TO_IINTERFACE(v));
        animationTmp->Start();
    }
    else if (toIndex == -1) {
        animationTmp->SetTarget(TO_IINTERFACE(v));
        animationTmp->Start();
    }
}

AutoPtr<IObjectAnimator> AdapterViewAnimator::GetDefaultInAnimation()
{
    AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 0.0f;
    (*array)[1] = 1.0f;
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<IObjectAnimator> anim;
    helper->OfFloat(NULL, String("alpha"), array, (IObjectAnimator**)&anim);
    IValueAnimator* valueAnimator = IValueAnimator::Probe(anim);
    valueAnimator->SetDuration(DEFAULT_ANIMATION_DURATION);
    return anim;
}

AutoPtr<IObjectAnimator> AdapterViewAnimator::GetDefaultOutAnimation()
{
    AutoPtr< ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = 1.0f;
    (*array)[1] = 0.0f;
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<IObjectAnimator> anim;
    helper->OfFloat(NULL, String("alpha"), array, (IObjectAnimator**)&anim);
    IValueAnimator* valueAnimator = IValueAnimator::Probe(anim);
    valueAnimator->SetDuration(DEFAULT_ANIMATION_DURATION);
    return anim;
}

void AdapterViewAnimator::ApplyTransformForChildAtIndex(
    /* [in] */ IView* child,
    /* [in] */ Int32 relativeIndex)
{
}

Int32 AdapterViewAnimator::Modulo(
    /* [in] */ Int32 pos,
    /* [in] */ Int32 size)
{
    if (size > 0) {
        return (size + (pos % size)) % size;
    }
    else {
        return 0;
    }
}

AutoPtr<IView> AdapterViewAnimator::GetViewAtRelativeIndex(
    /* [in] */ Int32 relativeIndex)
{
    if (relativeIndex >= 0 && relativeIndex <= GetNumActiveViews() - 1 && mAdapter) {
        Int32 i = Modulo(mCurrentWindowStartUnbounded + relativeIndex, GetWindowSize());
        // is right?
        if (mViewsMap.Find(i) != mViewsMap.End()) {
            return (mViewsMap.Find(i))->mSecond->mView;
        }
    }
    return NULL;
}

Int32 AdapterViewAnimator::GetNumActiveViews()
{
    if (mAdapter) {
        Int32 count = 0;
        GetCount(&count);
        return Elastos::Core::Math::Min(count + 1, mMaxNumActiveViews);
    }
    return mMaxNumActiveViews;
}

Int32 AdapterViewAnimator::GetWindowSize()
{
    if (mAdapter) {
        Int32 adapterCount = 0;
        GetCount(&adapterCount);
        if (adapterCount <= GetNumActiveViews() && mLoopViews) {
            return adapterCount * mMaxNumActiveViews;
        }
        else {
            return adapterCount;
        }
    }
    else {
        return 0;
    }
}

AutoPtr<IViewGroupLayoutParams> AdapterViewAnimator::CreateOrReuseLayoutParams(
    /* [in] */ IView* v)
{
    AutoPtr<IViewGroupLayoutParams> currentLp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&currentLp);
    AutoPtr<IViewGroupLayoutParams> temp = IViewGroupLayoutParams::Probe(currentLp);
    if (temp) return temp;
    CViewGroupLayoutParams::New(0, 0, (IViewGroupLayoutParams**)&temp);
    return temp;
}

void AdapterViewAnimator::RefreshChildren()
{
    if (!mAdapter) return;
    Int32 adapterCount = 0;
    for (Int32 i = mCurrentWindowStart; i < mCurrentWindowEnd; i++) {
        Int32 index = Modulo(i, GetWindowSize());
        GetCount(&adapterCount);
        AutoPtr<IView> updatedChild;
        mAdapter->GetView(Modulo(i, adapterCount), NULL, IViewGroup::Probe(this), (IView**)&updatedChild);
        Int32 mode = 0;
        if ((updatedChild->GetImportantForAccessibility(&mode), mode) == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
            updatedChild->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
        }

        if(mViewsMap.Find(index) != mViewsMap.End()) {
            AutoPtr<IView> v = (mViewsMap.Find(index))->mSecond->mView;
            AutoPtr<IFrameLayout> fl = IFrameLayout::Probe(v);
            IViewGroup* viewGroup = IViewGroup::Probe(fl);
            assert(fl != NULL);
            if(updatedChild) {
                viewGroup->RemoveAllViewsInLayout();
                viewGroup->AddView(updatedChild);
            }
        }
    }
}

AutoPtr<IFrameLayout> AdapterViewAnimator::GetFrameForChild()
{
    AutoPtr<IFrameLayout> fm;
    CFrameLayout::New(mContext, (IFrameLayout**)&fm);
    return fm;
}

void AdapterViewAnimator::ShowOnly(
    /* [in] */ Int32 childIndex,
    /* [in] */ Boolean animate)
{
    if (!mAdapter) return;
    Int32 adapterCount = 0;
    GetCount(&adapterCount);
    if (adapterCount == 0) return;
    // is right?
    List<Int32>::Iterator it = mPreviousViews.Begin();
    for (; it != mPreviousViews.End(); it++) {
        AutoPtr<IView> viewToRemove = mViewsMap.Find(*it)->mSecond->mView;
        mViewsMap.Erase(mViewsMap.Find(*it)->mFirst);
        viewToRemove->ClearAnimation();
        AutoPtr<IViewGroup> vg = IViewGroup::Probe(viewToRemove);
        if (vg) {
            vg->RemoveAllViewsInLayout();
        }
        ApplyTransformForChildAtIndex(viewToRemove, -1);
        RemoveViewInLayout(viewToRemove);
    }
    mPreviousViews.Clear();
    Int32 newWindowStartUnbounded = childIndex - mActiveOffset;
    Int32 newWindowEndUnbounded = newWindowStartUnbounded + GetNumActiveViews() - 1;
    Int32 newWindowStart = Elastos::Core::Math::Max(0, newWindowStartUnbounded);
    Int32 newWindowEnd = Elastos::Core::Math::Min(adapterCount - 1, newWindowEndUnbounded);

    if (mLoopViews) {
        newWindowStart = newWindowStartUnbounded;
        newWindowEnd = newWindowEndUnbounded;
    }

    Int32 rangeStart = Modulo(newWindowStart, GetWindowSize());
    Int32 rangeEnd = Modulo(newWindowEnd, GetWindowSize());

    Boolean wrap = FALSE;
    if (rangeStart > rangeEnd) {
        wrap = TRUE;
    }

    HashMap< Int32, AutoPtr<ViewAndMetaData> >::Iterator iter = mViewsMap.Begin();
    for (; iter != mViewsMap.End(); iter++) {
        Boolean remove = FALSE;
        if (!wrap && (iter->mFirst < rangeStart || iter->mFirst > rangeEnd)) {
            remove = TRUE;
        }
        else if (wrap && (iter->mFirst > rangeEnd && iter->mFirst < rangeStart)) {
            remove = TRUE;
        }

        if (remove) {
            AutoPtr<IView> previousView = mViewsMap.Find(iter->mFirst)->mSecond->mView;
            Int32 oldRelativeIndex = mViewsMap.Find(iter->mFirst)->mSecond->mRelativeIndex;

            mPreviousViews.Insert(iter->mFirst);
            TransformViewForTransition(oldRelativeIndex, -1, previousView, animate);
        }
    }

    if (!(newWindowStart == mCurrentWindowStart && newWindowEnd == mCurrentWindowEnd &&
            newWindowStartUnbounded == mCurrentWindowStartUnbounded)) {
        for (Int32 i = newWindowStart; i <= newWindowEnd; i++) {
            Int32 index = Modulo(i, GetWindowSize());
            Int32 oldRelativeIndex = -1;
            if (mViewsMap.Find(index) != mViewsMap.End()) {
                oldRelativeIndex = mViewsMap.Find(index)->mSecond->mRelativeIndex;
            }

            Int32 newRelativeIndex = i - newWindowStartUnbounded;

            Boolean res = FALSE;
            for (List<Int32>::Iterator it = mPreviousViews.Begin(); it != mPreviousViews.End(); it++) {
                if (*it == index) {
                    res = TRUE;
                }
            }
            Boolean inOldRange = mViewsMap.Find(index) != mViewsMap.End() && res;

            if (inOldRange) {
                AutoPtr<IView> view = mViewsMap.Find(index)->mSecond->mView;
                mViewsMap.Find(index)->mSecond->mRelativeIndex = newRelativeIndex;
                ApplyTransformForChildAtIndex(view, newRelativeIndex);
                TransformViewForTransition(oldRelativeIndex, newRelativeIndex, view, animate);
            }
            else {
                Int32 adapterPosition = Modulo(i, adapterCount);
                AutoPtr<IView> newView;
                mAdapter->GetView(adapterPosition, NULL, IViewGroup::Probe(this), (IView**)&newView);
                Int64 itemId = 0;
                mAdapter->GetItemId(adapterPosition, &itemId);

                AutoPtr<IFrameLayout> fl = GetFrameForChild();

                IViewGroup* viewGroupTmp = IViewGroup::Probe(fl);
                if (newView) {
                    viewGroupTmp->AddView(newView);
                }

                AutoPtr<ViewAndMetaData> source = new ViewAndMetaData(IView::Probe(fl), newRelativeIndex, adapterPosition, itemId);
                mViewsMap.Insert(HashMap<Int32, AutoPtr<ViewAndMetaData> >::ValueType(index, source));

                IView* viewTmp = IView::Probe(fl);
                AddChild(viewTmp);
                ApplyTransformForChildAtIndex(viewTmp, newRelativeIndex);
                TransformViewForTransition(-1, newRelativeIndex, IView::Probe(fl), animate);
            }

            mViewsMap.Find(index)->mSecond->mView->BringToFront();
        }
        mCurrentWindowStart = newWindowStart;
        mCurrentWindowEnd = newWindowEnd;
        mCurrentWindowStartUnbounded = newWindowStartUnbounded;

        if (mRemoteViewsAdapter) {
            Int32 adapterStart = Modulo(mCurrentWindowStart, adapterCount);
            Int32 adapterEnd = Modulo(mCurrentWindowEnd, adapterCount);
            mRemoteViewsAdapter->SetVisibleRangeHint(adapterStart, adapterEnd);
        }
    }
    RequestLayout();
    Invalidate();
}

void AdapterViewAnimator::ShowTapFeedback(
    /* [in] */ IView* child)
{
    child->SetPressed(TRUE);
}

void AdapterViewAnimator::HideTapFeedback(
    /* [in] */ IView* child)
{
    child->SetPressed(FALSE);
}

void AdapterViewAnimator::CancelHandleClick()
{
    AutoPtr<IView> v;// = GetCurrentView();
    if (v) {
        HideTapFeedback(v);
    }
    mTouchMode = TOUCH_MODE_NONE;
}

void AdapterViewAnimator::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthSpecSize = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSpecSize = MeasureSpec::GetSize(heightMeasureSpec);
    Int32 widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);

    Boolean haveChildRefSize = (mReferenceChildWidth != -1 && mReferenceChildHeight != -1);

    if (heightSpecMode == MeasureSpec::UNSPECIFIED) {
        heightSpecSize = haveChildRefSize ? mReferenceChildHeight + mPaddingTop + mPaddingBottom : 0;
    }
    else if (heightSpecMode == MeasureSpec::AT_MOST) {
        if (haveChildRefSize) {
            Int32 height = mReferenceChildHeight + mPaddingTop + mPaddingBottom;
            if (height > heightSpecSize) {
                heightSpecSize |= IView::MEASURED_STATE_TOO_SMALL;
            }
            else {
                heightSpecSize = height;
            }
        }
    }

    if (widthSpecMode == MeasureSpec::UNSPECIFIED) {
        widthSpecSize = haveChildRefSize ? mReferenceChildWidth + mPaddingLeft + mPaddingRight : 0;
    }
    else if (heightSpecMode == MeasureSpec::AT_MOST) {
        if (haveChildRefSize) {
            Int32 width = mReferenceChildWidth + mPaddingLeft + mPaddingRight;
            if (width > widthSpecSize) {
                widthSpecSize |= IView::MEASURED_STATE_TOO_SMALL;
            }
            else {
                widthSpecSize = width;
            }
        }
    }
    SetMeasuredDimension(widthSpecSize, heightSpecSize);
    MeasureChildren();
}

void AdapterViewAnimator::CheckForAndHandleDataChanged()
{
    Boolean dataChanged = mDataChanged;
    if (dataChanged) {
        AutoPtr<IRunnable> run = new CheckDataRun(this);
        Boolean resTmp;
        Post(run, &resTmp);
    }
    mDataChanged = FALSE;
}

ECode AdapterViewAnimator::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    CheckForAndHandleDataChanged();

    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 mw = 0, mh =0;
        child->GetMeasuredWidth(&mw);
        child->GetMeasuredHeight(&mh);
        Int32 childRight = mPaddingLeft + mw;
        Int32 childBottom = mPaddingTop + mh;

        child->Layout(mPaddingLeft, mPaddingTop, childRight, childBottom);
    }
    return NOERROR;
}

void AdapterViewAnimator::InitViewAnimator()
{
}

void AdapterViewAnimator::SetDisplayedChild(
    /* [in] */ Int32 whichChild,
    /* [in] */ Boolean animate)
{
    assert(0);
    if (mAdapter) {
        mWhichChild = whichChild;
        if (whichChild >= GetWindowSize()) {
            mWhichChild = mLoopViews ? 0 : GetWindowSize() - 1;
        }
        else if (whichChild < 0) {
            mWhichChild = mLoopViews ? GetWindowSize() - 1 : 0;
        }

        AutoPtr<IView> viewTmp;
        GetFocusedChild((IView**)&viewTmp);
        Boolean hasFocus =  viewTmp != NULL;

        ShowOnly(mWhichChild, animate);
        if (hasFocus) {
            Boolean resTmp;
            RequestFocus(IView::FOCUS_FORWARD, &resTmp);
        }
    }
}

AutoPtr<AdapterViewAnimator::ViewAndMetaData> AdapterViewAnimator::GetMetaDataForChild(
    /* [in] */ IView* child)
{
    //is right?
    HashMap< Int32, AutoPtr<ViewAndMetaData> >::Iterator it = mViewsMap.Begin();
    for (; it != mViewsMap.End(); it++) {
        if((it->mSecond->mView).Get() == child) {
            return it->mSecond;
        }
    }
    return NULL;
}

void AdapterViewAnimator::AddChild(
    /* [in] */ IView* child)
{
    AddViewInLayout(child, -1, CreateOrReuseLayoutParams(child));
    if (mReferenceChildWidth == -1 || mReferenceChildHeight == -1) {
        Int32 measureSpec = MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
        child->Measure(measureSpec, measureSpec);
        child->GetMeasuredWidth(&mReferenceChildWidth);
        child->GetMeasuredHeight(&mReferenceChildHeight);
    }
}

void AdapterViewAnimator::MeasureChildren()
{
    Int32 count = 0;
    GetChildCount(&count);
    Int32 measureWidth = 0;
    GetMeasuredWidth(&measureWidth);
    Int32 measureHeight = 0;
    GetMeasuredHeight(&measureHeight);
    Int32 childWidth = measureWidth - mPaddingLeft - mPaddingRight;
    Int32 childHeight = measureHeight - mPaddingTop - mPaddingBottom;

    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->Measure(MeasureSpec::MakeMeasureSpec(childWidth, MeasureSpec::EXACTLY),
            MeasureSpec::MakeMeasureSpec(childHeight, MeasureSpec::EXACTLY));
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos

