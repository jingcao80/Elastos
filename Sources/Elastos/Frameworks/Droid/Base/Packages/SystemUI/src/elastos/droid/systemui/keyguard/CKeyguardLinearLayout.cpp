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

#include "elastos/droid/systemui/keyguard/CKeyguardLinearLayout.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CKeyguardLinearLayout)

CAR_INTERFACE_IMPL(CKeyguardLinearLayout, LinearLayout, IKeyguardLinearLayout)

CKeyguardLinearLayout::CKeyguardLinearLayout()
    : mTopChild(0)
{
}

ECode CKeyguardLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}

ECode CKeyguardLinearLayout::SetTopChild(
    /* [in] */ IView* child)
{
    Int32 top;
    IndexOfChild(child, &top);
    mTopChild = top;
    return Invalidate();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
