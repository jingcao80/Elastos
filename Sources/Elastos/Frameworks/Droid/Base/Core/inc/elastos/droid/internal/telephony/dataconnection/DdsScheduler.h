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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DDSSCHEDULER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DDSSCHEDULER_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/internal/utility/StateMachine.h>

using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/* {@hide} */
class DdsScheduler
    : public StateMachine
    , public IDdsScheduler
{
private:
    class NetworkRequestInfo
        : public Object
    {
        friend class DdsScheduler;
    public:
        NetworkRequestInfo(
            /* [in] */ DdsScheduler* host);

        CARAPI constructor(
            /* [in] */ INetworkRequest* req);

        CARAPI ToString(
            /* [out] */ String* result);

        CARAPI GetMRequest(
            /* [out] */ INetworkRequest** result);

        CARAPI GetMAccepted(
            /* [out] */ Boolean* result);

        CARAPI SetMAccepted(
            /* [in] */ Boolean mAccepted);

    private:
        AutoPtr<INetworkRequest> mRequest;

        Boolean mAccepted;

        DdsScheduler* mHost;
    };

    class DefaultState
        : public State
    {
    public:
        DefaultState(
            /* [in] */ DdsScheduler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    public:
        static const String TAG;

    private:
        DdsScheduler* mHost;
    };

    /*
     * DdsIdleState: System is idle, none of the subscription is reserved.
     *
     * If a new request arrives do following actions.
     *  1. If the request is for currentDds, move to DdsReservedState.
     *  2. If the request is for other sub move to DdsSwitchState.
     */
    class DdsIdleState
        : public State
    {
    public:
        DdsIdleState(
            /* [in] */ DdsScheduler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    public:
        static const String TAG;

    private:
        DdsScheduler* mHost;
    };

    class DdsReservedState
        : public State
    {
    public:
        DdsReservedState(
            /* [in] */ DdsScheduler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        CARAPI HandleOtherSubRequests();

    public:
        static const String TAG;

    private:
        DdsScheduler* mHost;
    };

    class PsAttachReservedState
        : public State
    {
    public:
        PsAttachReservedState(
            /* [in] */ DdsScheduler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        CARAPI HandleOtherSubRequests();

    public:
        static const String TAG;

    private:
        DdsScheduler* mHost;
    };

    class DdsSwitchState
        : public State
    {
    public:
        DdsSwitchState(
            /* [in] */ DdsScheduler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    public:
        static const String TAG;

    private:
        DdsScheduler* mHost;
    };

    class DdsAutoRevertState
        : public State
    {
    public:
        DdsAutoRevertState(
            /* [in] */ DdsScheduler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    public:
        static const String TAG;

    private:
        DdsScheduler* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    // [@private]
    DdsScheduler();

    static CARAPI GetInstance(
        /* [out] */ IDdsScheduler** result);

    static CARAPI Init();

    CARAPI AddRequest(
        /* [in] */ INetworkRequest* req);

    CARAPI RemoveRequest(
        /* [in] */ INetworkRequest* req);

    CARAPI MarkAccepted(
        /* [in] */ INetworkRequest* req);

    CARAPI IsAlreadyAccepted(
        /* [in] */ INetworkRequest* nr,
        /* [out] */ Boolean* result);

    CARAPI GetFirstWaitingRequest(
        /* [out] */ INetworkRequest** result);

    CARAPI AcceptWaitingRequest(
        /* [out] */ Boolean* result);

    CARAPI NotifyRequestAccepted(
        /* [in] */ INetworkRequest* nr);

    CARAPI IsDdsSwitchRequired(
        /* [in] */ INetworkRequest* n,
        /* [out] */ Boolean* result);

    CARAPI GetCurrentDds(
        /* [out] */ Int64* result);

    CARAPI UpdateCurrentDds(
        /* [in] */ INetworkRequest* n);

    CARAPI GetSubIdFromNetworkRequest(
        /* [in] */ INetworkRequest* n,
        /* [out] */ Int64* result);

    CARAPI TriggerSwitch(
        /* [in] */ INetworkRequest* n);

    CARAPI IsMultiDataSupported(
        /* [out] */ Boolean* result);

    CARAPI IsAnyRequestWaiting(
        /* [out] */ Boolean* result);

private:
    CARAPI constructor();

    static CARAPI CreateDdsScheduler(
        /* [out] */ IDdsScheduler** result);

    CARAPI RequestDdsSwitch(
        /* [in] */ INetworkRequest* n);

    CARAPI RequestPsAttach(
        /* [in] */ INetworkRequest* n);

    CARAPI RequestPsDetach();

    CARAPI GetMaxDataAllowed(
        /* [out] */ Int32* result);

    CARAPI RegisterCallbacks();

public:
    static const String TAG;

private:
    AutoPtr<DefaultState> mDefaultState;

    AutoPtr<DdsIdleState> mDdsIdleState;

    AutoPtr<DdsReservedState> mDdsReservedState;

    AutoPtr<PsAttachReservedState> mPsAttachReservedState;

    AutoPtr<DdsSwitchState> mDdsSwitchState;

    AutoPtr<DdsAutoRevertState> mDdsAutoRevertState;

    AutoPtr<IList> mInbox;
    static Object sInboxLock;

    Int64 mCurrentDds;

    AutoPtr<IDctController> mDctController;

    static AutoPtr<IDdsScheduler> sDdsScheduler;

    const Int32 MODEM_DATA_CAPABILITY_UNKNOWN;

    const Int32 MODEM_SINGLE_DATA_CAPABLE;

    const Int32 MODEM_DUAL_DATA_CAPABLE;

    const String OVERRIDE_MODEM_DUAL_DATA_CAP_PROP;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DDSSCHEDULER_H__
