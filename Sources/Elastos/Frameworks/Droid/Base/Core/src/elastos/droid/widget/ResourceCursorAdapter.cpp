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
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/ResourceCursorAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ResourceCursorAdapter, CursorAdapter, IResourceCursorAdapter);

ResourceCursorAdapter::ResourceCursorAdapter()
    : mLayout(0)
    , mDropDownLayout(0)
{}

ECode ResourceCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout,
    /* [in] */ ICursor* c)
{
    FAIL_RETURN(CursorAdapter::constructor(context, c));
    return InitImpl(context, layout);
}

ECode ResourceCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout,
    /* [in] */ ICursor* c,
    /* [in] */ Boolean autoRequery)
{
    FAIL_RETURN(CursorAdapter::constructor(context, c, autoRequery));
    return InitImpl(context, layout);
}

ECode ResourceCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 flags)
{
    FAIL_RETURN(CursorAdapter::constructor(context, c, flags));
    return InitImpl(context, layout);
}

ECode ResourceCursorAdapter::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layout)
{
    VALIDATE_NOT_NULL(context);

    mLayout = mDropDownLayout = layout;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    return NOERROR;
}

ECode ResourceCursorAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return mInflater->Inflate(mLayout, parent, FALSE, view);
}

ECode ResourceCursorAdapter::NewDropDownView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return mInflater->Inflate(mDropDownLayout, parent, FALSE, view);
}

ECode ResourceCursorAdapter::SetViewResource(
    /* [in] */ Int32 layout)
{
    mLayout = layout;
    return NOERROR;
}

ECode ResourceCursorAdapter::SetDropDownViewResource(
    /* [in] */ Int32 dropDownLayout)
{
    mDropDownLayout = dropDownLayout;
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Widget
