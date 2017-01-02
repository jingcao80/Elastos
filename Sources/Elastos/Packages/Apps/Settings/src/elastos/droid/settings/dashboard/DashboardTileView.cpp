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

#include "elastos/droid/settings/dashboard/DashboardTileView.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "../R.h"

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::LayoutInflater;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

//===============================================================================
//                  DashboardTileView::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(DashboardTileView::InnerListener, Object, IViewOnClickListener)

DashboardTileView::InnerListener::InnerListener(
    /* [in] */ DashboardTileView* host)
    : mHost(host)
{}

ECode DashboardTileView::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//===============================================================================
//                  DashboardTileView
//===============================================================================

const Int32 DashboardTileView::DEFAULT_COL_SPAN = 1;

DashboardTileView::DashboardTileView()
    : mColSpan(DEFAULT_COL_SPAN)
{}

DashboardTileView::~DashboardTileView()
{}

ECode DashboardTileView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode DashboardTileView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::dashboard_tile, this, (IView**)&view);

    AutoPtr<IView> _view;
    view->FindViewById(R::id::icon, (IView**)&_view);
    mImageView = IImageView::Probe(_view);
    _view = NULL;
    view->FindViewById(R::id::title, (IView**)&_view);
    mTitleTextView = ITextView::Probe(_view);
    _view = NULL;
    view->FindViewById(R::id::status, (IView**)&_view);
    mStatusTextView = ITextView::Probe(_view);
    view->FindViewById(R::id::tile_divider, (IView**)&mDivider);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    SetOnClickListener(listener);
    SetBackgroundResource(R::drawable::dashboard_tile_background);
    SetFocusable(TRUE);
    return NOERROR;
}

AutoPtr<ITextView> DashboardTileView::GetTitleTextView()
{
    return mTitleTextView;
}

AutoPtr<ITextView> DashboardTileView::GetStatusTextView()
{
    return mStatusTextView;
}

AutoPtr<IImageView> DashboardTileView::GetImageView()
{
    return mImageView;
}

ECode DashboardTileView::SetTile(
    /* [in] */ CDashboardTile* tile)
{
    mTile = tile;
    return NOERROR;
}

ECode DashboardTileView::SetDividerVisibility(
    /* [in] */ Boolean visible)
{
    mDivider->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

void DashboardTileView::SetColumnSpan(
    /* [in] */ Int32 span)
{
    mColSpan = span;
}

Int32 DashboardTileView::GetColumnSpan()
{
    return mColSpan;
}

ECode DashboardTileView::OnClick(
    /* [in] */ IView* v)
{
    if (!mTile->mFragment.IsNull()) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        Utils::StartWithFragment(context, mTile->mFragment, mTile->mFragmentArguments, NULL, 0,
                mTile->mTitleRes, mTile->GetTitle(res));
    }
    else if (mTile->mIntent != NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->StartActivity(mTile->mIntent);
    }
    return NOERROR;
}

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos