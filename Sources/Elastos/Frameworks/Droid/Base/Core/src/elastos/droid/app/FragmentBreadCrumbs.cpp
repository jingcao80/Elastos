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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/FragmentBreadCrumbs.h"
#include "elastos/droid/app/CBackStackRecord.h"
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//==============================================================================
// FragmentBreadCrumbs::MyOnClickListener
//==============================================================================
CAR_INTERFACE_IMPL(FragmentBreadCrumbs::MyOnClickListener, Object, IViewOnClickListener)

FragmentBreadCrumbs::MyOnClickListener::MyOnClickListener(
    /* [in] */ FragmentBreadCrumbs* host)
    : mHost(host)
{
}

ECode FragmentBreadCrumbs::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> obj;
    v->GetTag((IInterface**)&obj);
    IFragmentManagerBackStackEntry* bse = IFragmentManagerBackStackEntry::Probe(obj);
    if (bse) {
        if (bse == IFragmentManagerBackStackEntry::Probe(mHost->mParentEntry)) {
            if (mHost->mParentClickListener != NULL) {
                mHost->mParentClickListener->OnClick(v);
            }
        }
        else {
            if (mHost->mOnBreadCrumbClickListener != NULL) {
                Boolean bval;
                mHost->mOnBreadCrumbClickListener->OnBreadCrumbClick(
                    bse == IFragmentManagerBackStackEntry::Probe(mHost->mTopEntry) ? NULL : bse, 0, &bval);
                if (bval) {
                    return NOERROR;
                }
            }
            AutoPtr<IFragmentManager> mgr;
            mHost->mActivity->GetFragmentManager((IFragmentManager**)&mgr);
            if (bse == IFragmentManagerBackStackEntry::Probe(mHost->mTopEntry)) {
                // Pop everything off the back stack.
                mgr->PopBackStack();
            }
            else {
                Int32 id;
                bse->GetId(&id);
                mgr->PopBackStack(id, 0);
            }
        }
    }
    return NOERROR;
}

//==============================================================================
// FragmentBreadCrumbs
//==============================================================================
const String FragmentBreadCrumbs::TAG("FragmentBreadCrumbs");
const Int32 FragmentBreadCrumbs::DEFAULT_GRAVITY = IGravity::START | IGravity::CENTER_VERTICAL;

CAR_INTERFACE_IMPL_2(FragmentBreadCrumbs, ViewGroup, IFragmentBreadCrumbs, IFragmentManagerOnBackStackChangedListener)

FragmentBreadCrumbs::FragmentBreadCrumbs()
    : mActivity(NULL)
    , mMaxVisible(-1)
    , mGravity(0)
    , mLayoutResId(0)
    , mTextColor(0)
{}

FragmentBreadCrumbs::~FragmentBreadCrumbs()
{}

ECode FragmentBreadCrumbs::constructor(
    /*[in] */ IContext* ctx)
{
    return constructor(ctx, NULL);
}

ECode FragmentBreadCrumbs::constructor(
    /*[in] */ IContext* ctx,
    /*[in] */ IAttributeSet* attrs)
{
    return constructor(ctx, attrs, R::attr::fragmentBreadCrumbsStyle);
}

ECode FragmentBreadCrumbs::constructor(
    /*[in] */ IContext* ctx,
    /*[in] */ IAttributeSet* attrs,
     /*[in] */ Int32 defStyleAttr)
{
    return constructor(ctx, attrs, defStyleAttr, 0);
}

ECode FragmentBreadCrumbs::constructor(
    /*[in] */ IContext* context,
    /*[in] */ IAttributeSet* attrs,
    /*[in] */ Int32 defStyleAttr,
    /*[in] */ Int32 defStyleRes)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes))

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::FragmentBreadCrumbs);
    AutoPtr<ITypedArray>  a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetInt32(R::styleable::FragmentBreadCrumbs_gravity,
        DEFAULT_GRAVITY, &mGravity);
    a->GetResourceId(
        R::styleable::FragmentBreadCrumbs_itemLayout,
        R::layout::fragment_bread_crumb_item, &mLayoutResId);
    a->GetColor(
        R::styleable::FragmentBreadCrumbs_itemColor, 0, &mTextColor);

    a->Recycle();
    return NOERROR;
}

