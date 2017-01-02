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

#include "elastos/droid/launcher2/HandleView.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Int32 HandleView::ORIENTATION_HORIZONTAL = 1;

HandleView::HandleView()
    : mOrientation(ORIENTATION_HORIZONTAL)
{
}

ECode HandleView::constructor(
    /* [in] */ IContext* context)
{
    return ImageView::constructor(context);
}

ECode HandleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode HandleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ImageView::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds =
        TO_ATTRS_ARRAYOF(Elastos::Droid::Launcher2::R::styleable::HandleView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);
    a->GetInt32(Elastos::Droid::Launcher2::R::styleable::HandleView_direction,
            ORIENTATION_HORIZONTAL, &mOrientation);
    a->Recycle();

    String str;
    context->GetString(Elastos::Droid::Launcher2::R::string::all_apps_button_label, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    return SetContentDescription(cchar);
}

ECode HandleView::FocusSearch(
    /* [in] */ Int32 direction,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> newFocus;
    ImageView::FocusSearch(direction, (IView**)&newFocus);
    Boolean res;
    mLauncher->IsAllAppsVisible(&res);
    if (newFocus == NULL && !res) {
        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        Boolean tmp;
        IView::Probe(workspace)->DispatchUnhandledMove(NULL, direction, &tmp);
        if (mOrientation == ORIENTATION_HORIZONTAL && direction == FOCUS_DOWN) {
            *view = IView::Probe(this);
            REFCOUNT_ADD(*view);
        }
        else {
            *view = IView::Probe(workspace);
            REFCOUNT_ADD(*view);
        }
        return NOERROR;
    }
    *view = newFocus;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode HandleView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action;
    ev->GetAction(&action);
    Boolean res;
    mLauncher->IsAllAppsVisible(&res);
    if (action == IMotionEvent::ACTION_DOWN && res) {
        *result = FALSE;
        return NOERROR;
    }
    return ImageView::OnTouchEvent(ev, result);
}

ECode HandleView::SetLauncher(
    /* [in] */ ILauncher* launcher)
{
    mLauncher = launcher;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos