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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeProvider.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(AccessibilityNodeProvider, Object, IAccessibilityNodeProvider)

ECode AccessibilityNodeProvider::CreateAccessibilityNodeInfo(
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IAccessibilityNodeInfo** nodeInfo)
{
    VALIDATE_NOT_NULL(nodeInfo);
    *nodeInfo = NULL;
    return NOERROR;
}
ECode AccessibilityNodeProvider::PerformAction(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode AccessibilityNodeProvider::FindAccessibilityNodeInfosByText(
    /* [in] */ const String& text,
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IList** nodeInfos)
{
    VALIDATE_NOT_NULL(nodeInfos);
    *nodeInfos = NULL;
    return NOERROR;
}

ECode AccessibilityNodeProvider::FindFocus(
    /* [in] */ Int32 focus,
    /* [out] */  IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
