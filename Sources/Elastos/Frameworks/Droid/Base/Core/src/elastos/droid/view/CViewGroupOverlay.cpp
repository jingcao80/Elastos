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

#include "elastos/droid/view/CViewGroupOverlay.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CViewGroupOverlay);
CAR_INTERFACE_IMPL(CViewGroupOverlay, ViewOverlay, IViewGroupOverlay);
ECode CViewGroupOverlay::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* hostView)
{
    return ViewOverlay::constructor(context, hostView);
}

ECode CViewGroupOverlay::Add(
    /* [in] */ IView* view)
{
    return mOverlayViewGroup->Add(view);
}

ECode CViewGroupOverlay::Remove(
    /* [in] */ IView* view)
{
    return mOverlayViewGroup->Remove(view);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
