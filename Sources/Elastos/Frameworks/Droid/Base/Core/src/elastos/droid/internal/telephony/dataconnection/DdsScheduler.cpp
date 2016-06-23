
#include "elastos/droid/internal/telephony/dataconnection/CDdsScheduler.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchState.h"
#include "elastos/droid/internal/telephony/dataconnection/DctController.h"
#include "elastos/droid/internal/telephony/dataconnection/DdsScheduler.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/Telephony.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IModemStackController;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::Telephony;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DdsScheduler::NetworkRequestInfo
//=============================================================================
DdsScheduler::NetworkRequestInfo::NetworkRequestInfo(
    /* [in] */ DdsScheduler* host)
    : mAccepted(FALSE)
    , mHost(host)
{}


ECode DdsScheduler::NetworkRequestInfo::constructor(
    /* [in] */ INetworkRequest* req)
{
    mRequest = req;
    return NOERROR;
}

ECode DdsScheduler::NetworkRequestInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("%s accepted = %d", TO_CSTR(mRequest), mAccepted);
    *result = rev;
    return NOERROR;
}

ECode DdsScheduler::NetworkRequestInfo::GetMRequest(
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRequest;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DdsScheduler::NetworkRequestInfo::GetMAccepted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccepted;
    return NOERROR;
}

ECode DdsScheduler::NetworkRequestInfo::SetMAccepted(
    /* [in] */ Boolean accepted)
{
    mAccepted = accepted;
    return NOERROR;
}

//=============================================================================
// DdsScheduler::DefaultState
//=============================================================================
const String DdsScheduler::DefaultState::TAG = DdsScheduler::TAG + "[DefaultState]";

DdsScheduler::DefaultState::DefaultState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}


ECode DdsScheduler::DefaultState::Enter()
{
    return NOERROR;
}

ECode DdsScheduler::DefaultState::Exit()
{
    return NOERROR;
}

ECode DdsScheduler::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    switch(msgWhat) {
        case IDdsSchedulerAc::EVENT_ADD_REQUEST: {
            AutoPtr<INetworkRequest> nr = INetworkRequest::Probe(msgObj);
            Logger::D(TAG, "EVENT_ADD_REQUEST = %s", TO_CSTR(nr));
            mHost->AddRequest(nr);
            AutoPtr<IMessage> msg;
            mHost->ObtainMessage(IDdsSchedulerAc::REQ_DDS_ALLOCATION, nr, (IMessage**)&msg);
            mHost->SendMessage(msg);
            break;
        }
        case IDdsSchedulerAc::EVENT_REMOVE_REQUEST: {
            AutoPtr<INetworkRequest> nr = INetworkRequest::Probe(msgObj);
            Logger::D(TAG, "EVENT_REMOVE_REQUEST%s", TO_CSTR(nr));
            mHost->RemoveRequest(nr);
            AutoPtr<IMessage> msg;
            mHost->ObtainMessage(IDdsSchedulerAc::REQ_DDS_FREE, nr, (IMessage**)&msg);
            mHost->SendMessage(msg);
            break;
        }
        case IDdsSchedulerAc::REQ_DDS_ALLOCATION: {
            String name;
            mHost->GetCurrentState()->GetName(&name);
            Logger::D(TAG, "REQ_DDS_ALLOCATION, currentState = %s",
                    name.string());
            *result = HANDLED;
            return NOERROR;
        }
        case IDdsSchedulerAc::REQ_DDS_FREE: {
            String name;
            mHost->GetCurrentState()->GetName(&name);
            Logger::D(TAG, "REQ_DDS_FREE, currentState = %s", name.string());
            *result = HANDLED;
            return NOERROR;
        }
        default: {
            Logger::D(TAG, "unknown msg = %s", TO_CSTR(msg));
            break;
        }
    }
    *result = HANDLED;
    return NOERROR;
}

String DdsScheduler::DefaultState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DdsScheduler::DdsIdleState
//=============================================================================
const String DdsScheduler::DdsIdleState::TAG = DdsScheduler::TAG + "[DdsIdleState]";

