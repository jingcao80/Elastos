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

#ifndef __ELASTOS_DROID_VIEW_CACCESSIBILITYINTERACTIONCONNECTION_H__
#define __ELASTOS_DROID_VIEW_CACCESSIBILITYINTERACTIONCONNECTION_H__

#include "_Elastos_Droid_View_CAccessibilityInteractionConnection.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnection;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnectionCallback;

namespace Elastos {
namespace Droid {
namespace View {


/**
 * This class is an interface this ViewAncestor provides to the
 * AccessibilityManagerService to the latter can interact with
 * the view hierarchy in this ViewAncestor.
 */
CarClass(CAccessibilityInteractionConnection)
    , public Object
    , public IIAccessibilityInteractionConnection
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IWeakReference* viewRootImpl);

    CARAPI FindAccessibilityNodeInfoByAccessibilityId(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ IRegion* bounds,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FindAccessibilityNodeInfosByViewId(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& viewId,
        /* [in] */ IRegion* bounds,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FindAccessibilityNodeInfosByText(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& text,
        /* [in] */ IRegion* bounds,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FindFocus(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 focusType,
        /* [in] */ IRegion* bounds,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FocusSearch(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 direction,
        /* [in] */ IRegion* bounds,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI PerformAccessibilityAction(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI ComputeClickPointInScreen(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ IRegion* bounds,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

private:
    AutoPtr<IWeakReference> mViewRootImpl;
};

} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_CACCESSIBILITYINTERACTIONCONNECTION_H__
