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

#include "elastos/droid/launcher2/CAccessibleTabView.h"
#include "elastos/droid/launcher2/FocusHelper.h"
#include "Elastos.Droid.Service.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CAccessibleTabView, TextView, IAccessibleTabView);

CAR_OBJECT_IMPL(CAccessibleTabView);

ECode CAccessibleTabView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if(FocusHelper::HandleTabKeyEvent(IAccessibleTabView::Probe(this), keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    TextView::OnKeyDown(keyCode, event, &res);
    *result = res;
    return NOERROR;
}

ECode CAccessibleTabView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (FocusHelper::HandleTabKeyEvent(IAccessibleTabView::Probe(this), keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    TextView::OnKeyUp(keyCode, event, &res);
    *result = res;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos