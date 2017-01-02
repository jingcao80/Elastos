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

#include "elastos/droid/app/ListFragment.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

using Elastos::Droid::R;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace App {

ECode ListFragment::MyRunnable::Run()
{
    IViewParent::Probe(mHost->mList)->FocusableViewAvailable(IView::Probe(mHost->mList));
    return NOERROR;
}

CAR_INTERFACE_IMPL(ListFragment::MyOnItemClickListener, Object, IAdapterViewOnItemClickListener)

ECode ListFragment::MyOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnListItemClick(IListView::Probe(parent), view, position, id);
}


CAR_INTERFACE_IMPL(ListFragment, Fragment, IListFragment)

ListFragment::ListFragment()
    : mAdapter(NULL)
    , mList(NULL)
    , mEmptyView(NULL)
    , mStandardEmptyView(NULL)
    , mProgressContainer(NULL)
    , mListContainer(NULL)
    , mEmptyText(NULL)
    , mListShown(FALSE)
    , mRequestFocus(new MyRunnable(this))
    , mOnClickListener(new MyOnItemClickListener(this))
{
    CHandler::New((IHandler**)&mHandler);
}

ListFragment::~ListFragment()
{}

ECode ListFragment::constructor()
{
    return Fragment::constructor();
}

ECode ListFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    return inflater->Inflate(R::layout::list_content,
            container, FALSE, view);
}

ECode ListFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Fragment::OnViewCreated(view, savedInstanceState))
    FAIL_RETURN(EnsureList())
    return NOERROR;
}

ECode ListFragment::OnDestroyView()
{
    mHandler->RemoveCallbacks(mRequestFocus);
    mList = NULL;
    mListShown = FALSE;
    mEmptyView = mProgressContainer = mListContainer = NULL;
    mStandardEmptyView = NULL;
    Fragment::OnDestroyView();
    return NOERROR;
}

ECode ListFragment::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return NOERROR;
}

ECode ListFragment::SetListAdapter(
    /* [in] */ IListAdapter* adapter)
{
    Boolean hadAdapter = mAdapter != NULL;
    mAdapter = adapter;
    if (mList != NULL) {
        IAdapterView::Probe(mList)->SetAdapter(IAdapter::Probe(adapter));
        if (!mListShown && !hadAdapter) {
            // The list was hidden, and previously didn't have an
            // adapter.  It is now time to show it.
            AutoPtr<IView> view;
            GetView((IView**)&view);
            AutoPtr<IBinder> token;
            view->GetWindowToken((IBinder**)&token);
            FAIL_RETURN(SetListShown(TRUE, token != NULL))
        }
    }
    return NOERROR;
}

ECode ListFragment::SetSelection(
    /* [in] */ Int32 position)
{
    FAIL_RETURN(EnsureList())
    IAdapterView::Probe(mList)->SetSelection(position);
    return NOERROR;
}

ECode ListFragment::GetSelectedItemPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);

    FAIL_RETURN(EnsureList())
    IAdapterView::Probe(mList)->GetSelectedItemPosition(position);
    return NOERROR;
}

ECode ListFragment::GetSelectedItemId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    FAIL_RETURN(EnsureList())
    IAdapterView::Probe(mList)->GetSelectedItemId(id);
    return NOERROR;
}

ECode ListFragment::GetListView(
    /* [out] */ IListView** listview)
{
    VALIDATE_NOT_NULL(listview);

    FAIL_RETURN(EnsureList())
    *listview = mList;
    REFCOUNT_ADD(*listview);
    return NOERROR;
}

