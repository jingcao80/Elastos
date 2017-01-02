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

#include "elastos/droid/systemui/FontSizeUtils.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::ITypedValue;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const Float FontSizeUtils::LARGE_TEXT_SCALE = 1.3f;
void FontSizeUtils::UpdateFontSize(
    /* [in] */ IView* parent,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 dimensId)
{
    AutoPtr<IView> view;
    parent->FindViewById(viewId, (IView**)&view);
    UpdateFontSize(ITextView::Probe(view), dimensId);
}

void FontSizeUtils::UpdateFontSize(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 dimensId)
{
    if (v != NULL) {
        AutoPtr<IResources> res;
        IView::Probe(v)->GetResources((IResources**)&res);
        Int32 size = 0;
        res->GetDimensionPixelSize(dimensId, &size);
        v->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, size);
    }
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
