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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_TTYMANAGER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_TTYMANAGER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

class TtyManager
    : public Object
    , public ITtyManager
{
public:
    class TtyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TtyManager::TtyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ ITtyManager* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TtyManager* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TtyManager::MyHandler")

        MyHandler(
            /* [in] */ TtyManager* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TtyManager* mHost;
    };

public:
    TO_STRING_IMPL("TtyManager")

    CAR_INTERFACE_DECL()

    TtyManager(
        /* [in] */ IContext* context,
        /* [in] */ IPhone* phone);

private:
    CARAPI_(void) UpdateTtyMode(
        /* [in] */ Int32 ttyMode);

    static CARAPI_(Int32) TelecomModeToPhoneMode(
        /* [in] */ Int32 telecomMode);

    static CARAPI_(Int32) PhoneModeToTelecomMode(
        /* [in] */ Int32 phoneMode);

private:
    static const Int32 MSG_SET_TTY_MODE_RESPONSE = 1;
    static const Int32 MSG_GET_TTY_MODE_RESPONSE = 2;

    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IPhone> mPhone;
    Int32 mTtyMode;

    AutoPtr<IHandler> mHandler;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TTYMANAGER_H__