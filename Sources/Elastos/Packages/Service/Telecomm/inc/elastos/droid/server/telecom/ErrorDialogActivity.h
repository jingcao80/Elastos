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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_ERRORDIALOGACTIVITY_H__
#define __ELASTOS_DROID_SERVER_TELECOM_ERRORDIALOGACTIVITY_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Used to display an error dialog from within the Telecom service when an outgoing call fails
 */
class ErrorDialogActivity
    : public Activity
    , public IErrorDialogActivity
{
private:
    class ClickSubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClickSubDialogInterfaceOnClickListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ErrorDialogActivity* mHost;
    };

    class CancelSubDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelSubDialogInterfaceOnCancelListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        ErrorDialogActivity* mHost;
    };

    class PositiveSubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PositiveSubDialogInterfaceOnClickListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ErrorDialogActivity* mHost;
    };

    class NegativeSubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NegativeSubDialogInterfaceOnClickListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ErrorDialogActivity* mHost;
    };

    class CancelShowSubDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelShowSubDialogInterfaceOnCancelListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        ErrorDialogActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI Finish();

private:
    CARAPI ShowGenericErrorDialog(
        /* [in] */ Int32 resid);

    CARAPI ShowMissingVoicemailErrorDialog();

    CARAPI AddVoiceMailNumberPanel(
        /* [in] */ IDialogInterface* dialog);

private:
    static const String TAG;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_ERRORDIALOGACTIVITY_H__
