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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENTHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENTHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityEventHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityEventHelper)
    , public Singleton
    , public IAccessibilityEventHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated with its type property set.
     *
     * @param eventType The event type.
     * @return An instance.
     */
    CARAPI Obtain(
        /* [in] */ Int32 eventType,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns a cached instance if such is available or a new one is
     * created. The returned instance is initialized from the given
     * <code>event</code>.
     *
     * @param event The other event.
     * @return An instance.
     */
    CARAPI Obtain(
        /* [in] */ IAccessibilityEvent* otherEvent,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated.
     *
     * @return An instance.
     */
    CARAPI Obtain(
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns the string representation of an event type. For example,
     * {@link #TYPE_VIEW_CLICKED} is represented by the string TYPE_VIEW_CLICKED.
     *
     * @param eventType The event type
     * @return The string representation.
     */
    CARAPI EventTypeToString(
        /* [in] */ Int32 eventType,
        /* [out] */ String* str);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENTHELPER_H__
