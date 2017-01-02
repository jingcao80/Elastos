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

#ifndef  __ELASTOS_DROID_PHONE_CMMIDIALOGACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CMMIDIALOGACTIVITY_H__

#include "_Elastos_Droid_TeleService_Phone_CMMIDialogActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::IMmiCode;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Used to display a dialog from within the Telephony service when running an USSD code
 */
CarClass(CMMIDialogActivity)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CMMIDialogActivity::MyHandler")

        MyHandler(
            /* [in] */ CMMIDialogActivity* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CMMIDialogActivity* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CMMIDialogActivity();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ShowMMIDialog();

    /**
     * Handles an MMI_COMPLETE event, which is triggered by telephony
     */
    CARAPI_(void) OnMMIComplete(
        /* [in] */ IMmiCode* mmiCode);

    /**
     * Handles an MMI_CANCEL event, which is triggered by the button
     * (labeled either "OK" or "Cancel") on the "MMI Started" dialog.
     * @see PhoneUtils#cancelMmiCode(Phone)
     */
    CARAPI_(void) OnMMICancel();

    CARAPI_(void) DismissDialogsAndFinish();

private:
    static const String TAG;

    AutoPtr<IDialog> mMMIDialog;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ICallManager> mCM;
    AutoPtr<IPhone> mPhone;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CMMIDIALOGACTIVITY_H__