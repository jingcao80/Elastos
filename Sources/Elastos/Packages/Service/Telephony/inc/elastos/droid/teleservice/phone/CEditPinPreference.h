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

#ifndef  __ELASTOS_DROID_PHONE_CEDITPINPREFRENCE_H__
#define  __ELASTOS_DROID_PHONE_CEDITPINPREFRENCE_H__

#include "_Elastos_Droid_TeleService_Phone_CEditPinPreference.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/EditTextPreference.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Preference::EditTextPreference;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Class similar to the com.android.settings.EditPinPreference
 * class, with a couple of modifications, including a different layout
 * for the dialog.
 */
CarClass(CEditPinPreference)
    , public EditTextPreference
    , public IPhoneEditPinPreference
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEditPinPreference();

    CARAPI SetOnPinEnteredListener(
        /* [in] */ IEditPinPreferenceOnPinEnteredListener* listener);

    /**
     * Externally visible method to bring up the dialog to
     * for multi-step / multi-dialog requests (like changing
     * the SIM pin).
     */
    CARAPI ShowPinDialog();

protected:
    /**
     * Overridden to setup the correct dialog layout, as well as setting up
     * other properties for the pin / puk entry field.
     */
    //@Override
    CARAPI OnCreateDialogView(
        /* [out] */ IView** view);

    //@Override
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

private:
    Boolean mShouldHideButtons;

    AutoPtr<IEditPinPreferenceOnPinEnteredListener> mPinListener;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEDITPINPREFRENCE_H__