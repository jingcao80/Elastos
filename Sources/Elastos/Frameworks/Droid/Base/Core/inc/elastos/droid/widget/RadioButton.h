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

#ifndef __ELASTOS_DROID_WIDGET_RADIOBUTTON_H__
#define __ELASTOS_DROID_WIDGET_RADIOBUTTON_H__

#include "elastos/droid/widget/CompoundButton.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class RadioButton
    : public CompoundButton
    , public IRadioButton
{
public:
    CAR_INTERFACE_DECL()

    RadioButton();

    ~RadioButton();

    CARAPI constructor(
        /* [in] */ IContext * context);

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IAttributeSet * attrs);

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IAttributeSet * attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * {@inheritDoc}
     * <p>
     * If the radio button is already checked, this method will not toggle the radio button.
     */
    // @Override
    virtual CARAPI Toggle();

    //@Override
    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);
};


} // namespace Widget
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_RADIOBUTTON_H__
