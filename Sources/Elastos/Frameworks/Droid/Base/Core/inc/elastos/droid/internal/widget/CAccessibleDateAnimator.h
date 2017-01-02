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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CACCESSIBLEDATEANIMATOR_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CACCESSIBLEDATEANIMATOR_H__

#include "_Elastos_Droid_Internal_Widget_CAccessibleDateAnimator.h"
#include "elastos/droid/widget/ViewAnimator.h"

using Elastos::Droid::Widget::ViewAnimator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * @hide
 */
CarClass(CAccessibleDateAnimator)
    , public ViewAnimator
    , public IAccessibleDateAnimator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CAccessibleDateAnimator();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDateMillis(
        /* [in] */ Int64 dateMillis);

    /**
     * Announce the currently-selected date when launched.
     */
    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

private:
    Int64 mDateMillis;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CACCESSIBLEDATEANIMATOR_H__
