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

#include "elastos/droid/systemui/statusbar/CEmptyShadeView.h"
#include "R.h"

using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CEmptyShadeView)
CAR_INTERFACE_IMPL(CEmptyShadeView, StackScrollerDecorView, IEmptyShadeView)
ECode CEmptyShadeView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StackScrollerDecorView::constructor(context, attrs);
}

ECode CEmptyShadeView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    StackScrollerDecorView::OnConfigurationChanged(newConfig);
    AutoPtr<IView> view;
    FindViewById(R::id::no_notifications, (IView**)&view);
    ITextView::Probe(view)->SetText(R::string::empty_shade_text);
    return NOERROR;
}

AutoPtr<IView> CEmptyShadeView::FindContentView()
{
    AutoPtr<IView> view;
    FindViewById(R::id::no_notifications, (IView**)&view);
    return view;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
