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

#include "elastos/droid/systemui/statusbar/CDismissView.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CDismissView)
CAR_INTERFACE_IMPL(CDismissView, StackScrollerDecorView, IDismissView)
ECode CDismissView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StackScrollerDecorView::constructor(context, attrs);
}

AutoPtr<IView> CDismissView::FindContentView()
{
    AutoPtr<IView> view;
    FindViewById(R::id::dismiss_text, (IView**)&view);
    return view;
}

ECode CDismissView::SetOnButtonClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    mContent->SetOnClickListener(listener);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
