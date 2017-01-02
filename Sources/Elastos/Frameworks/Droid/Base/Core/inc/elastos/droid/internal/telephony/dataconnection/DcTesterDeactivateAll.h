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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERDEACTIVATEALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERDEACTIVATEALL_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * To bring down all DC's send the following intent:
 *
 * adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_deactivate_all
 */
class DcTesterDeactivateAll
    : public Object
    , public IDcTesterDeactivateAll
{
private:
    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DcTesterDeactivateAll::SubBroadcastReceiver")

        SubBroadcastReceiver(
            /* [in] */ DcTesterDeactivateAll* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcTesterDeactivateAll* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DcTesterDeactivateAll();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IDcController* dcc,
        /* [in] */ IHandler* handler);

    CARAPI Dispose();

public:
    static String sActionDcTesterDeactivateAll;

    // Below code can't be identified.
    // The static intent receiver one for all instances and we assume this
    // is running on the same thread as Dcc.
    AutoPtr<IBroadcastReceiver> sIntentReceiver;

private:
    static CARAPI Log(
        /* [in] */ const char *fmt, ...);

    static const String LOG__TAG;

    static const Boolean DBG;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<IDcController> mDcc;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERDEACTIVATEALL_H__