ECode ListFragment::SetEmptyText(
    /* [in] */ ICharSequence* text)
{
    FAIL_RETURN(EnsureList())
    if (mStandardEmptyView == NULL) {
        // throw new IllegalStateException("Can't be used with a custom content view");
        Slogger::E("ListFragment", "Can't be used with a custom content view");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mStandardEmptyView->SetText(text);
    if (mEmptyText == NULL) {
        IAdapterView::Probe(mList)->SetEmptyView(IView::Probe(mStandardEmptyView));
    }
    mEmptyText = text;
    return NOERROR;
}

ECode ListFragment::SetListShown(
    /* [in] */ Boolean shown)
{
    return SetListShown(shown, TRUE);
}

ECode ListFragment::SetListShownNoAnimation(
    /* [in] */ Boolean shown)
{
    return SetListShown(shown, FALSE);
}

ECode ListFragment::SetListShown(
    /* [in] */ Boolean shown,
    /* [in] */ Boolean animate)
{
    FAIL_RETURN(EnsureList())
    if (mProgressContainer == NULL) {
        // throw new IllegalStateException("Can't be used with a custom content view");
        Slogger::E("ListFragment", "Can't be used with a custom content view");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mListShown == shown) {
        return NOERROR;
    }

    mListShown = shown;
    if (shown) {
        if (animate) {
            AutoPtr<IAnimation> aOut, aIn;
            AnimationUtils::LoadAnimation(IContext::Probe(mActivity), R::anim::fade_out, (IAnimation**)&aOut);
            AnimationUtils::LoadAnimation(IContext::Probe(mActivity), R::anim::fade_in, (IAnimation**)&aIn);
            mProgressContainer->StartAnimation(aOut);
            mListContainer->StartAnimation(aIn);
        }
        else {
            mProgressContainer->ClearAnimation();
            mListContainer->ClearAnimation();
        }
        mProgressContainer->SetVisibility(IView::GONE);
        mListContainer->SetVisibility(IView::VISIBLE);
    }
    else {
        if (animate) {
            AutoPtr<IAnimation> aOut, aIn;
            AnimationUtils::LoadAnimation(IContext::Probe(mActivity), R::anim::fade_out, (IAnimation**)&aOut);
            AnimationUtils::LoadAnimation(IContext::Probe(mActivity), R::anim::fade_in, (IAnimation**)&aIn);
            mProgressContainer->StartAnimation(aIn);
            mListContainer->StartAnimation(aOut);
        }
        else {
            mProgressContainer->ClearAnimation();
            mListContainer->ClearAnimation();
        }
        mProgressContainer->SetVisibility(IView::VISIBLE);
        mListContainer->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode ListFragment::GetListAdapter(
    /* [out] */ IListAdapter** listadapter)
{
    VALIDATE_NOT_NULL(listadapter);

    *listadapter = mAdapter;
    REFCOUNT_ADD(*listadapter);
    return NOERROR;
}

ECode ListFragment::EnsureList()
{
    if (mList != NULL) {
        return NOERROR;
    }
    AutoPtr<IView> root;
    GetView((IView**)&root);
    if (root == NULL) {
        // throw new IllegalStateException("Content view not yet created");
        Slogger::E("ListFragment", "Content view not yet created");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    IListView* listView = IListView::Probe(root);
    if (listView) {
        mList = listView;
    }
    else {
        AutoPtr<IView> view;
        root->FindViewById(R::id::internalEmpty, (IView**)&view);
        mStandardEmptyView = ITextView::Probe(view);
        if (mStandardEmptyView == NULL) {
            mEmptyView = NULL;
            root->FindViewById(R::id::empty, (IView**)&mEmptyView);
        }
        else {
            IView::Probe(mStandardEmptyView)->SetVisibility(IView::GONE);
        }

        mProgressContainer = NULL;
        mListContainer = NULL;
        root->FindViewById(R::id::progressContainer, (IView**)&mProgressContainer);
        root->FindViewById(R::id::listContainer, (IView**)&mListContainer);
        AutoPtr<IView> rawListView;
        root->FindViewById(R::id::list, (IView**)&rawListView);
        if (IListView::Probe(rawListView) == NULL) {
            // throw new RuntimeException(
            //         "Content has view with id attribute 'android.R.id.list' "
            //         + "that is not a ListView class");
            Slogger::E("ListFragment", "Content has view with id attribute 'android.R.id.list' that is not a ListView class");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        mList = IListView::Probe(rawListView);
        if (mList == NULL) {
            // throw new RuntimeException(
            //         "Your content must have a ListView whose id attribute is " +
            //         "'android.R.id.list'");
            Slogger::E("ListFragment", "Your content must have a ListView whose id attribute is 'android.R.id.list'");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (mEmptyView != NULL) {
            IAdapterView::Probe(mList)->SetEmptyView(mEmptyView);
        }
        else if (mEmptyText != NULL) {
            mStandardEmptyView->SetText(mEmptyText);
            IAdapterView::Probe(mList)->SetEmptyView(IView::Probe(mStandardEmptyView));
        }
    }
    mListShown = TRUE;
    IAdapterView::Probe(mList)->SetOnItemClickListener(mOnClickListener);
    if (mAdapter != NULL) {
        AutoPtr<IListAdapter> adapter = mAdapter;
        mAdapter = NULL;
        SetListAdapter(adapter);
    } else {
        // We are starting without an adapter, so assume we won't
        // have our data right away and start with the progress indicator.
        if (mProgressContainer != NULL) {
            FAIL_RETURN(SetListShown(FALSE, FALSE))
        }
    }
    Boolean result;
    mHandler->Post(mRequestFocus.Get(), &result);
    return NOERROR;
}

} //namespace App
} //namespace Droid
} //namespace Elastos
