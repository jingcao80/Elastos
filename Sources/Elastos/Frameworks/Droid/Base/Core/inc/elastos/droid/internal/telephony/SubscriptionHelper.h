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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SubscriptionHelper
    : public Handler
    , public ISubscriptionHelper
{
private:
    class MyContentObserver
        : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in] */ SubscriptionHelper* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfUpdate);

    public:
        SubscriptionHelper* mHost;
    };

public:
    TO_STRING_IMPL("SubscriptionHelper")

    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<ISubscriptionHelper>) Init(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    static CARAPI_(AutoPtr<ISubscriptionHelper>) GetInstance();

    CARAPI constructor(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    //@Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI UpdateSubActivation(
        /* [in] */ ArrayOf<Int32>* simStatus,
        /* [in] */ Boolean isStackReadyEvent);

    CARAPI UpdateNwMode();

    CARAPI SetUiccSubscription(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 subStatus);

    CARAPI IsRadioOn(
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    CARAPI IsRadioAvailable(
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    CARAPI IsApmSIMNotPwdn(
        /* [out] */ Boolean* result);

    CARAPI ProceedToHandleIccEvent(
        /* [in] */ Int32 slotId,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) UpdateNwModesInSubIdTable(
        /* [in] */ Boolean override);

    /**
     * Handles the EVENT_SET_UICC_SUBSCRPTION_DONE.
     * @param ar
     */
    CARAPI_(void) ProcessSetUiccSubscriptionDone(
        /* [in] */ IMessage* msg);

    CARAPI_(void) BroadcastSetUiccResult(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 newSubState,
        /* [in] */ Int32 result);

    CARAPI_(Boolean) IsAllSubsAvailable();

    static CARAPI_(void) Logd(
        /* [in] */ String message);

    CARAPI_(void) Logi(
        /* [in] */ String msg);

    CARAPI_(void) Loge(
        /* [in] */ String msg);

public:
    static const String LOGTAG;
    static AutoPtr<ISubscriptionHelper> sInstance;

    AutoPtr<IContext> mContext;
    AutoPtr<ArrayOf<ICommandsInterface*> > mCi;
    AutoPtr<ArrayOf<Int32> > mSubStatus;
    static Int32 sNumPhones;
    // This flag is used to trigger Dds during boot-up
    // and when flex mapping performed
    static Boolean sTriggerDds;

    static const String APM_SIM_NOT_PWDN_PROPERTY;
    static const Boolean sApmSIMNotPwdn;

    static const Int32 EVENT_SET_UICC_SUBSCRIPTION_DONE;
    static const Int32 EVENT_REFRESH;
    static const Int32 EVENT_REFRESH_OEM;

    static Boolean mNwModeUpdated;

    AutoPtr<MyContentObserver> mNwModeObserver;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONHELPER_H__
