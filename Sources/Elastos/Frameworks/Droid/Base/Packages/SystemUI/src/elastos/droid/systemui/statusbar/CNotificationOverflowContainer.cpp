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

#include "elastos/droid/systemui/statusbar/CNotificationOverflowContainer.h"
#include "R.h"

using Elastos::Droid::SystemUI::StatusBar::Phone::IIconMerger;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CNotificationOverflowContainer)

CAR_INTERFACE_IMPL(CNotificationOverflowContainer, ActivatableNotificationView, INotificationOverflowContainer)

ECode CNotificationOverflowContainer::OnFinishInflate()
{
    ActivatableNotificationView::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::overflow_icons_view, (IView**)&view);
    mIconsView = INotificationOverflowIconsView::Probe(view);

    view = NULL;
    FindViewById(R::id::more_text, (IView**)&view);
    mIconsView->SetMoreText(ITextView::Probe(view));

    view = NULL;
    FindViewById(R::id::more_icon_overflow, (IView**)&view);
    IIconMerger::Probe(mIconsView)->SetOverflowIndicator(view);
    return NOERROR;
}

ECode CNotificationOverflowContainer::GetIconsView(
    /* [out] */ INotificationOverflowIconsView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mIconsView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
