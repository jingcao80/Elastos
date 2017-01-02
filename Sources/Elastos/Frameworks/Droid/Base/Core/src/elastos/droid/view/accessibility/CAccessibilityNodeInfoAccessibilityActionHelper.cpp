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
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoAccessibilityActionHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoAccessibilityAction.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityNodeInfoAccessibilityActionHelper, Singleton, IAccessibilityNodeInfoAccessibilityActionHelper)

CAR_SINGLETON_IMPL(CAccessibilityNodeInfoAccessibilityActionHelper)

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_FOCUS(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_FOCUS;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_CLEAR_FOCUS(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_FOCUS;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_SELECT(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_SELECT;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_CLEAR_SELECTION(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_SELECTION;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_CLICK(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_CLICK;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_LONG_CLICK(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_LONG_CLICK;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_ACCESSIBILITY_FOCUS(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_ACCESSIBILITY_FOCUS;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_CLEAR_ACCESSIBILITY_FOCUS(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_NEXT_AT_MOVEMENT_GRANULARITY(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_NEXT_AT_MOVEMENT_GRANULARITY;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_NEXT_HTML_ELEMENT(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_NEXT_HTML_ELEMENT;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_PREVIOUS_HTML_ELEMENT(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_PREVIOUS_HTML_ELEMENT;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_SCROLL_FORWARD(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_SCROLL_FORWARD;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_SCROLL_BACKWARD(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_SCROLL_BACKWARD;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_COPY(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_COPY;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_PASTE(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_PASTE;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_CUT(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_CUT;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_SET_SELECTION(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_SET_SELECTION;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_EXPAND(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_EXPAND;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_COLLAPSE(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_COLLAPSE;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_DISMISS(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_DISMISS;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

ECode CAccessibilityNodeInfoAccessibilityActionHelper::GetACTION_SET_TEXT(
    /* [out] */ IAccessibilityNodeInfoAccessibilityAction** action)
{
    VALIDATE_NOT_NULL(action);
    *action = CAccessibilityNodeInfoAccessibilityAction::ACTION_SET_TEXT;
    REFCOUNT_ADD(*action);
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
