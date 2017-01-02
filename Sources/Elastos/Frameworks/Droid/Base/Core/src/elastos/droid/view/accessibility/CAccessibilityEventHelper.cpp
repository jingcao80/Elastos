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
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityEventHelper, Singleton, IAccessibilityEventHelper)

CAR_SINGLETON_IMPL(CAccessibilityEventHelper)

ECode CAccessibilityEventHelper::Obtain(
    /* [in] */ Int32 eventType,
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return CAccessibilityEvent::Obtain(eventType, event);
}

ECode CAccessibilityEventHelper::Obtain(
    /* [in] */ IAccessibilityEvent* otherEvent,
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return CAccessibilityEvent::Obtain(otherEvent, event);
}

ECode CAccessibilityEventHelper::Obtain(
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return CAccessibilityEvent::Obtain(event);
}

ECode CAccessibilityEventHelper::EventTypeToString(
    /* [in] */ Int32 eventType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = CAccessibilityEvent::EventTypeToString(eventType);
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
