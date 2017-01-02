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
#include "elastos/droid/app/ActionBarLayoutParams.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::IGravity;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(ActionBarLayoutParams, ViewGroup::MarginLayoutParams, IActionBarLayoutParams)

ActionBarLayoutParams::ActionBarLayoutParams()
    : mGravity(IGravity::NO_GRAVITY)
{}

ActionBarLayoutParams::~ActionBarLayoutParams()
{}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    MarginLayoutParams::constructor(ctx, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ActionBar_LayoutParams);

    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetInt32(R::styleable::ActionBar_LayoutParams_layout_gravity,
        IGravity::NO_GRAVITY, &mGravity);
    a->Recycle();
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    MarginLayoutParams::constructor(width, height);
    mGravity = IGravity::CENTER_VERTICAL | IGravity::START;
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 gravity)
{
    MarginLayoutParams::constructor(width, height);
    mGravity = gravity;
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ Int32 gravity)
{
    MarginLayoutParams::constructor(IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::MATCH_PARENT);
    mGravity = gravity;
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ IActionBarLayoutParams* ablp)
{
    MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(ablp));
    ablp->GetGravity(&mGravity);
    return NOERROR;
}

ECode ActionBarLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* vglp)
{
    MarginLayoutParams::constructor(vglp);
    return NOERROR;
}

ECode ActionBarLayoutParams::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity)
    *gravity = mGravity;
    return NOERROR;
}

ECode ActionBarLayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
