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

#include "elastos/droid/internal/telephony/dataconnection/DcController.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTesterDeactivateAll.h"
#include "elastos/droid/internal/telephony/dataconnection/DataCallResponse.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/DcTrackerBase.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/telephony/CDataConnectionRealTimeInfo.h"
#include <Elastos.CoreLibrary.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::ILinkPropertiesCompareResult;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Telephony::CDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Net::IInetAddress;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcController::DccDefaultState
//=============================================================================
DcController::DccDefaultState::DccDefaultState(
    /* [in] */ DcController* host)
    : mHost(host)
{
}

ECode DcController::DccDefaultState::Enter()
{
    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    ((PhoneBase*) mHost->mPhone.Get())->mCi->RegisterForRilConnected(handler,
            DataConnection::EVENT_RIL_CONNECTED, NULL);
    ((PhoneBase*) mHost->mPhone.Get())->mCi->RegisterForDataNetworkStateChanged(handler,
            DataConnection::EVENT_DATA_STATE_CHANGED, NULL);
    if (Build::IS_DEBUGGABLE) {
        mHost->mDcTesterDeactivateAll = NULL;
         CDcTesterDeactivateAll::New(mHost->mPhone, IDcController::Probe(TO_IINTERFACE(mHost)), handler, (IDcTesterDeactivateAll**)&(mHost->mDcTesterDeactivateAll));
    }
    return NOERROR;
}

ECode DcController::DccDefaultState::Exit()
{
    if (mHost->mPhone != NULL) {
        AutoPtr<IHandler> handler;
        mHost->GetHandler((IHandler**)&handler);
        ((PhoneBase*) mHost->mPhone.Get())->mCi->UnregisterForRilConnected(handler);
        ((PhoneBase*) mHost->mPhone.Get())->mCi->UnregisterForDataNetworkStateChanged(handler);
    }
    if (mHost->mDcTesterDeactivateAll != NULL) {
        ((DcTesterDeactivateAll*) mHost->mDcTesterDeactivateAll.Get())->Dispose();
    }
    return NOERROR;
}

ECode DcController::DccDefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    AutoPtr<IAsyncResult> ar;
    if (msgWhat == DataConnection::EVENT_RIL_CONNECTED) {
        ar = IAsyncResult::Probe(msgObj);
        if (((AsyncResult*) ar.Get())->mException == NULL) {
            if (DBG) {
                mHost->Log("DccDefaultState: msg.what=EVENT_RIL_CONNECTED mRilVersion=%s",
                    TO_CSTR(((AsyncResult*) ar.Get())->mResult));
            }
        } else {
            mHost->Log("DccDefaultState: Unexpected exception on EVENT_RIL_CONNECTED");
        }
    }
    else if (msgWhat == DataConnection::EVENT_DATA_STATE_CHANGED) {
        ar = IAsyncResult::Probe(msgObj);
        if (((AsyncResult*) ar.Get())->mException == NULL) {
            OnDataStateChanged(IArrayList::Probe(((AsyncResult*) ar.Get())->mResult));
        } else {
            mHost->Log("DccDefaultState: EVENT_DATA_STATE_CHANGED:"
                    " exception; likely radio not available, ignore");
        }
    }
    *result = HANDLED;
    return NOERROR;
}

