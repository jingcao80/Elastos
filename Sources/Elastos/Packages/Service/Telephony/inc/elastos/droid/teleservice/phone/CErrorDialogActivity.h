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

#ifndef  __ELASTOS_DROID_PHONE_CERRORDIALOGACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CERRORDIALOGACTIVITY_H__

#include "_Elastos_Droid_TeleService_Phone_CErrorDialogActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Used to display an error dialog from within the Telephony service when an outgoing call fails
 */
CarClass(CErrorDialogActivity)
    , public Activity
    , public IErrorDialogActivity
{
private:
    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CErrorDialogActivity* mHost;

    };

    class MyDialogInterfaceOnClickListener2
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnClickListener2")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener2(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CErrorDialogActivity* mHost;

    };

    class MyDialogInterfaceOnClickListener3
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnClickListener3")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener3(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CErrorDialogActivity* mHost;

    };

    class MyDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnCancelListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnCancelListener(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CErrorDialogActivity* mHost;
    };

    class MyDialogInterfaceOnCancelListener2
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnCancelListener2")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnCancelListener2(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CErrorDialogActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CErrorDialogActivity() {}

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ShowGenericErrorDialog(
        /* [in] */ Int32 resid);

    CARAPI_(void) ShowMissingVoicemailErrorDialog();

    CARAPI_(void) AddVoiceMailNumberPanel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI_(void) DontAddVoiceMailNumber();

private:
    static const String TAG;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CERRORDIALOGACTIVITY_H__