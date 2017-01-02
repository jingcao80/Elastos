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

#include "elastos/droid/server/telecom/MultiLineTitleEditTextPreference.h"
#include <Elastos.Droid.View.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

ECode MultiLineTitleEditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return EditTextPreference::constructor(context, attrs, defStyle);
}

ECode MultiLineTitleEditTextPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return EditTextPreference::constructor(context, attrs);
 }

ECode MultiLineTitleEditTextPreference::constructor(
    /* [in] */ IContext* context)
{
    return EditTextPreference::constructor(context);
}

ECode MultiLineTitleEditTextPreference::OnBindView(
    /* [in] */ IView* view)
{
    EditTextPreference::OnBindView(view);
    AutoPtr<IView> titleView;
    view->FindViewById(Elastos::Droid::R::id::title, (IView**)&titleView);
    AutoPtr<ITextView> textView = ITextView::Probe(titleView);
    if (textView != NULL) {
        textView->SetSingleLine(FALSE);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
