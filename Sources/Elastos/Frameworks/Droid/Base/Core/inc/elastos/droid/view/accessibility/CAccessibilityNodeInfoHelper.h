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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfoHelper)
    , public Singleton
    , public IAccessibilityNodeInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAccessibilityViewId(
        /* [in] */ Int64 accessibilityNodeId,
        /* [out] */ Int32* viewId);

    CARAPI GetVirtualDescendantId(
        /* [in] */ Int64 accessibilityNodeId,
        /* [out] */ Int32* descendantId);

    CARAPI MakeNodeId(
        /* [in] */ Int32 accessibilityViewId,
        /* [in] */ Int32 virtualDescendantId,
        /* [out] */ Int64* nodeId);

    CARAPI Obtain(
        /* [in] */ IView* source,
        /* [out] */ IAccessibilityNodeInfo** info);

    CARAPI Obtain(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId,
        /* [out] */ IAccessibilityNodeInfo** info);

    CARAPI Obtain(
        /* [out] */ IAccessibilityNodeInfo** info);

    CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [out] */ IAccessibilityNodeInfo** infoClone);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__CAccessibilityNodeInfoHelper_H__