ECode FragmentBreadCrumbs::SetActivity(
    /* [in] */ IActivity* a)
{
    mActivity = a;
    AutoPtr<IInterface> obj;
    IContext::Probe(a)->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> view;
    mInflater->Inflate(
       R::layout::fragment_bread_crumbs, this, FALSE, (IView**)&view);
    mContainer = ILinearLayout::Probe(view);
    AddView(view);
    AutoPtr<IFragmentManager> mgr;
    a->GetFragmentManager((IFragmentManager**)&mgr);
    mgr->AddOnBackStackChangedListener(this);
    UpdateCrumbs();
    AutoPtr<ILayoutTransition> layoutTransition;
    CLayoutTransition::New((ILayoutTransition**)&layoutTransition);
    SetLayoutTransition(layoutTransition);
    return NOERROR;
}

ECode FragmentBreadCrumbs::SetMaxVisible(
    /* [in] */ Int32 visibleCrumbs)
{
    if (visibleCrumbs < 1) {
        Logger::E(TAG, "IllegalArgumentException: visibleCrumbs must be greater than zero");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMaxVisible = visibleCrumbs;
    return NOERROR;
}

ECode FragmentBreadCrumbs::SetParentTitle(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* shortTitle,
    /* [in] */ IViewOnClickListener* listener)
{
    mParentEntry = CreateBackStackEntry(title, shortTitle);
    mParentClickListener = listener;
    UpdateCrumbs();
    return NOERROR;
}

ECode FragmentBreadCrumbs::SetOnBreadCrumbClickListener(
    /* [in] */ IOnBreadCrumbClickListener* listener)
{
    mOnBreadCrumbClickListener = listener;
    return NOERROR;
}

AutoPtr<IBackStackRecord> FragmentBreadCrumbs::CreateBackStackEntry(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* shortTitle)
{
    if (title == NULL) return NULL;

    AutoPtr<IFragmentManager> mgr;
    mActivity->GetFragmentManager((IFragmentManager**)&mgr);
    AutoPtr<IBackStackRecord> entry;
    CBackStackRecord::New(IFragmentManagerImpl::Probe(mgr), (IBackStackRecord**)&entry);
    IFragmentTransaction::Probe(entry)->SetBreadCrumbTitle(title);
    IFragmentTransaction::Probe(entry)->SetBreadCrumbShortTitle(shortTitle);
    return entry;
}

ECode FragmentBreadCrumbs::SetTitle(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* shortTitle)
{
    mTopEntry = CreateBackStackEntry(title, shortTitle);
    return UpdateCrumbs();
}

ECode FragmentBreadCrumbs::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // Eventually we should implement our own layout of the views, rather than relying on
    // a single linear layout.
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount == 0) {
        return NOERROR;
    }

    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);

    Int32 mh, mw;
    child->GetMeasuredWidth(&mw);
    child->GetMeasuredHeight(&mh);
    Int32 childTop = mPaddingTop;
    Int32 childBottom = mPaddingTop + mh - mPaddingBottom;

    Int32 childLeft;
    Int32 childRight;

    Int32 layoutDirection;
    GetLayoutDirection(&layoutDirection);
    Int32 horizontalGravity = mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    Int32 ag;
    Gravity::GetAbsoluteGravity(horizontalGravity, layoutDirection, &ag);
    switch (ag) {
        case IGravity::RIGHT:
            childRight = mRight - mLeft - mPaddingRight;
            childLeft = childRight - mw;
            break;

        case IGravity::CENTER_HORIZONTAL:
            childLeft = mPaddingLeft + (mRight - mLeft - mw) / 2;
            childRight = childLeft + mw;
            break;

        case IGravity::LEFT:
        default:
            childLeft = mPaddingLeft;
            childRight = childLeft + mw;
            break;
    }

    if (childLeft < mPaddingLeft) {
        childLeft = mPaddingLeft;
    }

    if (childRight > mRight - mLeft - mPaddingRight) {
        childRight = mRight - mLeft - mPaddingRight;
    }

    child->Layout(childLeft, childTop, childRight, childBottom);
    return NOERROR;
}

ECode FragmentBreadCrumbs::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 count;
    GetChildCount(&count);

    Int32 maxHeight = 0;
    Int32 maxWidth = 0;
    Int32 measuredChildState = 0;
    Int32 state;

    using Elastos::Core::Math;

    Int32 mh, mw, visibility;
    // Find rightmost and bottom-most child
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->GetVisibility(&visibility);
        if (visibility != GONE) {
            MeasureChild(child, widthMeasureSpec, heightMeasureSpec);
            child->GetMeasuredWidth(&mw);
            child->GetMeasuredHeight(&mh);
            maxWidth = Math::Max(maxWidth, mw);
            maxHeight = Math::Max(maxHeight, mh);
            child->GetMeasuredState(&state);
            measuredChildState = CombineMeasuredStates(measuredChildState, state);
        }
    }

    // Account for padding too
    maxWidth += mPaddingLeft + mPaddingRight;
    maxHeight += mPaddingTop + mPaddingBottom;

    // Check against our minimum height and width
    maxHeight = Math::Max(maxHeight, GetSuggestedMinimumHeight());
    maxWidth = Math::Max(maxWidth, GetSuggestedMinimumWidth());

    SetMeasuredDimension(
        ResolveSizeAndState(maxWidth, widthMeasureSpec, measuredChildState),
        ResolveSizeAndState(maxHeight, heightMeasureSpec,
            measuredChildState << IView::MEASURED_HEIGHT_STATE_SHIFT));
    return NOERROR;
}