ECode DcController::DccDefaultState::OnDataStateChanged(
    /* [in] */ IArrayList* dcsList)
{
    if (DBG) {
        mHost->Lr("onDataStateChanged: dcsList=%s mDcListActiveByCid=%s",
                TO_CSTR(dcsList), TO_CSTR(mHost->mDcListActiveByCid));
    }
    if (VDBG) {
        mHost->Log("onDataStateChanged: mDcListAll=%s", TO_CSTR(mHost->mDcListAll));
    }
    // Create hashmap of cid to DataCallResponse
    AutoPtr<IHashMap> dataCallResponseListByCid;
    CHashMap::New((IHashMap**)&dataCallResponseListByCid);
    AutoPtr<IIterator> it;
    dcsList->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDataCallResponse> dcs = IDataCallResponse::Probe(obj);
        Int32 cid;
        dcs->GetCid(&cid);
        AutoPtr<IInteger32> i32;
        CInteger32::New(cid, (IInteger32**)&i32);
        dataCallResponseListByCid->Put(i32, dcs);
    }
    // Add a DC that is active but not in the
    // dcsList to the list of DC's to retry
    AutoPtr<IArrayList> dcsToRetry;
    CArrayList::New((IArrayList**)&dcsToRetry);
    AutoPtr<ICollection> values;
    mHost->mDcListActiveByCid->GetValues((ICollection**)&values);
    it = NULL;
    values->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDataConnection> dc = IDataConnection::Probe(obj);
        AutoPtr<IInteger32> i32;
        CInteger32::New(((DataConnection*)dc.Get())->mCid, (IInteger32**)&i32);
        obj = NULL;
        dataCallResponseListByCid->Get(i32, (IInterface**)&obj);
        if (obj == NULL) {
            if (DBG) mHost->Log("onDataStateChanged: add to retry dc=%s", TO_CSTR(dc));
            dcsToRetry->Add(dc);
        }
    }
    if (DBG) mHost->Log("onDataStateChanged: dcsToRetry=%s", TO_CSTR(dcsToRetry));
    // Find which connections have changed state and send a notification or cleanup
    // and any that are in active need to be retried.
    AutoPtr<IArrayList> apnsToCleanup;
    CArrayList::New((IArrayList**)&apnsToCleanup);
    Boolean isAnyDataCallDormant = FALSE;
    Boolean isAnyDataCallActive = FALSE;
    it = NULL;
    dcsList->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDataCallResponse> newState = IDataCallResponse::Probe(obj);
        obj = NULL;
        Int32 cid;
        newState->GetCid(&cid);
        AutoPtr<IInteger32> i32;
        CInteger32::New(cid, (IInteger32**)&i32);
        mHost->mDcListActiveByCid->Get(i32, (IInterface**)&obj);
        AutoPtr<IDataConnection> dc = IDataConnection::Probe(obj);
        if (dc == NULL) {
            // UNSOL_DATA_CALL_LIST_CHANGED arrived before SETUP_DATA_CALL completed.
            mHost->Loge("onDataStateChanged: no associated DC yet, ignore");
            continue;
        }
        Int32 size;
        ((DataConnection*) dc.Get())->mApnContexts->GetSize(&size);
        if (size == 0) {
            if (DBG) mHost->Loge("onDataStateChanged: no connected apns, ignore");
        } else {
            // Determine if the connection/apnContext should be cleaned up
            // or just a notification should be sent out.
            Int32 cid;
            newState->GetCid(&cid);
            if (DBG) mHost->Log("onDataStateChanged: Found ConnId=%d newState=%s",
                    cid, TO_CSTR(newState));
            Int32 active;
            newState->GetActive(&active);
            if (active == DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE) {
                if (DcTrackerBase::mIsCleanupRequired) {
                    apnsToCleanup->AddAll(ICollection::Probe(((DataConnection*) dc.Get())->mApnContexts));
                    DcTrackerBase::mIsCleanupRequired = FALSE;
                } else {
                    Int32 status;
                    newState->GetStatus(&status);
                    AutoPtr<IDcFailCause> failCause;
                    DcFailCause::FromInt32(status, (IDcFailCause**)&failCause);
                    if (DBG) mHost->Log("onDataStateChanged: inactive failCause=%s", TO_CSTR(failCause));
                    Boolean isRestartRadioFail;
                    failCause->IsRestartRadioFail(&isRestartRadioFail);
                    Boolean isPermanentFail;
                    mHost->mDct->IsPermanentFail(failCause, &isPermanentFail);
                    if (isRestartRadioFail) {
                        if (DBG) mHost->Log("onDataStateChanged: X restart radio");
                        ((DcTrackerBase*) mHost->mDct.Get())->SendRestartRadio();
                    } else if (isPermanentFail) {
                        if (DBG) mHost->Log("onDataStateChanged: inactive, add to cleanup list");
                        apnsToCleanup->AddAll(ICollection::Probe(((DataConnection*) dc.Get())->mApnContexts));
                    } else {
                        if (DBG) mHost->Log("onDataStateChanged: inactive, add to retry list");
                        dcsToRetry->Add(dc);
                    }
                }
            } else {
                // Its active so update the DataConnections link properties
                AutoPtr<DataConnection::UpdateLinkPropertyResult> result;
                ((DataConnection*) dc.Get())->UpdateLinkProperty(newState, (DataConnection::UpdateLinkPropertyResult**)&result);
                Boolean isEquals;
                IObject::Probe(result->mOldLp)->Equals(result->mNewLp, &isEquals);
                if (isEquals) {
                    if (DBG) mHost->Log("onDataStateChanged: no change");
                } else {
                    Boolean isIdenticalInterfaceName;
                    result->mOldLp->IsIdenticalInterfaceName(result->mNewLp, &isIdenticalInterfaceName);
                    if (isIdenticalInterfaceName) {
                        Boolean isIdenticalDnses;
                        result->mOldLp->IsIdenticalDnses(result->mNewLp, &isIdenticalDnses);
                        Boolean isIdenticalRoutes;
                        result->mOldLp->IsIdenticalRoutes(result->mNewLp, &isIdenticalRoutes);
                        Boolean isIdenticalHttpProxy;
                        result->mOldLp->IsIdenticalHttpProxy(result->mNewLp, &isIdenticalHttpProxy);
                        Boolean isIdenticalAddresses;
                        result->mOldLp->IsIdenticalAddresses(result->mNewLp, &isIdenticalAddresses);
                        if (! isIdenticalDnses ||
                                ! isIdenticalRoutes ||
                                ! isIdenticalHttpProxy ||
                                ! isIdenticalAddresses) {
                            // If the same address type was removed and
                            // added we need to cleanup
                            AutoPtr<ILinkPropertiesCompareResult> car;
                            result->mOldLp->CompareAddresses(result->mNewLp, (ILinkPropertiesCompareResult**)&car);
                            if (DBG) {
                                mHost->Log("onDataStateChanged: oldLp=%s newLp=%s car=%s",
                                        TO_CSTR(result->mOldLp),TO_CSTR(result->mNewLp), TO_CSTR(car));
                            }
                            Boolean needToClean = FALSE;
                            AutoPtr<IList> added;
                            car->GetAdded((IList**)&added);
                            AutoPtr<IList> removed;
                            car->GetRemoved((IList**)&removed);
                            AutoPtr<IIterator> it;
                            added->GetIterator((IIterator**)&it);
                            Boolean hasNext;
                            while (it->HasNext(&hasNext), hasNext) {
                                AutoPtr<IInterface> obj;
                                it->GetNext((IInterface**)&obj);
                                AutoPtr<ILinkAddress> added = ILinkAddress::Probe(obj);
                                AutoPtr<IIterator> iter;
                                removed->GetIterator((IIterator**)&iter);
                                while (iter->HasNext(&hasNext), hasNext) {
                                    AutoPtr<IInterface> obj;
                                    iter->GetNext((IInterface**)&obj);
                                    AutoPtr<ILinkAddress> removed = ILinkAddress::Probe(obj);
                                    Boolean isAddressTypeMatches;
                                    AutoPtr<IInetAddress> removedAddr;
                                    removed->GetAddress((IInetAddress**)&removedAddr);
                                    AutoPtr<IInetAddress> addedAddr;
                                    added->GetAddress((IInetAddress**)&addedAddr);
                                    NetworkUtils::AddressTypeMatches(removedAddr, addedAddr, &isAddressTypeMatches);
                                    if (isAddressTypeMatches) {
                                        needToClean = TRUE;
                                        break;
                                    }
                                }
                            }
                            if (needToClean) {
                                if (DBG) {
                                    mHost->Log("onDataStateChanged: addr change, cleanup apns=%s oldLp=%s newLp=%s",
                                            TO_CSTR(((DataConnection*) dc.Get())->mApnContexts), TO_CSTR(result->mOldLp), TO_CSTR(result->mNewLp));
                                }
                                apnsToCleanup->AddAll(ICollection::Probe(((DataConnection*) dc.Get())->mApnContexts));
                            } else {
                                if (DBG) mHost->Log("onDataStateChanged: simple change");
                                AutoPtr<IIterator> it;
                                ((DataConnection*) dc.Get())->mApnContexts->GetIterator((IIterator**)&it);
                                Boolean hasNext;
                                while (it->HasNext(&hasNext), hasNext) {
                                    AutoPtr<IInterface> obj;
                                    it->GetNext((IInterface**)&obj);
                                    AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
                                    String apnType;
                                    apnContext->GetApnType(&apnType);
                                    mHost->mPhone->NotifyDataConnection(IPhoneConstants::REASON_LINK_PROPERTIES_CHANGED,
                                            apnType);
                                }
                            }
                        } else {
                            if (DBG) {
                                mHost->Log("onDataStateChanged: no changes");
                            }
                        }
                    } else {
                        apnsToCleanup->AddAll(ICollection::Probe(((DataConnection*) dc.Get())->mApnContexts));
                        if (DBG) {
                            mHost->Log("onDataStateChanged: interface change, cleanup apns=%s"
                                    , TO_CSTR(((DataConnection*) dc.Get())->mApnContexts));
                        }
                    }
                }
            }
        }
        Int32 active;
        newState->GetActive(&active);
        if (active == DATA_CONNECTION_ACTIVE_PH_LINK_UP) {
            isAnyDataCallActive = TRUE;
        }
        if (active == DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT) {
            isAnyDataCallDormant = TRUE;
        }
    }
    Int32 newOverallDataConnectionActiveState = mHost->mOverallDataConnectionActiveState;
    if (isAnyDataCallDormant && !isAnyDataCallActive) {
        // There is no way to indicate link activity per APN right now. So
        // Link Activity will be considered dormant only when all data calls
        // are dormant.
        // If a single data call is in dormant state and none of the data
        // calls are active broadcast overall link state as dormant.
        if (DBG) {
            mHost->Log("onDataStateChanged: Data Activity updated to DORMANT. stopNetStatePoll");
        }
        mHost->mDct->SendStopNetStatPoll(DctConstantsActivity_DORMANT);
        newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT;
    } else {
        if (DBG) {
            mHost->Log("onDataStateChanged: Data Activity updated to NONE. "
                    "isAnyDataCallActive = %d isAnyDataCallDormant = %d",
                    isAnyDataCallActive, isAnyDataCallDormant);
        }
        if (isAnyDataCallActive) {
            newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_UP;
            mHost->mDct->SendStartNetStatPoll(DctConstantsActivity_NONE);
        } else {
            newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE;
        }
    }
    // Temporary notification until RIL implementation is complete.
    if (mHost->mOverallDataConnectionActiveState != newOverallDataConnectionActiveState) {
        mHost->mOverallDataConnectionActiveState = newOverallDataConnectionActiveState;
        Int64 time = SystemClock::GetElapsedRealtimeNanos();
        Int32 dcPowerState;
        if (mHost->mOverallDataConnectionActiveState == DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE
                || mHost->mOverallDataConnectionActiveState == DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT) {
            dcPowerState = IDataConnectionRealTimeInfo::DC_POWER_STATE_LOW;
        }
        else if (mHost->mOverallDataConnectionActiveState == DATA_CONNECTION_ACTIVE_PH_LINK_UP) {
            dcPowerState = IDataConnectionRealTimeInfo::DC_POWER_STATE_HIGH;
        }
        else {
            dcPowerState = IDataConnectionRealTimeInfo::DC_POWER_STATE_UNKNOWN;
        }
        AutoPtr<IDataConnectionRealTimeInfo> dcRtInfo;
        CDataConnectionRealTimeInfo::New(time, dcPowerState, (IDataConnectionRealTimeInfo**)&dcRtInfo);
        mHost->Log("onDataStateChanged: notify DcRtInfo changed dcRtInfo=%s", TO_CSTR(dcRtInfo));
        mHost->mPhone->NotifyDataConnectionRealTimeInfo(dcRtInfo);
    }
    if (DBG) {
        mHost->Lr("onDataStateChanged: dcsToRetry=%s apnsToCleanup=%s",
                TO_CSTR(dcsToRetry), TO_CSTR(apnsToCleanup));
    }
    // Cleanup connections that have changed
    it = NULL;
    apnsToCleanup->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnContext> apnContext = IApnContext::Probe(obj);
        ((DcTrackerBase*) mHost->mDct.Get())->SendCleanUpConnection(TRUE, apnContext);
    }
    // Retry connections that have disappeared
    it = NULL;
    dcsToRetry->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IDataConnection> dc = IDataConnection::Probe(obj);
        if (DBG) mHost->Log("onDataStateChanged: send EVENT_LOST_CONNECTION dc.mTag=%d", ((DataConnection*) dc.Get())->mTag);
        ((DataConnection*) dc.Get())->SendMessage(DataConnection::EVENT_LOST_CONNECTION, ((DataConnection*) dc.Get())->mTag);
    }
    if (DBG) mHost->Log("onDataStateChanged: X");
    return NOERROR;
}

