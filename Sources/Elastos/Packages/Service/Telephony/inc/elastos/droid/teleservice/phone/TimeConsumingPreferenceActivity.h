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

#ifndef  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__
#define  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/preference/PreferenceActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Internal::Telephony::ICommandException;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Utility class to look up carrier logo resource IDs.
 */
class TimeConsumingPreferenceActivity
    : public PreferenceActivity
    , public ITimeConsumingPreferenceListener
    , public IDialogInterfaceOnCancelListener
{
private:
    class DismissOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("TimeConsumingPreferenceActivity::DismissOnClickListener")

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    };

    class DismissAndFinishOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("TimeConsumingPreferenceActivity::DismissAndFinishOnClickListener")

        CAR_INTERFACE_DECL()

        DismissAndFinishOnClickListener(
            TimeConsumingPreferenceActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        TimeConsumingPreferenceActivity* mHost;
    };

public:
    TO_STRING_IMPL("TimeConsumingPreferenceActivity")

    CAR_INTERFACE_DECL()

    TimeConsumingPreferenceActivity();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnStarted(
        /* [in] */ IPreference* preference,
        /* [in] */ Boolean reading);

    //@Override
    CARAPI OnFinished(
        /* [in] */ IPreference* preference,
        /* [in] */ Boolean reading);

    //@Override
    CARAPI OnError(
        /* [in] */ IPreference* preference,
        /* [in] */ Int32 error);

    //@Override
    CARAPI OnException(
        /* [in] */ IPreference* preference,
        /* [in] */ ICommandException* exception);

    //@Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI DumpState();

protected:
    //@Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

private:
    CARAPI_(void) DismissDialogSafely(
        /* [in] */ Int32 id);

protected:
    Boolean mIsForeground;

private:
    static const String TAG;
    const Boolean DBG;

    AutoPtr<IDialogInterfaceOnClickListener> mDismiss;
    AutoPtr<IDialogInterfaceOnClickListener> mDismissAndFinish;

    static const Int32 BUSY_READING_DIALOG;
    static const Int32 BUSY_SAVING_DIALOG;

    static const Int32 EXCEPTION_ERROR;
    static const Int32 RESPONSE_ERROR;
    static const Int32 RADIO_OFF_ERROR;
    static const Int32 FDN_CHECK_FAILURE;

    AutoPtr<IArrayList> mBusyList;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CARRIERLOGO_H__