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

#include "elastos/droid/systemui/statusbar/phone/CKeyguardIndicationTextView.h"
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CKeyguardIndicationTextView)

CAR_INTERFACE_IMPL(CKeyguardIndicationTextView, TextView, IKeyguardIndicationTextView)

ECode CKeyguardIndicationTextView::SwitchIndication(
    /* [in] */ ICharSequence* text)
{
    // TODO: Animation, make sure that we will show one indication long enough.
    if (TextUtils::IsEmpty(text)) {
        SetVisibility(IView::INVISIBLE);
    }
    else {
        SetVisibility(IView::VISIBLE);
        SetText(text);
    }
    return NOERROR;
}

ECode CKeyguardIndicationTextView::SwitchIndication(
    /* [in] */ Int32 textResId)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ICharSequence> text;
    res->GetText(textResId, (ICharSequence**)&text);
    SwitchIndication(text);
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
