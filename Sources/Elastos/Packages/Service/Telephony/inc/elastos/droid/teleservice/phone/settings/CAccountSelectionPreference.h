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

#ifndef  __ELASTOS_DROID_PHONE_SETTINGS_CACCOUNTSELECTIONPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_SETTINGS_CACCOUNTSELECTIONPREFERENCE_H__

#include "_Elastos_Droid_TeleService_Phone_Settings_CAccountSelectionPreference.h"
#include "elastos/droid/preference/ListPreference.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Preference.h>
#include <Elastos.Droid.Telecom.h>
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::ListPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {
namespace Settings {

CarClass(CAccountSelectionPreference)
    , public ListPreference
    , public IAccountSelectionPreference
{
private:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CAccountSelectionPreference* host);

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* res);
    private:
        CAccountSelectionPreference* mHost;
    };
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL()

    CAccountSelectionPreference();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetListener(
        /* [in] */ IAccountSelectionListener* listener);

    CARAPI SetShowSelectionInSummary(
        /* [in] */ Boolean value);

    CARAPI SetModel(
        /* [in] */ ITelecomManager* telecomManager,
        /* [in] */ IList* accountsList,
        /* [in] */ IPhoneAccountHandle* currentSelection,
        /* [in] */ ICharSequence* nullSelectionString);

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* res);

protected:
    /**
     * Modifies the dialog to change the default "Cancel" button to "Choose Accounts", which
     * triggers the {@link PhoneAccountSelectionPreferenceActivity} to be shown.
     *
     * @param builder The {@code AlertDialog.Builder}.
     */
    //@Override
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IAccountSelectionListener> mListener;
    AutoPtr<ArrayOf<IInterface*> > mAccounts;
    AutoPtr<ArrayOf<ICharSequence*> > mEntryValues;
    AutoPtr<ArrayOf<ICharSequence*> > mEntries;
    Boolean mShowSelectionInSummary;
};

} // namespace Settings
} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_SETTINGS_CACCOUNTSELECTIONPREFERENCE_H__