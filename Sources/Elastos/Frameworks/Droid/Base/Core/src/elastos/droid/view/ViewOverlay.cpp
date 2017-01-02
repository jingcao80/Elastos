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

#include "elastos/droid/view/ViewOverlay.h"
#include "elastos/droid/view/COverlayViewGroup.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(ViewOverlay, Object, IViewOverlay);
ECode ViewOverlay::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* hostView)
{
    COverlayViewGroup::New(context, hostView, (IOverlayViewGroup**)&mOverlayViewGroup);
    return NOERROR;
}

ECode ViewOverlay::GetOverlayView(
    /* [out] */ IViewGroup** group)
{
    VALIDATE_NOT_NULL(group);
    *group = IViewGroup::Probe(mOverlayViewGroup);
    REFCOUNT_ADD(*group);
    return NOERROR;
}

ECode ViewOverlay::Add(
    /* [in] */ IDrawable* drawable)
{
    return mOverlayViewGroup->Add(drawable);
}

ECode ViewOverlay::Remove(
    /* [in] */ IDrawable* drawable)
{
    return mOverlayViewGroup->Remove(drawable);
}

ECode ViewOverlay::Clear()
{
    return mOverlayViewGroup->Clear();
}

ECode ViewOverlay::IsEmpty(
    /* [out] */ Boolean* empyt)
{
    VALIDATE_NOT_NULL(empyt);
    return mOverlayViewGroup->IsEmpty(empyt);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
