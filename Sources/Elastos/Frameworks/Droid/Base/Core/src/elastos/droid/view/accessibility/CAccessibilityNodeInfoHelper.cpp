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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityNodeInfoHelper, Singleton, IAccessibilityNodeInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityNodeInfoHelper)

ECode CAccessibilityNodeInfoHelper::GetAccessibilityViewId(
    /* [in] */ Int64 accessibilityNodeId,
    /* [out] */ Int32* viewId)
{
    VALIDATE_NOT_NULL(viewId);
    *viewId = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);
    return NOERROR;
}

ECode CAccessibilityNodeInfoHelper::GetVirtualDescendantId(
    /* [in] */ Int64 accessibilityNodeId,
    /* [out] */ Int32* descendantId)
{
    VALIDATE_NOT_NULL(descendantId);
    *descendantId = CAccessibilityNodeInfo::GetVirtualDescendantId(accessibilityNodeId);
    return NOERROR;
}

ECode CAccessibilityNodeInfoHelper::MakeNodeId(
    /* [in] */ Int32 accessibilityViewId,
    /* [in] */ Int32 virtualDescendantId,
    /* [out] */ Int64* nodeId)
{
    VALIDATE_NOT_NULL(nodeId);
    *nodeId = CAccessibilityNodeInfo::MakeNodeId(accessibilityViewId, virtualDescendantId);
    return NOERROR;
}

ECode CAccessibilityNodeInfoHelper::Obtain(
    /* [in] */ IView* source,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return CAccessibilityNodeInfo::Obtain(source, info);
}

ECode CAccessibilityNodeInfoHelper::Obtain(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return CAccessibilityNodeInfo::Obtain(root, virtualDescendantId, info);
}

ECode CAccessibilityNodeInfoHelper::Obtain(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return CAccessibilityNodeInfo::Obtain(info);
}

ECode CAccessibilityNodeInfoHelper::Obtain(
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [out] */ IAccessibilityNodeInfo** infoClone)
{
    VALIDATE_NOT_NULL(infoClone);
    return CAccessibilityNodeInfo::Obtain(info, infoClone);
}

} // Accessibility
} // View
} // Droid
} // Elastos
