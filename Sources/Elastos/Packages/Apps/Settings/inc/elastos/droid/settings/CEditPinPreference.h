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

#ifndef __ELASTOS_DROID_SETTINGS_CEDITPINPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_CEDITPINPREFERENCE_H__

#include "_Elastos_Droid_Settings_CEditPinPreference.h"
#include "elastos/droid/preference/EditTextPreference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::EditTextPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * TODO: Add a soft dialpad for PIN entry.
 */
CarClass(CEditPinPreference)
    , public EditTextPreference
    , public IEditPinPreference
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CEditPinPreference();

    virtual ~CEditPinPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    virtual CARAPI SetOnPinEnteredListener(
        /* [in] */ IEditPinPreferenceOnPinEnteredListener* listener);

    virtual CARAPI_(Boolean) IsDialogOpen();

    virtual CARAPI ShowPinDialog();

protected:
    //@Override
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

private:
    AutoPtr<IEditPinPreferenceOnPinEnteredListener> mPinListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CEDITPINPREFERENCE_H__