String DcController::DccDefaultState::GetName()
{
    return String("DcController::DccDefaultState");
}

//=============================================================================
// DcController
//=============================================================================
CAR_INTERFACE_IMPL(DcController, StateMachine, IDcController)

const Boolean DcController::DBG = TRUE;
const Boolean DcController::VDBG = FALSE;
const Int32 DcController::DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0;
const Int32 DcController::DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT = 1;
const Int32 DcController::DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2;
const Int32 DcController::DATA_CONNECTION_ACTIVE_UNKNOWN = Elastos::Core::Math::INT32_MAX_VALUE;

DcController::DcController()
    : mOverallDataConnectionActiveState(DATA_CONNECTION_ACTIVE_UNKNOWN)
{}

ECode DcController::constructor(
    /* [in] */ const String& name,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDcTrackerBase* dct,
    /* [in] */ IHandler* handler)
{
    StateMachine::constructor(name, handler);
    SetLogRecSize(300);
    Log("E ctor");
    mPhone = phone;
    mDct = dct;
    Log("X ctor");
    CArrayList::New((IArrayList**)&mDcListAll);
    CHashMap::New((IHashMap**)&mDcListActiveByCid);
    mDccDefaultState = new DccDefaultState(this);
    AddState(mDccDefaultState);
    SetInitialState(mDccDefaultState);
    return NOERROR;
}

