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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOACCESSIBILITYACTIONHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOACCESSIBILITYACTIONHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfoAccessibilityActionHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfoAccessibilityActionHelper)
    , public Singleton
    , public IAccessibilityNodeInfoAccessibilityActionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetACTION_FOCUS(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_CLEAR_FOCUS(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_SELECT(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_CLEAR_SELECTION(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_CLICK(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_LONG_CLICK(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_ACCESSIBILITY_FOCUS(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_CLEAR_ACCESSIBILITY_FOCUS(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_NEXT_AT_MOVEMENT_GRANULARITY(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_NEXT_HTML_ELEMENT(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_PREVIOUS_HTML_ELEMENT(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_SCROLL_FORWARD(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_SCROLL_BACKWARD(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_COPY(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_PASTE(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_CUT(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_SET_SELECTION(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_EXPAND(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_COLLAPSE(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_DISMISS(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);

    CARAPI GetACTION_SET_TEXT(
        /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOACCESSIBILITYACTIONHELPER_H__
