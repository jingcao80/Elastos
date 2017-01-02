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

#include "elastos/droid/app/CInnerReceiver.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/app/LoadedPkg.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::App::LoadedPkg;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CInnerReceiver, Object, IIntentReceiver, IBinder)

CAR_OBJECT_IMPL(CInnerReceiver)

ECode CInnerReceiver::constructor(
    /* [in] */ IReceiverDispatcher* rd,
    /* [in] */ Boolean strong)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(rd);
    wrs->GetWeakReference((IWeakReference**)&mDispatcher);
    mStrongRefRef = strong ? rd : NULL;
    return NOERROR;
}

ECode CInnerReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    AutoPtr<IReceiverDispatcher> rd;
    mDispatcher->Resolve(EIID_IReceiverDispatcher, (IInterface**)&rd);
    AutoPtr<LoadedPkg::ReceiverDispatcher> dispatcher;
    if (rd != NULL) {
        dispatcher = (LoadedPkg::ReceiverDispatcher*)rd.Get();
    }

    if (CActivityThread::DEBUG_BROADCAST) {
        Int32 seq = -1;
        intent->GetInt32Extra(String("seq"), -1, &seq);
        String action;
        intent->GetAction(&action);
        Slogger::I("CInnerReceiver", "Receiving broadcast %s seq=%d to %p", action.string(), seq, dispatcher.Get());
    }
    if (dispatcher != NULL) {
        dispatcher->PerformReceive(intent, resultCode, data, extras, ordered, sticky, sendingUser);
        return NOERROR;
    }
    else {
        // The activity manager dispatched a broadcast to a registered
        // receiver in this process, but before it could be delivered the
        // receiver was unregistered.  Acknowledge the broadcast on its
        // behalf so that the system's broadcast sequence can continue.
        if (CActivityThread::DEBUG_BROADCAST) {
            Slogger::I("CInnerReceiver", "Finishing broadcast to unregistered receiver");
        }
        AutoPtr<IIActivityManager> mgr = ActivityManagerNative::GetDefault();
        //try {
        if (extras != NULL) {
            Boolean bval;
            extras->SetAllowFds(FALSE, &bval);
        }

        Int32 flags;
        intent->GetFlags(&flags);
        ECode ec = mgr->FinishReceiver(this, resultCode, data, extras, FALSE, flags);
        if (FAILED(ec)) {
            Slogger::W("CInnerReceiver", "Couldn't finish broadcast to unregistered receiver");
        }
        // } catch () {
        //
        // }
    }
    return NOERROR;
}

ECode CInnerReceiver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