ECode FragmentBreadCrumbs::OnBackStackChanged()
{
    return UpdateCrumbs();
}

Int32 FragmentBreadCrumbs::GetPreEntryCount()
{
    return (mTopEntry != NULL ? 1 : 0) + (mParentEntry != NULL ? 1 : 0);
}

AutoPtr<IFragmentManagerBackStackEntry> FragmentBreadCrumbs::GetPreEntry(
    /* [in] */ Int32 index)
{
    // If there's a parent entry, then return that for zero'th item, else top entry.
    if (mParentEntry != NULL) {
        return index == 0 ?
            IFragmentManagerBackStackEntry::Probe(mParentEntry)
            : IFragmentManagerBackStackEntry::Probe(mTopEntry);
    }
    else {
        return IFragmentManagerBackStackEntry::Probe(mTopEntry);
    }
}

ECode FragmentBreadCrumbs::UpdateCrumbs()
{
    AutoPtr<IFragmentManager> fm;
    mActivity->GetFragmentManager((IFragmentManager**)&fm);
    Int32 numEntries, numPreEntries, numViews;
    fm->GetBackStackEntryCount(&numEntries);
    numPreEntries = GetPreEntryCount();
    IViewGroup* cvp = IViewGroup::Probe(mContainer);
    cvp->GetChildCount(&numViews);


    for (Int32 i = 0; i < numEntries + numPreEntries; i++) {
        AutoPtr<IFragmentManagerBackStackEntry> bse;
        if (i < numPreEntries) {
            bse = GetPreEntry(i);
        }
        else {
            fm->GetBackStackEntryAt(i - numPreEntries, (IFragmentManagerBackStackEntry**)&bse);
        }
        if (i < numViews) {
            AutoPtr<IView> v;
            cvp->GetChildAt(i, (IView**)&v);
            AutoPtr<IInterface> tag;
            v->GetTag((IInterface**)&tag);
            if (IFragmentManagerBackStackEntry::Probe(tag) != bse) {
                for (Int32 j = i; j < numViews; j++) {
                    cvp->RemoveViewAt(i);
                }
                numViews = i;
            }
        }

        if (i >= numViews) {
            AutoPtr<IView> item, tv, tmp;
            mInflater->Inflate(mLayoutResId, this, FALSE, (IView**)&item);
            item->FindViewById(R::id::title, (IView**)&tv);
            AutoPtr<ITextView> text = ITextView::Probe(tv);
            AutoPtr<ICharSequence> bct;
            bse->GetBreadCrumbTitle((ICharSequence**)&bct);
            text->SetText(bct);
            IView::Probe(text)->SetTag(bse.Get());
            text->SetTextColor(mTextColor);
            if (i == 0) {
                item->FindViewById(R::id::left_icon, (IView**)&tmp);
                tmp->SetVisibility(IView::GONE);
            }
            cvp->AddView(item);
            IView::Probe(text)->SetOnClickListener(mOnClickListener);
        }
    }

    Int32 viewI = numEntries + numPreEntries;
    cvp->GetChildCount(&numViews);
    while (numViews > viewI) {
        cvp->RemoveViewAt(numViews - 1);
        numViews--;
    }
    // Adjust the visibility and availability of the bread crumbs and divider
    for (Int32 i = 0; i < numViews; i++) {
        AutoPtr<IView> child;
        cvp->GetChildAt(i, (IView**)&child);
        // Disable the last one
        AutoPtr<IView> tmp;
        child->FindViewById(R::id::title, (IView**)&tmp);
        tmp->SetEnabled(i < numViews - 1);
        if (mMaxVisible > 0) {
            // Make only the last mMaxVisible crumbs visible
            child->SetVisibility(i < numViews - mMaxVisible ? IView::GONE : IView::VISIBLE);
            AutoPtr<IView> leftIcon;
            child->FindViewById(R::id::left_icon, (IView**)&leftIcon);
            // Remove the divider for all but the last mMaxVisible - 1
            leftIcon->SetVisibility(
                (i > numViews - mMaxVisible && i != 0) ? IView::VISIBLE : IView::GONE);
        }
    }
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
