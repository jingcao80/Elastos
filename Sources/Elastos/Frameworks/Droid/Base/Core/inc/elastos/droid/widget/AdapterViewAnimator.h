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

#ifndef __ELASTOS_DROID_WIDGET_ADAPTERVIEWANIMATOR_H__
#define __ELASTOS_DROID_WIDGET_ADAPTERVIEWANIMATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/AdapterView.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::IAdapterViewAnimator;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Widget {

class AdapterViewAnimator
    : public AdapterView
    , public IAdapterViewAnimator
    , public IRemoteAdapterConnectionCallback
    , public IAdvanceable
{
public:
    class ViewAndMetaData
        : public Object
    {
    public:
        ViewAndMetaData(
            /* [in] */ IView* v,
            /* [in] */ Int32 relativeIndex,
            /* [in] */ Int32 adapterPosition,
            /* [in] */ Int64 itemId);

    public:
        AutoPtr<IView> mView;
        Int32 mRelativeIndex;
        Int32 mAdapterPosition;
        Int64 mItemId;
    };

    class CheckForTap
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        CheckForTap(
            /* [in] */ AdapterViewAnimator* host);

        CARAPI Run();

    private:
        AdapterViewAnimator* mHost;
    };

    class AdapterViewAnimatorSavedState
        : public View::BaseSavedState
        , public IAdapterViewAnimatorSavedState
    {
        friend class AdapterViewAnimator;
    public:
        CAR_INTERFACE_DECL()

        AdapterViewAnimatorSavedState();

        ~AdapterViewAnimatorSavedState();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ Int32 whichChild);

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    protected:
        Int32 mWhichChild;
    };

private:
    class ActionUpRun
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ActionUpRun(
            /* [in] */ AdapterViewAnimator* host,
            /* [in] */ ViewAndMetaData* data,
            /* [in] */ IView* v);

        CARAPI Run();

    private:
        AdapterViewAnimator* mHost;
        AutoPtr<ViewAndMetaData> mData;
        AutoPtr<IView> mView;
    };

    class ActionUpInner
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ActionUpInner(
            /* [in] */ AdapterViewAnimator* host,
            /* [in] */ ViewAndMetaData* data,
            /* [in] */ IView* v);

        CARAPI Run();

    private:
        AdapterViewAnimator* mHost;
        AutoPtr<ViewAndMetaData> mData;
        AutoPtr<IView> mView;
    };

    class CheckDataRun
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        CheckDataRun(
            /* [in] */ AdapterViewAnimator* host);

        CARAPI Run();

    private:
        AdapterViewAnimator* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AdapterViewAnimator();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI SetDisplayedChild(
        /* [in] */ Int32 whichChild);

    virtual CARAPI GetDisplayedChild(
        /* [out] */ Int32* result);

    virtual CARAPI ShowNext();

    virtual CARAPI ShowPrevious();

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    // @Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    virtual CARAPI GetCurrentView(
        /* [out] */ IView** result);

    virtual CARAPI GetInAnimation(
        /* [out] */ IObjectAnimator** result);

    virtual CARAPI SetInAnimation(
        /* [in] */ IObjectAnimator* inAnimation);

    virtual CARAPI SetInAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceID);

    virtual CARAPI GetOutAnimation(
        /* [out] */ IObjectAnimator** result);

    virtual CARAPI SetOutAnimation(
        /* [in] */ IObjectAnimator* outAnimation);

    virtual CARAPI SetOutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceID);

    virtual CARAPI SetAnimateFirstView(
        /* [in] */ Boolean animate);

    // @Override
    CARAPI GetBaseline(
        /* [out] */ Int32* baseline);

    // @Override
    CARAPI GetAdapter(
        /* [out] */ IAdapter** result);

    // @Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    virtual CARAPI SetRemoteViewsAdapter(
        /* [in] */ IIntent* intent);

    virtual CARAPI SetRemoteViewsOnClickHandler(
        /* [in] */ IRemoteViewsOnClickHandler* handler);

    // @Override
    CARAPI SetSelection(
        /* [in] */ Int32 position);

    // @Override
    CARAPI GetSelectedView(
        /* [out] */ IView** result);

    virtual CARAPI DeferNotifyDataSetChanged();

    virtual CARAPI OnRemoteAdapterConnected(
        /* [out] */ Boolean* result);

    virtual CARAPI OnRemoteAdapterDisconnected();

    virtual CARAPI Advance();

    virtual CARAPI FyiWillBeAdvancedByHostKThx();

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI_(void) ConfigureViewAnimator(
        /* [in] */ Int32 numVisibleViews,
        /* [in] */ Int32 activeOffset);

    virtual CARAPI_(void) TransformViewForTransition(
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 toIndex,
        /* [in] */ IView* v,
        /* [in] */ Boolean animate);

    virtual CARAPI_(AutoPtr<IObjectAnimator>) GetDefaultInAnimation();

    virtual CARAPI_(AutoPtr<IObjectAnimator>) GetDefaultOutAnimation();

    virtual CARAPI_(void) ApplyTransformForChildAtIndex(
        /* [in] */ IView* child,
        /* [in] */ Int32 relativeIndex);

    virtual CARAPI_(Int32) Modulo(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 size);

    virtual CARAPI_(AutoPtr<IView>) GetViewAtRelativeIndex(
        /* [in] */ Int32 relativeIndex);

    virtual CARAPI_(Int32) GetNumActiveViews();

    virtual CARAPI_(Int32) GetWindowSize();

    virtual CARAPI_(AutoPtr<IViewGroupLayoutParams>) CreateOrReuseLayoutParams(
        /* [in] */ IView* v);

    virtual CARAPI_(void) RefreshChildren();

    virtual CARAPI_(AutoPtr<IFrameLayout>) GetFrameForChild();

    virtual CARAPI_(void) ShowOnly(
        /* [in] */ Int32 childIndex,
        /* [in] */ Boolean animate);

    virtual CARAPI_(void) ShowTapFeedback(
        /* [in] */ IView* child);

    virtual CARAPI_(void) HideTapFeedback(
        /* [in] */ IView* child);

    virtual CARAPI_(void) CancelHandleClick();

    virtual CARAPI_(void) CheckForAndHandleDataChanged();