DdsScheduler::DdsIdleState::DdsIdleState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsIdleState::Enter()
{
    Logger::D(TAG, "Enter");
    AutoPtr<INetworkRequest> nr;
    mHost->GetFirstWaitingRequest((INetworkRequest**)&nr);
    if (nr != NULL) {
        Logger::D(TAG, "Request pending = %s", TO_CSTR(nr));
        Boolean isDdsSwitchRequired;
        mHost->IsDdsSwitchRequired(nr, &isDdsSwitchRequired);
        if (!isDdsSwitchRequired) {
            mHost->TransitionTo(mHost->mDdsReservedState);
        } else {
            mHost->TransitionTo(mHost->mDdsSwitchState);
        }
    } else {
        Logger::D(TAG, "Nothing to process");
    }
    return NOERROR;
}

ECode DdsScheduler::DdsIdleState::Exit()
{
    return Logger::D(TAG, "Exit");
}

ECode DdsScheduler::DdsIdleState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    switch(msgWhat) {
        case IDdsSchedulerAc::REQ_DDS_ALLOCATION: {
            Logger::D(TAG, "REQ_DDS_ALLOCATION");
            AutoPtr<INetworkRequest> n = INetworkRequest::Probe(msgObj);
            Boolean isDdsSwitchRequired;
            mHost->IsDdsSwitchRequired(n, &isDdsSwitchRequired);
            if (!isDdsSwitchRequired) {
                mHost->TransitionTo(mHost->mDdsReservedState);
            } else {
                mHost->TransitionTo(mHost->mDdsSwitchState);
            }
            *result = HANDLED;
            return NOERROR;
        }
        default: {
            Logger::D(TAG, "unknown msg = %s", TO_CSTR(msg));
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    return NOERROR;
}

String DdsScheduler::DdsIdleState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DdsScheduler::DdsReservedState
//=============================================================================
const String DdsScheduler::DdsReservedState::TAG = DdsScheduler::TAG + "[DdsReservedState]";

DdsScheduler::DdsReservedState::DdsReservedState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsReservedState::HandleOtherSubRequests()
{
    AutoPtr<INetworkRequest> nr;
    mHost->GetFirstWaitingRequest((INetworkRequest**)&nr);
    Int64 subIdFromNetworkRequest;
    mHost->GetSubIdFromNetworkRequest(nr, &subIdFromNetworkRequest);
    Int64 currentDds;
    mHost->GetCurrentDds(&currentDds);
    if (nr == NULL) {
        Logger::D(TAG, "No more requests to accept");
        mHost->TransitionTo(mHost->mDdsAutoRevertState);
    } else if (subIdFromNetworkRequest != currentDds) {
        Logger::D(TAG, "Switch required for %s", TO_CSTR(nr));
        mHost->TransitionTo(mHost->mDdsSwitchState);
    } else {
        Logger::E(TAG, "This request could not get accepted, start over. nr = %s", TO_CSTR(nr));
        //reset state machine to stable state.
        mHost->TransitionTo(mHost->mDdsAutoRevertState);
    }
    return NOERROR;
}

ECode DdsScheduler::DdsReservedState::Enter()
{
    Logger::D(TAG, "Enter");
    Boolean acceptWaitingRequest;
    mHost->AcceptWaitingRequest(&acceptWaitingRequest);
    if (!acceptWaitingRequest) {
        HandleOtherSubRequests();
    }
    return NOERROR;
}

ECode DdsScheduler::DdsReservedState::Exit()
{
    return Logger::D(TAG, "Exit");
}

ECode DdsScheduler::DdsReservedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isMultiDataSupported;
    mHost->IsMultiDataSupported(&isMultiDataSupported);
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    switch(msgWhat) {
        case IDdsSchedulerAc::REQ_DDS_ALLOCATION: {
            Logger::D(TAG, "REQ_DDS_ALLOCATION");
            AutoPtr<INetworkRequest> n = INetworkRequest::Probe(msgObj);
            Int64 subIdFromNetworkRequest;
            mHost->GetSubIdFromNetworkRequest(n, &subIdFromNetworkRequest);
            Int64 currentDds;
            mHost->GetCurrentDds(&currentDds);
            if (subIdFromNetworkRequest == currentDds) {
                Logger::D(TAG, "Accepting simultaneous request for current sub");
                mHost->NotifyRequestAccepted(n);
            } else if (isMultiDataSupported) {
                Logger::D(TAG, "Incoming request is for on-demand subscription, n = %s", TO_CSTR(n));
                mHost->RequestPsAttach(n);
            }
            *result = HANDLED;
            return NOERROR;
        }
        case IDdsSchedulerAc::REQ_DDS_FREE: {
            Logger::D(TAG, "REQ_DDS_FREE");
            Boolean acceptWaitingRequest;
            mHost->AcceptWaitingRequest(&acceptWaitingRequest);
            if (!acceptWaitingRequest) {
                Logger::D(TAG, "Can't process next in this DDS");
                HandleOtherSubRequests();
            } else {
                Logger::D(TAG, "Processing next in same DDS");
            }
            *result = HANDLED;
            return NOERROR;
        }
        case IDdsSchedulerAc::EVENT_ON_DEMAND_PS_ATTACH_DONE: {
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
            AutoPtr<INetworkRequest> n = INetworkRequest::Probe(((AsyncResult*) ar.Get())->mResult);
            if (((AsyncResult*) ar.Get())->mException == NULL) {
                mHost->UpdateCurrentDds(n);
                mHost->TransitionTo(mHost->mPsAttachReservedState);
            } else {
                Logger::D(TAG, "Switch failed, ignore the req = %s", TO_CSTR(n));
                //discard the request so that we can process other pending reqeusts
                mHost->RemoveRequest(n);
            }
            *result = HANDLED;
            return NOERROR;
        }
        default: {
            Logger::D(TAG, "unknown msg = %s", TO_CSTR(msg));
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    return NOERROR;
}

String DdsScheduler::DdsReservedState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DdsScheduler::PsAttachReservedState
//=============================================================================
const String DdsScheduler::PsAttachReservedState::TAG = DdsScheduler::TAG + "[PSAttachReservedState]";

DdsScheduler::PsAttachReservedState::PsAttachReservedState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::PsAttachReservedState::HandleOtherSubRequests()
{
    AutoPtr<INetworkRequest> nr;
    mHost->GetFirstWaitingRequest((INetworkRequest**)&nr);
    if (nr == NULL) {
        Logger::D(TAG, "No more requests to accept");
    } else {
        Int64 subIdFromNetworkRequest;
        mHost->GetSubIdFromNetworkRequest(nr, &subIdFromNetworkRequest);
        Int64 currentDds;
        mHost->GetCurrentDds(&currentDds);
        if (subIdFromNetworkRequest != currentDds) {
            Logger::D(TAG, "Next request is not for current on-demand PS sub(DSDA). nr = %s", TO_CSTR(nr));
            Boolean isAlreadyAccepted;
            mHost->IsAlreadyAccepted(nr, &isAlreadyAccepted);
            if (isAlreadyAccepted) {
                Logger::D(TAG, "Next request is already accepted on other sub in DSDA mode. nr = %s", TO_CSTR(nr));
                mHost->TransitionTo(mHost->mDdsReservedState);
                return NOERROR;
            }
        }
    }
    mHost->TransitionTo(mHost->mDdsAutoRevertState);
    return NOERROR;
}

ECode DdsScheduler::PsAttachReservedState::Enter()
{
    Logger::D(TAG, "Enter");
    Boolean acceptWaitingRequest;
    mHost->AcceptWaitingRequest(&acceptWaitingRequest);
    if (!acceptWaitingRequest) {
        HandleOtherSubRequests();
    }
    return NOERROR;
}

ECode DdsScheduler::PsAttachReservedState::Exit()
{
    Logger::D(TAG, "Exit");
    //Request PS Detach on currentDds.
    mHost->RequestPsDetach();
    //Update currentDds back to defaultDataSub.
    mHost->UpdateCurrentDds(NULL);
    return NOERROR;
}

ECode DdsScheduler::PsAttachReservedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    switch(msgWhat) {
        case IDdsSchedulerAc::REQ_DDS_ALLOCATION: {
            Logger::D(TAG, "REQ_DDS_ALLOCATION");
            AutoPtr<INetworkRequest> n = INetworkRequest::Probe(msgObj);
            Logger::D(TAG, "Accepting request in dual-data mode, req = %s", TO_CSTR(n));
            mHost->NotifyRequestAccepted(n);
            *result = HANDLED;
            return NOERROR;
        }
        case IDdsSchedulerAc::REQ_DDS_FREE: {
            Logger::D(TAG, "REQ_DDS_FREE");
            Boolean acceptWaitingRequest;
            mHost->AcceptWaitingRequest(&acceptWaitingRequest);
            if (!acceptWaitingRequest) {
                //No more requests for current sub. If there are few accepted requests
                //for defaultDds then move to DdsReservedState so that on-demand PS
                //detach on current sub can be triggered.
                HandleOtherSubRequests();
            }
            *result = HANDLED;
            return NOERROR;
        }
        default: {
            Logger::D(TAG, "unknown msg = %s", TO_CSTR(msg));
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    return NOERROR;
}

String DdsScheduler::PsAttachReservedState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DdsScheduler::DdsSwitchState
//=============================================================================
const String DdsScheduler::DdsSwitchState::TAG = DdsScheduler::TAG + "[DdsSwitchState]";

DdsScheduler::DdsSwitchState::DdsSwitchState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsSwitchState::Enter()
{
    Logger::D(TAG, "Enter");
    AutoPtr<INetworkRequest> nr;
    mHost->GetFirstWaitingRequest((INetworkRequest**)&nr);
    if (nr != NULL) {
        mHost->TriggerSwitch(nr);
    } else {
        Logger::D(TAG, "Error");
    }
    return NOERROR;
}

ECode DdsScheduler::DdsSwitchState::Exit()
{
    return Logger::D(TAG, "Exit");
}

ECode DdsScheduler::DdsSwitchState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    switch(msgWhat) {
        case IDdsSchedulerAc::EVENT_ON_DEMAND_PS_ATTACH_DONE:
        case IDdsSchedulerAc::EVENT_ON_DEMAND_DDS_SWITCH_DONE : {
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
            AutoPtr<INetworkRequest> n = INetworkRequest::Probe(((AsyncResult*) ar.Get())->mResult);
            if (((AsyncResult*) ar.Get())->mException == NULL) {
                mHost->UpdateCurrentDds(n);
                if (msgWhat == IDdsSchedulerAc::EVENT_ON_DEMAND_PS_ATTACH_DONE) {
                    mHost->TransitionTo(mHost->mPsAttachReservedState);
                } else {
                    mHost->TransitionTo(mHost->mDdsReservedState);
                }
            } else {
                Logger::D(TAG, "Switch failed, move back to idle state");
                //discard the request so that we can process other pending reqeusts
                mHost->RemoveRequest(n);
                mHost->TransitionTo(mHost->mDdsIdleState);
            }
            *result = HANDLED;
            return NOERROR;
        }
        default: {
            Logger::D(TAG, "unknown msg = %s", TO_CSTR(msg));
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    return NOERROR;
}

String DdsScheduler::DdsSwitchState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DdsScheduler::DdsAutoRevertState
//=============================================================================
const String DdsScheduler::DdsAutoRevertState::TAG = DdsScheduler::TAG + "[DdsAutoRevertState]";

DdsScheduler::DdsAutoRevertState::DdsAutoRevertState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsAutoRevertState::Enter()
{
    Logger::D(TAG, "Enter");
    mHost->TriggerSwitch(NULL);
    return NOERROR;
}

ECode DdsScheduler::DdsAutoRevertState::Exit()
{
    return Logger::D(TAG, "Exit");
}

ECode DdsScheduler::DdsAutoRevertState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    switch(msgWhat) {
         case IDdsSchedulerAc::EVENT_ON_DEMAND_PS_ATTACH_DONE: {
            Logger::D(TAG, "SET_DDS_DONE");
            mHost->UpdateCurrentDds(NULL);
            mHost->TransitionTo(mHost->mDdsIdleState);
            *result = HANDLED;
            return NOERROR;
        }
        default: {
            Logger::D(TAG, "unknown msg = %s", TO_CSTR(msg));
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    return NOERROR;
}

String DdsScheduler::DdsAutoRevertState::GetName()
{
    return String("DcSwitchState::IdleState");
}

//=============================================================================
// DdsScheduler
//=============================================================================
CAR_INTERFACE_IMPL(DdsScheduler, StateMachine, IDdsScheduler)

const String DdsScheduler::TAG("DdsScheduler");
AutoPtr<IDdsScheduler> DdsScheduler::sDdsScheduler;

ECode DdsScheduler::CreateDdsScheduler(
    /* [out] */ IDdsScheduler** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IDdsScheduler> ddsScheduler;
    CDdsScheduler::New((IDdsScheduler**)&ddsScheduler);
    IStateMachine::Probe(ddsScheduler)->Start();
    *result = ddsScheduler;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DdsScheduler::GetInstance(
    /* [out] */ IDdsScheduler** result)
{
    VALIDATE_NOT_NULL(result)

    if (sDdsScheduler == NULL) {
        CreateDdsScheduler((IDdsScheduler**)&sDdsScheduler);
    }
    Logger::D(TAG, "getInstance = %s", TO_CSTR(sDdsScheduler));
    *result = sDdsScheduler;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DdsScheduler::Init()
{
    if (sDdsScheduler == NULL) {
        GetInstance((IDdsScheduler**)&sDdsScheduler);
    }
    ((DdsScheduler*) sDdsScheduler.Get())->RegisterCallbacks();
    Logger::D(TAG, "init = %s", TO_CSTR(sDdsScheduler));
    return NOERROR;
}

DdsScheduler::DdsScheduler()
    : mCurrentDds(ISubscriptionManager::INVALID_SUB_ID)
    , MODEM_DATA_CAPABILITY_UNKNOWN(-1)
    , MODEM_SINGLE_DATA_CAPABLE(1)
    , MODEM_DUAL_DATA_CAPABLE(2)
    , OVERRIDE_MODEM_DUAL_DATA_CAP_PROP("persist.test.msim.config")
{
    mDefaultState = new DefaultState(this);
    mDdsIdleState = new DdsIdleState(this);
    mDdsReservedState = new DdsReservedState(this);
    mPsAttachReservedState = new PsAttachReservedState(this);
    mDdsSwitchState = new DdsSwitchState(this);
    mDdsAutoRevertState = new DdsAutoRevertState(this);
    AutoPtr<IArrayList> arrayList;
    CArrayList::New((IArrayList**)&arrayList);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IList> mInbox;
    helper->SynchronizedList(IList::Probe(arrayList), (IList**)&mInbox);

    StateMachine::constructor(String("DdsScheduler"));
    AddState(mDefaultState);
    AddState(mDdsIdleState, mDefaultState);
    AddState(mDdsReservedState, mDefaultState);
    AddState(mDdsSwitchState, mDefaultState);
    AddState(mDdsAutoRevertState, mDefaultState);
    AddState(mPsAttachReservedState, mDefaultState);
    SetInitialState(mDdsIdleState);
}

ECode DdsScheduler::constructor()
{
    return NOERROR;
}

ECode DdsScheduler::AddRequest(
    /* [in] */ INetworkRequest* req)
{
    AutoLock lock(mInbox);
    AutoPtr<NetworkRequestInfo> info = new NetworkRequestInfo(this);
    info->constructor(req);
    mInbox->Add(TO_IINTERFACE(info));
    return NOERROR;
}

ECode DdsScheduler::RemoveRequest(
    /* [in] */ INetworkRequest* req)
{
    AutoLock lock(mInbox);
    Int32 size;
    mInbox->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mInbox->Get(i, (IInterface**)&obj);
        AutoPtr<NetworkRequestInfo> tempNrInfo = (NetworkRequestInfo*) IObject::Probe(obj);
        Boolean isEquals;
        IObject::Probe(tempNrInfo->mRequest)->Equals(req, &isEquals);
        if (isEquals) {
            mInbox->Remove(i);
        }
    }
    return NOERROR;
}

ECode DdsScheduler::MarkAccepted(
    /* [in] */ INetworkRequest* req)
{
    AutoLock lock(mInbox);
    Int32 size;
    mInbox->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mInbox->Get(i, (IInterface**)&obj);
        AutoPtr<NetworkRequestInfo> tempNrInfo = (NetworkRequestInfo*) IObject::Probe(obj);
        Boolean isEquals;
        IObject::Probe(tempNrInfo->mRequest)->Equals(req, &isEquals);
        if (isEquals) {
            tempNrInfo->mAccepted = TRUE;
        }
    }
    return NOERROR;
}

ECode DdsScheduler::IsAlreadyAccepted(
    /* [in] */ INetworkRequest* nr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock lock(mInbox);
        Int32 size;
        mInbox->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mInbox->Get(i, (IInterface**)&obj);
            AutoPtr<NetworkRequestInfo> tempNrInfo = (NetworkRequestInfo*) IObject::Probe(obj);
            Boolean isEquals;
            IObject::Probe(tempNrInfo->mRequest)->Equals(nr, &isEquals);
            if (isEquals) {
                *result = (tempNrInfo->mAccepted == TRUE);
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DdsScheduler::GetFirstWaitingRequest(
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(mInbox);
    Boolean isEmpty;
    mInbox->IsEmpty(&isEmpty);
    if (isEmpty) {
        *result = NULL;
        return NOERROR;
    } else {
        AutoPtr<IInterface> obj;
        mInbox->Get(0, (IInterface**)&obj);
        *result = ((NetworkRequestInfo*) IObject::Probe(obj))->mRequest;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode DdsScheduler::AcceptWaitingRequest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean anyAccepted = FALSE;
    {
        AutoLock lock(mInbox);
        Boolean isEmpty;
        mInbox->IsEmpty(&isEmpty);
        if (!isEmpty) {
            Int32 size;
            mInbox->GetSize(&size);
            for (Int32 i =0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mInbox->Get(i, (IInterface**)&obj);
                AutoPtr<INetworkRequest> nr = ((NetworkRequestInfo*) IObject::Probe(obj))->mRequest;
                Int64 subIdFromNetworkRequest;
                GetSubIdFromNetworkRequest(nr, &subIdFromNetworkRequest);
                Int64 currentDds;
                GetCurrentDds(&currentDds);
                if (subIdFromNetworkRequest == currentDds) {
                    NotifyRequestAccepted(nr);
                    anyAccepted = TRUE;
                }
            }
        } else {
            Logger::D(TAG, "No request can be accepted for current sub");
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = anyAccepted;
    return NOERROR;
}

ECode DdsScheduler::NotifyRequestAccepted(
    /* [in] */ INetworkRequest* nr)
{
    Boolean isAlreadyAccepted;
    IsAlreadyAccepted(nr, &isAlreadyAccepted);
    if (!isAlreadyAccepted) {
        MarkAccepted(nr);
        Logger::D(TAG, "Accepted req = %s", TO_CSTR(nr));
        assert(0 && "SubscriptionController");
        // AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
        // subController->NotifyOnDemandDataSubIdChanged(nr);
    } else {
        Logger::D(TAG, "Already accepted/notified req = %s", TO_CSTR(nr));
    }
    return NOERROR;
}

ECode DdsScheduler::IsDdsSwitchRequired(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 subIdFromNetworkRequest;
    GetSubIdFromNetworkRequest(n, &subIdFromNetworkRequest);
    Int64 currentDds;
    GetCurrentDds(&currentDds);
    if (subIdFromNetworkRequest == currentDds) {
        Logger::D(TAG, "DDS switch required for req = %s", TO_CSTR(n));
        *result = TRUE;
        return NOERROR;
    } else {
        Logger::D(TAG, "DDS switch not required for req = %s", TO_CSTR(n));
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DdsScheduler::GetCurrentDds(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    assert(0 && "SubscriptionController");
    // AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
    // if (mCurrentDds == ISubscriptionManager::INVALID_SUB_ID) {
    //     subController->GetDefaultDataSubId(&mCurrentDds);
    // }
    Logger::D(TAG, "mCurrentDds = %ld", mCurrentDds);
    *result = mCurrentDds;
    return NOERROR;
}

ECode DdsScheduler::UpdateCurrentDds(
    /* [in] */ INetworkRequest* n)
{
    GetSubIdFromNetworkRequest(n, &mCurrentDds);
    Logger::D(TAG, "mCurrentDds = %ld", mCurrentDds);
    return NOERROR;
}

ECode DdsScheduler::GetSubIdFromNetworkRequest(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    assert(0 && "SubscriptionController");
    // AutoPtr<ISubscriptionController> subController = SubscriptionController::GetInstance();
    // return subController->GetSubIdFromNetworkRequest(n);
    return NOERROR;
}

ECode DdsScheduler::RequestDdsSwitch(
    /* [in] */ INetworkRequest* n)
{
    if (n != NULL) {
        mDctController->SetOnDemandDataSubId(n);
    } else {
        // set DDS to user configured defaultDds SUB.
        // requestPsAttach would make sure that OemHook api to set DDS
        // is called as well as PS ATTACH is requested.
        RequestPsAttach(NULL);
    }
    return NOERROR;
}

ECode DdsScheduler::RequestPsAttach(
    /* [in] */ INetworkRequest* n)
{
    return mDctController->DoPsAttach(n);
}

ECode DdsScheduler::RequestPsDetach()
{
    return mDctController->DoPsDetach();
}

ECode DdsScheduler::GetMaxDataAllowed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    assert(0 && "ModemStackController");
    // AutoPtr<IModemStackController> modemStackController = ModemStackController::GetInstance();
    // Logger::D(TAG, "ModemStackController = %s", TO_CSTR(modemStackController));
    // Int32 maxData;
    // modemStackController->GetMaxDataAllowed(&maxData);
    // Logger::D(TAG, "modem value of max_data = %d", maxData);
    // Int32 override = SystemProperties::GetInt32(OVERRIDE_MODEM_DUAL_DATA_CAP_PROP,
    //         MODEM_DATA_CAPABILITY_UNKNOWN);
    // if (override != MODEM_DATA_CAPABILITY_UNKNOWN) {
    //     Logger::D(TAG, "Overriding modem max_data_value with %d", override);
    //     maxData = override;
    // }
    // *result = maxData;
    return NOERROR;
}

ECode DdsScheduler::RegisterCallbacks()
{
    if (mDctController == NULL) {
        Logger::D(TAG, "registerCallbacks");
        DctController::GetInstance((IDctController**)&mDctController);
        AutoPtr<IHandler> handler;
        GetHandler((IHandler**)&handler);
        mDctController->RegisterForOnDemandDataSwitchInfo(handler,
                IDdsSchedulerAc::EVENT_ON_DEMAND_DDS_SWITCH_DONE, NULL);
        mDctController->RegisterForOnDemandPsAttach(handler,
                IDdsSchedulerAc::EVENT_ON_DEMAND_PS_ATTACH_DONE, NULL);
   }
   return NOERROR;
}

ECode DdsScheduler::TriggerSwitch(
    /* [in] */ INetworkRequest* n)
{
    Boolean multiDataSupported = FALSE;
    Boolean isMultiDataSupported;
    IsMultiDataSupported(&isMultiDataSupported);
    if (isMultiDataSupported) {
        multiDataSupported = TRUE;
        Logger::D(TAG, "Simultaneous dual-data supported");
    } else {
        Logger::D(TAG, "Simultaneous dual-data NOT supported");
    }
    if ((n != NULL) && multiDataSupported) {
        RequestPsAttach(n);
    } else {
        RequestDdsSwitch(n);
    }
    return NOERROR;
}

ECode DdsScheduler::IsMultiDataSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean flag = FALSE;
    Int32 maxDataAllowed;
    GetMaxDataAllowed(&maxDataAllowed);
    if (maxDataAllowed == MODEM_DUAL_DATA_CAPABLE) {
        flag = TRUE;
    }
    *result = flag;
    return NOERROR;
}

ECode DdsScheduler::IsAnyRequestWaiting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoLock lock(mInbox);
    Boolean isEmpty;
    mInbox->IsEmpty(&isEmpty);
    *result = !isEmpty;
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
