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

#ifndef __ELASTOS_DROID_INCALLUI_CDIALPADSLIDINGLINEARLAYOUT_H__
#define __ELASTOS_DROID_INCALLUI_CDIALPADSLIDINGLINEARLAYOUT_H__

#include "_Elastos_Droid_InCallUI_CDialpadSlidingLinearLayout.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * LinearLayout with getter and setter methods for the translationY property using floats,
 * for animation purposes.
 */
CarClass(CDialpadSlidingLinearLayout)
    , public LinearLayout
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI_(Float) GetYFraction();

    CARAPI_(void) SetYFraction(
        /* [in] */ Float yFraction);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CDIALPADSLIDINGLINEARLAYOUT_H__