protected:
    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) InitViewAnimator();

    CARAPI_(void) SetDisplayedChild(
        /* [in] */ Int32 whichChild,
        /* [in] */ Boolean animate);

    CARAPI_(AutoPtr<ViewAndMetaData>) GetMetaDataForChild(
        /* [in] */ IView* child);

    CARAPI_(void) AddChild(
        /* [in] */ IView* child);

    using AdapterView::MeasureChildren;

    CARAPI_(void) MeasureChildren();

public:
    static const Int32 TOUCH_MODE_NONE = 0;
    static const Int32 TOUCH_MODE_DOWN_IN_CURRENT_VIEW = 1;
    static const Int32 TOUCH_MODE_HANDLED = 2;
    Int32 mWhichChild;
    Boolean mAnimateFirstTime;
    Int32 mActiveOffset;
    Int32 mMaxNumActiveViews;
    Int32 mCurrentWindowStart;
    Int32 mCurrentWindowEnd;
    Int32 mCurrentWindowStartUnbounded;

    AutoPtr<AdapterDataSetObserver> mDataSetObserver;
    AutoPtr<IAdapter> mAdapter;
    AutoPtr<IRemoteViewsAdapter> mRemoteViewsAdapter;

    Boolean mDeferNotifyDataSetChanged;
    Boolean mFirstTime;
    Boolean mLoopViews;

    Int32 mReferenceChildWidth;
    Int32 mReferenceChildHeight;

    AutoPtr<IObjectAnimator> mInAnimation;
    AutoPtr<IObjectAnimator> mOutAnimation;

    HashMap< Int32, AutoPtr<ViewAndMetaData> > mViewsMap;
    List<Int32> mPreviousViews;

private:
    static const String TAG;
    static const Int32 DEFAULT_ANIMATION_DURATION = 200;
    AutoPtr<IRunnable> mPendingCheckForTap;
    Int32 mRestoreWhichChild;
    Int32 mTouchMode; //= TOUCH_MODE_NONE;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_ADAPTERVIEWANIMATOR_H__

