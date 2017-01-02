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

#ifndef __ELASTOS_DROID_PREFERENCE_SWITCHPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_SWITCHPREFERENCE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/preference/TwoStatePreference.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace Preference {

class ECO_PUBLIC SwitchPreference
    : public TwoStatePreference
    , public ISwitchPreference
{
public:
    class Listener
        : public Object
        , public ICompoundButtonOnCheckedChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        Listener(
            /* [in] */ SwitchPreference* host);

        ECode OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

     private:
        SwitchPreference* mHost;
     };

public:
    CAR_INTERFACE_DECL()

    SwitchPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetSwitchTextOn(
        /* [in] */ ICharSequence* onText);

    CARAPI SetSwitchTextOff(
        /* [in] */ ICharSequence* offText);

    CARAPI SetSwitchTextOn(
        /* [in] */ Int32 resId);

    CARAPI SetSwitchTextOff(
        /* [in] */ Int32 resId);

    CARAPI GetSwitchTextOn(
        /* [out] */ ICharSequence** textOn);

    CARAPI GetSwitchTextOff(
        /* [out] */ ICharSequence** textOff);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    // Switch text for on and off states
    AutoPtr<ICharSequence> mSwitchOn;
    AutoPtr<ICharSequence> mSwitchOff;
    AutoPtr<ICompoundButtonOnCheckedChangeListener> mListener;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_SWITCHPREFERENCE_H__
