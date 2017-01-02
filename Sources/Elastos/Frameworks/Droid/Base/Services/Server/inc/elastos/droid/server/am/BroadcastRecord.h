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

#ifndef __ELASTOS_DROID_SERVER_AM_BROADCASTRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_BROADCASTRECORD_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/am/BroadcastFilter.h"
#include "elastos/droid/server/am/ProcessRecord.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class BroadcastQueue;

/**
 * An active intent broadcast.
 */
class BroadcastRecord : public Object
{
public:
    BroadcastRecord(
        /* [in] */ BroadcastQueue* queue,
        /* [in] */ IIntent* intent,
        /* [in] */ ProcessRecord* callerApp,
        /* [in] */ const String& callerCapsule,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& resolvedType,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Int32 appOp,
        /* [in] */ IList* receivers,
        /* [in] */ IIntentReceiver* resultTo,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras,
        /* [in] */ Boolean serialized,
        /* [in] */ Boolean sticky,
        /* [in] */ Boolean initialSticky,
        /* [in] */ Int32 userId);

    ~BroadcastRecord();

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI_(String) ToString();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    static const Int32 IDLE = 0;
    static const Int32 APP_RECEIVE = 1;
    static const Int32 CALL_IN_RECEIVE = 2;
    static const Int32 CALL_DONE_RECEIVE = 3;
    static const Int32 WAITING_SERVICES = 4;

    const AutoPtr<IIntent> mIntent;       // the original intent that generated us
    AutoPtr<IComponentName> mTargetComp; // original component name set on the intent
    const AutoPtr<ProcessRecord> mCallerApp;      // process that sent this
    const String mCallerPackage;          // who sent this
    const Int32 mCallingPid;              // the pid of who sent this
    const Int32 mCallingUid;              // the uid of who sent this
    const Boolean mOrdered;               // serialize the send to receivers?
    const Boolean mSticky;                // originated from existing sticky data?
    const Boolean mInitialSticky;         // initial broadcast from register to sticky?
    const Int32 mUserId;       // user id this broadcast was for
    const String mResolvedType; // the resolved data type
    const String mRequiredPermission;     // a permission the caller has required
    const Int32 mAppOp;        // an app op that is associated with this broadcast
    AutoPtr<IList> mReceivers;  // contains BroadcastFilter and ResolveInfo
    AutoPtr<IIntentReceiver> mResultTo; // who receives final result if non-null
    Int64 mDispatchTime;      // when dispatch started on this set of receivers
    Int64 mDispatchClockTime; // the clock time the dispatch started
    Int64 mReceiverTime;      // when current receiver started for timeouts.
    Int64 mFinishTime;        // when we finished the broadcast.
    Int32 mResultCode;         // current result code value.
    String mResultData;      // current result data value.
    AutoPtr<IBundle> mResultExtras;    // current result extra data values.
    Boolean mResultAbort;    // current result abortBroadcast value.
    Int32 mNextReceiver; // next receiver to be executed.
    AutoPtr<IBinder> mReceiver;// who is currently running, null if none.
    Int32 mState;
    Int32 mAnrCount;           // has this broadcast record hit any ANRs?
    BroadcastQueue* mQueue;   // the outbound queue handling this broadcast

    // The following are set when we are calling a receiver (one that
    // was found in our list of registered receivers).
    AutoPtr<BroadcastFilter> mCurFilter;

    AutoPtr<ProcessRecord> mCurApp;         // hosting application of current receiver.
    AutoPtr<IComponentName> mCurComponent;   // the receiver class that is currently running.
    AutoPtr<IActivityInfo> mCurReceiver;   // info about the receiver that is currently running.
};


} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Server::Am::BroadcastRecord*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SERVER_AM_BROADCASTRECORD_H__