ECode DcController::MakeDcc(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDcTrackerBase* dct,
    /* [in] */ IHandler* handler,
    /* [out] */ IDcController** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<DcController> dcc = new DcController();
    dcc->constructor(String("Dcc"), phone, dct, handler);

    IStateMachine::Probe(dcc)->Start();
    *result = dcc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DcController::Dispose()
{
    Log("dispose: call quiteNow()");
    QuitNow();
    return NOERROR;
}

ECode DcController::AddDc(
    /* [in] */ IDataConnection* dc)
{
    mDcListAll->Add(dc);
    return NOERROR;
}

ECode DcController::RemoveDc(
    /* [in] */ IDataConnection* dc)
{
    AutoPtr<IInteger32> i32;
    CInteger32::New(((DataConnection*) dc)->mCid, (IInteger32**)&i32);
    mDcListActiveByCid->Remove(i32);
    mDcListAll->Remove(dc);
    return NOERROR;
}

ECode DcController::AddActiveDcByCid(
    /* [in] */ IDataConnection* dc)
{
    if (DBG && ((DataConnection*) dc)->mCid < 0) {
        Log("addActiveDcByCid dc.mCid < 0 dc=%s", TO_CSTR(dc));
    }
    AutoPtr<IInteger32> i32;
    CInteger32::New(((DataConnection*) dc)->mCid, (IInteger32**)&i32);
    mDcListActiveByCid->Put(i32, dc);
    return NOERROR;
}

ECode DcController::RemoveActiveDcByCid(
    /* [in] */ IDataConnection* dc)
{
    AutoPtr<IInteger32> i32;
    CInteger32::New(((DataConnection*) dc)->mCid, (IInteger32**)&i32);
    AutoPtr<IInterface> obj;
    mDcListActiveByCid->Remove(i32, (IInterface**)&obj);
    AutoPtr<IDataConnection> removedDc = IDataConnection::Probe(obj);
    if (DBG && removedDc == NULL) {
        Log("removeActiveDcByCid removedDc=null dc=%s", TO_CSTR(dc));
    }
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcController::Lr(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    LogAndAddLogRec(String(msgBuf));
    return NOERROR;
}

ECode DcController::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::D(name, msgBuf);
}

ECode DcController::Loge(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String name;
    GetName(&name);
    return Logger::E(name, msgBuf);
}

String DcController::GetWhatToString(
    /* [in] */ Int32 what)
{
    String info(NULL);
    DataConnection::CmdToString(what, &info);
    if (info.IsNull()) {
        DcAsyncChannel::CmdToString(what, &info);
    }
    return info;
}

ECode DcController::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("mDcListAll=%s mDcListActiveByCid=%s", TO_CSTR(mDcListAll), TO_CSTR(mDcListActiveByCid));
    *result = rev;
    return NOERROR;
}

ECode DcController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StateMachine::Dump(fd, pw, args);
    pw->Println(String(" mPhone=") + TO_STR(mPhone));
    pw->Println(String(" mDcListAll=") + TO_STR(mDcListAll));
    pw->Println(String(" mDcListActiveByCid=") + TO_STR(mDcListActiveByCid));
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
