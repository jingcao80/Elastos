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

#ifndef __ELASTOS_DROID_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__
#define __ELASTOS_DROID_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__

#include "_Elastos_Droid_Dialer_Interactions_CUndemoteOutgoingCallReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Thread.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

/**
 * This broadcast receiver is used to listen to outgoing calls and undemote formerly demoted
 * contacts if a phone call is made to a phone number belonging to that contact.
 */
CarClass(CUndemoteOutgoingCallReceiver)
    , public BroadcastReceiver
    , public IUndemoteOutgoingCallReceiver
{
private:
    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ CUndemoteOutgoingCallReceiver* host,
            /* [in] */ IContext* context,
            /* [in] */ const String& number);

        // @Override
        CARAPI Run();

    private:
        CUndemoteOutgoingCallReceiver* mHost;
        AutoPtr<IContext> mContext;
        String mNumber;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) UndemoteContactWithId(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    CARAPI_(Int64) GetContactIdFromPhoneNumber(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

private:
    static const Int64 NO_CONTACT_FOUND; // = -1;
};

} // Interactions
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__
