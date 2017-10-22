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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/ResourceCursorTreeAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ResourceCursorTreeAdapter, CursorTreeAdapter, IResourceCursorTreeAdapter);
ResourceCursorTreeAdapter::ResourceCursorTreeAdapter()
    : mCollapsedGroupLayout(0)
    , mExpandedGroupLayout(0)
    , mChildLayout(0)
    , mLastChildLayout(0)
{}

ECode ResourceCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 collapsedGroupLayout,
    /* [in] */ Int32 expandedGroupLayout,
    /* [in] */ Int32 childLayout,
    /* [in] */ Int32 lastChildLayout)
{
    CursorTreeAdapter::constructor(cursor, context);
    mCollapsedGroupLayout = collapsedGroupLayout;
    mExpandedGroupLayout = expandedGroupLayout;
    mChildLayout = childLayout;
    mLastChildLayout = lastChildLayout;

    AutoPtr<IInterface> sTemp;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&sTemp);
    mInflater = ILayoutInflater::Probe(sTemp);
    return NOERROR;
}

ECode ResourceCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 collapsedGroupLayout,
    /* [in] */ Int32 expandedGroupLayout,
    /* [in] */ Int32 childLayout)
{
    return constructor(context, cursor, collapsedGroupLayout, expandedGroupLayout, childLayout, childLayout);
}

ECode ResourceCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 groupLayout,
    /* [in] */ Int32 childLayout)
{
    return constructor(context, cursor, groupLayout, groupLayout, childLayout, childLayout);
}

AutoPtr<IView> ResourceCursorTreeAdapter::NewChildView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isLastChild,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    mInflater->Inflate((isLastChild) ? mLastChildLayout : mChildLayout, parent, FALSE, (IView**)&view);
    return view;
}

AutoPtr<IView> ResourceCursorTreeAdapter::NewGroupView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isExpanded,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    mInflater->Inflate((isExpanded) ? mExpandedGroupLayout : mCollapsedGroupLayout, parent, FALSE, (IView**)&view);
    return view;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
