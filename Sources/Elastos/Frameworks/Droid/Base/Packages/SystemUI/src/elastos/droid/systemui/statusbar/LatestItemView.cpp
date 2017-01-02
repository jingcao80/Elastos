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

#include "elastos/droid/systemui/statusbar/LatestItemView.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"

using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


LatestItemView::LatestItemView()
{}

LatestItemView::LatestItemView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
{}

ECode LatestItemView::SetOnClickListener(
    /* [in] */ IViewOnClickListener* l)
{
    return FrameLayout::SetOnClickListener(l);
}

Boolean LatestItemView::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event)
{
    Boolean result = FrameLayout::OnRequestSendAccessibilityEvent(child, event);
    if (result) {
        // Add a record for the entire layout since its content is somehow small.
        // The event comes from a leaf view that is interacted with.
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> record;
        helper->Obtain((IAccessibilityEvent**)&record);
        OnInitializeAccessibilityEvent(record);
        DispatchPopulateAccessibilityEvent(record);
        event->AppendRecord(record);
        return TRUE;
    }

    return FALSE;
}


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
