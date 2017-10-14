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

#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcSwitchAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcSwitchState.h"
#include "elastos/droid/internal/telephony/dataconnection/CDctController.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchState.h"
#include "elastos/droid/internal/telephony/dataconnection/DctController.h"
#include "elastos/droid/internal/telephony/dataconnection/DdsScheduler.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/net/NetworkRequest.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Net.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Telephony::IISub;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_DISCONNECTED;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::CRegistrantList;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Provider::Telephony;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CThrowable;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IThread;
using Elastos::Core::IThrowable;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DctController::SubHandler
//=============================================================================
DctController::SubHandler::SubHandler(
    /* [in] */ DctController* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode DctController::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IAsyncResult> ar;
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == EVENT_PHONE1_DETACH ||
            msgWhat == EVENT_PHONE2_DETACH ||
            msgWhat == EVENT_PHONE3_DETACH ||
            msgWhat == EVENT_PHONE4_DETACH) {
        mHost->Logd("EVENT_PHONE%d_DETACH: mRequestedDataPhone=%d",
                msgWhat, mHost->mRequestedDataPhone);
        mHost->mCurrentDataPhone = PHONE_NONE;
        if (mHost->mRequestedDataPhone != PHONE_NONE) {
            mHost->mCurrentDataPhone = mHost->mRequestedDataPhone;
            mHost->mRequestedDataPhone = PHONE_NONE;
            AutoPtr<IIterator> itrType;
            mHost->mApnTypes->GetIterator((IIterator**)&itrType);
            Boolean hasNext;
            while (itrType->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                itrType->GetNext((IInterface**)&obj);
                Int32 i32;
                (*mHost->mDcSwitchAsyncChannel)[mHost->mCurrentDataPhone]->ConnectSync(TO_STR(obj), &i32);
            }
            mHost->mApnTypes->Clear();
        }
    }
    else if (msgWhat == EVENT_PHONE1_RADIO_OFF ||
            msgWhat == EVENT_PHONE2_RADIO_OFF ||
            msgWhat == EVENT_PHONE3_RADIO_OFF ||
            msgWhat == EVENT_PHONE4_RADIO_OFF) {
        mHost->Logd("EVENT_PHONE%d_RADIO_OFF.", (msgWhat - EVENT_PHONE1_RADIO_OFF + 1));
        (*mHost->mServicePowerOffFlag)[msgWhat - EVENT_PHONE1_RADIO_OFF] = TRUE;
    }
    return NOERROR;
}

//=============================================================================
// DctController::SubIDataStateChangedCallback
//=============================================================================
CAR_INTERFACE_IMPL(DctController::SubIDataStateChangedCallback, Object, IDataStateChangedCallback)

DctController::SubIDataStateChangedCallback::SubIDataStateChangedCallback(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::SubIDataStateChangedCallback::OnDataStateChanged(
    /* [in] */ Int64 subId,
    /* [in] */ const String& state,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnName,
    /* [in] */ const String& apnType,
    /* [in] */ Boolean unavailable)
{
    mHost->Logd("[DataStateChanged]:state=%s,reason=%s,apnName=%s,apnType=%s,from subId=%lld",
            state.string(), reason.string(), apnName.string(), apnType.string(), subId);
    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    (*mHost->mDcSwitchState)[phoneId]->NotifyDataConnection(phoneId, state, reason,
            apnName, apnType, unavailable);
    return NOERROR;
}

//=============================================================================
// DctController::DataStateReceiver
//=============================================================================
DctController::DataStateReceiver::DataStateReceiver(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::DataStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoLock lock(this);
    String action;
    intent->GetAction(&action);
    if (action.Equals(ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED)) {
        AutoPtr<IBundle> bundle;
        intent->GetExtras((IBundle**)&bundle);
        AutoPtr<IServiceState> ss;
        CServiceState::NewFromBundle(bundle, (IServiceState**)&ss);
        Int64 subId;
        intent->GetInt64Extra(IPhoneConstants::SUBSCRIPTION_KEY, IPhoneConstants::SUB1, &subId);
        Int32 phoneId;
        SubscriptionManager::GetPhoneId(subId, &phoneId);
        // for the case of network out of service when bootup (ignore dummy values too)
        Boolean isActiveSubId;
        mHost->IsActiveSubId(subId, &isActiveSubId);
        Boolean isValidSubId;
        SubscriptionManager::IsValidSubId(subId, &isValidSubId);
        if (!isValidSubId || (subId < 0) || !isActiveSubId) {
            // FIXME: Maybe add SM.IsRealSubId(subId)??
            mHost->Logd("DataStateReceiver: ignore invalid subId=%lld phoneId = %d",
                    subId, phoneId);
            return NOERROR;
        }
        Boolean isValidPhoneId;
        SubscriptionManager::IsValidPhoneId(phoneId, &isValidPhoneId);
        if (!isValidPhoneId) {
            mHost->Logd("DataStateReceiver: ignore invalid phoneId=%d", phoneId);
            return NOERROR;
        }
        Boolean prevPowerOff = (*mHost->mServicePowerOffFlag)[phoneId];
        if (ss != NULL) {
            Int32 state;
            ss->GetState(&state);
            switch (state) {
                case IServiceState::STATE_POWER_OFF:
                    (*mHost->mServicePowerOffFlag)[phoneId] = TRUE;
                    mHost->Logd("DataStateReceiver: STATE_POWER_OFF Intent from phoneId=%d", phoneId);
                    break;
                case IServiceState::STATE_IN_SERVICE:
                    (*mHost->mServicePowerOffFlag)[phoneId] = FALSE;
                    mHost->Logd("DataStateReceiver: STATE_IN_SERVICE Intent from phoneId=%d", phoneId);
                    break;
                case IServiceState::STATE_OUT_OF_SERVICE:
                    mHost->Logd("DataStateReceiver: STATE_OUT_OF_SERVICE Intent from phoneId=%d", phoneId);
                    if ((*mHost->mServicePowerOffFlag)[phoneId]) {
                        (*mHost->mServicePowerOffFlag)[phoneId] = FALSE;
                    }
                    break;
                case IServiceState::STATE_EMERGENCY_ONLY:
                    mHost->Logd("DataStateReceiver: STATE_EMERGENCY_ONLY Intent from phoneId=%d", phoneId);
                    break;
                default:
                    mHost->Logd("DataStateReceiver: SERVICE_STATE_CHANGED invalid state");
                    break;
            }
            Int32 dataConnectionFromSetting;
            mHost->GetDataConnectionFromSetting(&dataConnectionFromSetting);
            if (prevPowerOff && (*mHost->mServicePowerOffFlag)[phoneId] == FALSE &&
                    mHost->mCurrentDataPhone == PHONE_NONE &&
                    phoneId == dataConnectionFromSetting) {
                mHost->Logd("DataStateReceiver: Current Phone is none and default phoneId=%d"
                        ", then EnableApnType()", phoneId);
                Int32 i32;
                mHost->EnableApnType(subId, IPhoneConstants::APN_TYPE_DEFAULT, &i32);
            }
        }
    }
    return NOERROR;
}

//=============================================================================
// DctController::SwitchInfo
//=============================================================================
DctController::SwitchInfo::SwitchInfo(
    /* [in] */ DctController* host)
    : mRetryCount(0)
    , mPhoneId(0)
    , mIsDefaultDataSwitchRequested(FALSE)
    , mIsOnDemandPsAttachRequested(FALSE)
    , mHost(host)
{}

ECode DctController::SwitchInfo::constructor(
    /* [in] */ Int32 phoneId,
    /* [in] */ INetworkRequest* n,
    /* [in] */ Boolean flag,
    /* [in] */ Boolean isAttachReq)
{
    mPhoneId = phoneId;
    mNetworkRequest = n;
    mIsDefaultDataSwitchRequested = flag;
    mIsOnDemandPsAttachRequested = isAttachReq;
    return NOERROR;
}

ECode DctController::SwitchInfo::constructor(
    /* [in] */ Int32 phoneId,
    /* [in] */ Boolean flag)
{
    mPhoneId = phoneId;
    mNetworkRequest = NULL;
    mIsDefaultDataSwitchRequested = flag;
    return NOERROR;
}

ECode DctController::SwitchInfo::IncRetryCount()
{
    mRetryCount++;
    return NOERROR;
}

ECode DctController::SwitchInfo::IsRetryPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mRetryCount < mHost->MAX_RETRY_FOR_ATTACH);
    return NOERROR;
}

ECode DctController::SwitchInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("SwitchInfo[phoneId = %d, NetworkRequest =%s, isDefaultSwitchRequested = %d, isOnDemandPsAttachRequested = %d, RetryCount = %d",
            mPhoneId, TO_CSTR(mNetworkRequest), mIsDefaultDataSwitchRequested, mIsOnDemandPsAttachRequested, mRetryCount);
    *result = rev;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneId;
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMPhoneId(
    /* [in] */ Int32 phoneId)
{
    mPhoneId = phoneId;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMNetworkRequest(
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNetworkRequest;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMNetworkRequest(
    /* [in] */ INetworkRequest* networkRequest)
{
    mNetworkRequest = networkRequest;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMIsDefaultDataSwitchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsDefaultDataSwitchRequested;
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMIsDefaultDataSwitchRequested(
    /* [in] */ Boolean isDefaultDataSwitchRequested)
{
    mIsDefaultDataSwitchRequested = isDefaultDataSwitchRequested;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMIsOnDemandPsAttachRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsOnDemandPsAttachRequested;
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMIsOnDemandPsAttachRequested(
    /* [in] */ Boolean isOnDemandPsAttachRequested)
{
    mIsOnDemandPsAttachRequested = isOnDemandPsAttachRequested;
    return NOERROR;
}

//=============================================================================
// DctController::DdsSwitchSerializerHandler
//=============================================================================
const String DctController::DdsSwitchSerializerHandler::TAG("DdsSwitchSerializer");

DctController::DdsSwitchSerializerHandler::DdsSwitchSerializerHandler(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::DdsSwitchSerializerHandler::constructor(
    /* [in] */ ILooper* looper)
{
    return Handler::constructor(looper);
}

ECode DctController::DdsSwitchSerializerHandler::UnLock()
{
    Logger::D(TAG, "unLock the DdsSwitchSerializer");
    {
        AutoLock lock(this);
        mHost->mIsDdsSwitchCompleted = TRUE;
        Logger::D(TAG, "unLocked the DdsSwitchSerializer");
        NotifyAll();
    }
    return NOERROR;
}

ECode DctController::DdsSwitchSerializerHandler::IsLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock lock(this);
        Logger::D(TAG, "isLocked = %d", !mHost->mIsDdsSwitchCompleted);
        *result = !mHost->mIsDdsSwitchCompleted;
        return NOERROR;
    }
    return NOERROR;
}

ECode DctController::DdsSwitchSerializerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    if (msgWhat == EVENT_START_DDS_SWITCH) {
        Logger::D(TAG, "EVENT_START_DDS_SWITCH");
        // try {
        ECode ec = NOERROR;
        do {
            AutoLock lock(this);
            while (!mHost->mIsDdsSwitchCompleted) {
                Logger::D(TAG, "DDS switch in progress, wait");
                ec = Wait();
                if (FAILED(ec)) break;
            }
            Logger::D(TAG, "Locked!");
            mHost->mIsDdsSwitchCompleted = FALSE;
        } while(FALSE);
        // } catch (Exception e) {
        if (FAILED(ec)) {
            Logger::D(TAG, "Exception while serializing the DDS switch request , e=%d", ec);
            return NOERROR;
        }
        // }
        AutoPtr<INetworkRequest> n = INetworkRequest::Probe(msgObj);
        Logger::D(TAG, "start the DDS switch for req %s", TO_CSTR(n));
        Int64 subId;
        mHost->mSubController->GetSubIdFromNetworkRequest(n, &subId);
        Int64 currentDds;
        mHost->mSubController->GetCurrentDds(&currentDds);
        if (subId == currentDds) {
            Logger::D(TAG, "No change in DDS, respond back");
            mHost->mIsDdsSwitchCompleted = TRUE;
            ((RegistrantList*) mHost->mNotifyOnDemandDataSwitchInfo.Get())->NotifyRegistrants(
                    new AsyncResult(NULL, n, NULL));
            return NOERROR;
        }
        Int32 phoneId;
        IISub::Probe(mHost->mSubController)->GetPhoneId(subId, &phoneId);
        Int32 prefPhoneId;
        IISub::Probe(mHost->mSubController)->GetPhoneId(
                currentDds, &prefPhoneId);
        AutoPtr<IPhone> phone;
        (*mHost->mPhones)[prefPhoneId]->GetActivePhone((IPhone**)&phone);
        AutoPtr<IDcTrackerBase> dcTracker = ((PhoneBase*) phone.Get())->mDcTracker;
        AutoPtr<SwitchInfo> s = new DctController::SwitchInfo(mHost);
        s->constructor(phoneId, n, FALSE, FALSE);
        AutoPtr<IMessage> dataAllowFalse = CMessage::Obtain(mHost,
                EVENT_SET_DATA_ALLOW_FALSE, TO_IINTERFACE(s));
        dcTracker->SetDataAllowed(FALSE, dataAllowFalse);
        Int32 phoneType;
        phone->GetPhoneType(&phoneType);
        if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
            //cleanup data from apss as there is no detach procedure for CDMA
            dcTracker->CleanUpAllConnections(String("Ondemand DDS switch"));
        }
        (*mHost->mPhones)[prefPhoneId]->RegisterForAllDataDisconnected(
                IHandler::Probe(sDctController), EVENT_ALL_DATA_DISCONNECTED, TO_IINTERFACE(s));
    }
    return NOERROR;
}

//=============================================================================
// DctController
//=============================================================================
CAR_INTERFACE_IMPL(DctController, Handler, IDctController)

const String DctController::LOG__TAG("DctController");
const Boolean DctController::DBG = TRUE;
const Int32 DctController::EVENT_PHONE1_DETACH = 1;
const Int32 DctController::EVENT_PHONE2_DETACH = 2;
const Int32 DctController::EVENT_PHONE3_DETACH = 3;
const Int32 DctController::EVENT_PHONE4_DETACH = 4;
const Int32 DctController::EVENT_PHONE1_RADIO_OFF = 5;
const Int32 DctController::EVENT_PHONE2_RADIO_OFF = 6;
const Int32 DctController::EVENT_PHONE3_RADIO_OFF = 7;
const Int32 DctController::EVENT_PHONE4_RADIO_OFF = 8;
const Int32 DctController::EVENT_START_DDS_SWITCH = 9;
const Int32 DctController::PHONE_NONE = -1;
AutoPtr<IDctController> DctController::sDctController;
const Int32 DctController::EVENT_ALL_DATA_DISCONNECTED = 1;
const Int32 DctController::EVENT_SET_DATA_ALLOW_DONE = 2;
const Int32 DctController::EVENT_DELAYED_RETRY = 3;
const Int32 DctController::EVENT_LEGACY_SET_DATA_SUBSCRIPTION = 4;
const Int32 DctController::EVENT_SET_DATA_ALLOW_FALSE = 5;

ECode DctController::IsActiveSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int64> > activeSubs;
    IISub::Probe(mSubController)->GetActiveSubIdList((ArrayOf<Int64>**)&activeSubs);
    for (Int32 i = 0; i < activeSubs->GetLength(); i++) {
        if (subId == (*activeSubs)[i]) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode DctController::GetDataStateChangedCallback(
    /* [out] */ IDataStateChangedCallback** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDataStateChangedCallback;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DctController::GetInstance(
    /* [out] */ IDctController** result)
{
    VALIDATE_NOT_NULL(result)

    if (sDctController == NULL) {
        Logger::E(LOG__TAG, "DCTrackerController.getInstance can't be called before makeDCTController()");
        return E_RUNTIME_EXCEPTION;
    }
    *result = sDctController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DctController::MakeDctController(
    /* [in] */ ArrayOf<IPhoneProxy*>* phones,
    /* [in] */ ILooper* looper,
    /* [out] */ IDctController** result)
{
    VALIDATE_NOT_NULL(result)

    if (sDctController == NULL) {
        CDctController::New(phones, looper, (IDctController**)&sDctController);
        DdsScheduler::Init();
    }
    *result = sDctController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

DctController::DctController()
    : mPhoneNum(0)
    , mCurrentDataPhone(PHONE_NONE)
    , mRequestedDataPhone(PHONE_NONE)
    , mIsDdsSwitchCompleted(TRUE)
    , MAX_RETRY_FOR_ATTACH(6)
    , ATTACH_RETRY_DELAY(1000 * 10)
{}

ECode DctController::constructor(
    /* [in] */ ArrayOf<IPhoneProxy*>* phones,
    /* [in] */ ILooper* looper)
{
    Handler::constructor(looper);

    CRegistrantList::New((IRegistrantList**)&mNotifyDefaultDataSwitchInfo);
    CRegistrantList::New((IRegistrantList**)&mNotifyOnDemandDataSwitchInfo);
    CRegistrantList::New((IRegistrantList**)&mNotifyOnDemandPsAttach);
    CHashSet::New((IHashSet**)&mApnTypes);
    mRspHander = new SubHandler(this);
    mDataStateChangedCallback = new SubIDataStateChangedCallback(this);
    mSubController = SubscriptionController::GetInstance();

    if (phones == NULL || phones->GetLength() == 0) {
        if (phones == NULL) {
            Loge("DctController(phones): UNEXPECTED phones=null, ignore");
        }
        else {
            Loge("DctController(phones): UNEXPECTED phones.length=0, ignore");
        }
        return NOERROR;
    }
    mPhoneNum = phones->GetLength();
    mServicePowerOffFlag = ArrayOf<Boolean>::Alloc(mPhoneNum);
    mPhones = phones;
    mDcSwitchState = ArrayOf<IDcSwitchState*>::Alloc(mPhoneNum);
    mDcSwitchAsyncChannel = ArrayOf<IDcSwitchAsyncChannel*>::Alloc(mPhoneNum);
    mDcSwitchStateHandler = ArrayOf<IHandler*>::Alloc(mPhoneNum);
    mActivePhone = IPhone::Probe((*mPhones)[0]);
    for (Int32 i = 0; i < mPhoneNum; ++i) {
        Int32 phoneId = i;
        (*mServicePowerOffFlag)[i] = TRUE;
        CDcSwitchState::New(IPhone::Probe((*mPhones)[i]), String("DcSwitchState-%d") + StringUtils::ToString(phoneId), phoneId, (IDcSwitchState**)&(*mDcSwitchState)[i]);
        IStateMachine::Probe((*mDcSwitchState)[i])->Start();
        CDcSwitchAsyncChannel::New((*mDcSwitchState)[i], phoneId, (IDcSwitchAsyncChannel**)&(*mDcSwitchAsyncChannel)[i]);
        CHandler::New((IHandler**)&(*mDcSwitchStateHandler)[i]);
        AutoPtr<IHandler> handler;
        IStateMachine::Probe((*mDcSwitchState)[i])->GetHandler((IHandler**)&handler);
        AutoPtr<IContext> context;
        IPhone::Probe((*mPhones)[i])->GetContext((IContext**)&context);
        Int32 status;
        IAsyncChannel::Probe((*mDcSwitchAsyncChannel)[i])->FullyConnectSync(context,
            (*mDcSwitchStateHandler)[i], handler, &status);
        if (status == IAsyncChannel::STATUS_SUCCESSFUL) {
            Logd("DctController(phones): Connect success: %d", i);
        } else {
            Loge("DctController(phones): Could not connect to %d", i);
        }
        (*mDcSwitchState)[i]->RegisterForIdle(mRspHander, EVENT_PHONE1_DETACH + i, NULL);
        // Register for radio state change
        AutoPtr<IPhone> phone;
        IPhoneProxy::Probe((*mPhones)[i])->GetActivePhone((IPhone**)&phone);
        AutoPtr<IPhoneBase> phoneBase = IPhoneBase::Probe(phone);
        ((PhoneBase*) phoneBase.Get())->mCi->RegisterForOffOrNotAvailable(mRspHander, EVENT_PHONE1_RADIO_OFF + i, NULL);
    }
    mActivePhone->GetContext((IContext**)&mContext);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_DATA_CONNECTION_FAILED);
    filter->AddAction(ITelephonyIntents::ACTION_SERVICE_STATE_CHANGED);
    mDataStateReceiver = new DataStateReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mDataStateReceiver, filter, (IIntent**)&intent);
    AutoPtr<IHandlerThread> t;
    CHandlerThread::New(String("DdsSwitchSerializer"), (IHandlerThread**)&t);
    IThread::Probe(t)->Start();
    AutoPtr<ILooper> tLooper;
    t->GetLooper((ILooper**)&tLooper);
    mDdsSwitchSerializer= new DdsSwitchSerializerHandler(this);
    mDdsSwitchSerializer->constructor(tLooper);

    return NOERROR;
}

ECode DctController::GetIccCardState(
    /* [in] */ Int32 phoneId,
    /* [out] */ IccCardConstantsState* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIccCard> iccCard;
    IPhone::Probe((*mPhones)[phoneId])->GetIccCard((IIccCard**)&iccCard);
    return iccCard->GetState(result);
}

ECode DctController::EnableApnType(
    /* [in] */ Int64 subId,
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    Boolean isValidphoneId;
    IsValidphoneId(phoneId, &isValidphoneId);
    if (phoneId == PHONE_NONE || !isValidphoneId) {
        Logw("enableApnType(): with PHONE_NONE or Invalid PHONE ID");
        *result = IPhoneConstants::APN_REQUEST_FAILED;
        return NOERROR;
    }
    Logd("enableApnType():type=%s,phoneId=%d,powerOff=%d",
            type.string(), phoneId, (*mServicePowerOffFlag)[phoneId]);
    if (!IPhoneConstants::APN_TYPE_DEFAULT.Equals(type)) {
        for (Int32 peerphoneId =0; peerphoneId < mPhoneNum; peerphoneId++) {
            // check peer Phone has non default APN activated as receiving non default APN request.
            if (phoneId == peerphoneId) {
                continue;
            }
            AutoPtr<ArrayOf<String> > activeApnTypes;
            IPhone::Probe((*mPhones)[peerphoneId])->GetActiveApnTypes((ArrayOf<String>**)&activeApnTypes);
            if (activeApnTypes != NULL && activeApnTypes->GetLength() != 0) {
                for (Int32 i=0; i<activeApnTypes->GetLength(); i++) {
                    PhoneConstantsDataState pcds;
                    IPhone::Probe((*mPhones)[peerphoneId])->GetDataConnectionState((*activeApnTypes)[i], &pcds);
                    if (!IPhoneConstants::APN_TYPE_DEFAULT.Equals((*activeApnTypes)[i]) &&
                            pcds != PhoneConstantsDataState_DISCONNECTED) {
                        Logd("enableApnType:Peer Phone still have non-default active APN type:"
                                "activeApnTypes[%d]=", i, (*activeApnTypes)[i].string());
                        *result = IPhoneConstants::APN_REQUEST_FAILED;
                        return NOERROR;
                    }
                }
            }
        }
    }
    Logd("enableApnType(): CurrentDataPhone=%d"
            ", RequestedDataPhone=%d", mCurrentDataPhone, mRequestedDataPhone);
    Boolean isIdleOrDeactingSync = FALSE;
    if (phoneId == mCurrentDataPhone &&
        ((*mDcSwitchAsyncChannel)[mCurrentDataPhone]->IsIdleOrDeactingSync(&isIdleOrDeactingSync), !isIdleOrDeactingSync)) {
        mRequestedDataPhone = PHONE_NONE;
        Logd("enableApnType(): mRequestedDataPhone equals request PHONE ID.");
        return (*mDcSwitchAsyncChannel)[phoneId]->ConnectSync(type, result);
    }
    else {
        // Only can switch data when mCurrentDataPhone is PHONE_NONE,
        // it is set to PHONE_NONE only as receiving EVENT_PHONEX_DETACH
        if (mCurrentDataPhone == PHONE_NONE) {
            mCurrentDataPhone = phoneId;
            mRequestedDataPhone = PHONE_NONE;
            Logd("enableApnType(): current PHONE is NONE or IDLE, mCurrentDataPhone=%d",
                    mCurrentDataPhone);
            return (*mDcSwitchAsyncChannel)[phoneId]->ConnectSync(type, result);
        } else {
            Logd("enableApnType(): current PHONE:%d is active.", mCurrentDataPhone);
            if (phoneId != mRequestedDataPhone) {
                mApnTypes->Clear();
            }
            mApnTypes->Add(StringUtils::ParseCharSequence(type));
            mRequestedDataPhone = phoneId;
            (*mDcSwitchState)[mCurrentDataPhone]->CleanupAllConnection();
        }
    }
    *result = IPhoneConstants::APN_REQUEST_STARTED;
    return NOERROR;
}

ECode DctController::DisableApnType(
    /* [in] */ Int64 subId,
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    Boolean isValidphoneId;
    IsValidphoneId(phoneId, &isValidphoneId);
    if (phoneId == PHONE_NONE || !isValidphoneId) {
        Logw("disableApnType(): with PHONE_NONE or Invalid PHONE ID");
        *result = IPhoneConstants::APN_REQUEST_FAILED;
        return NOERROR;
    }
    Logd("disableApnType():type=%s,phoneId=%d,powerOff=%d",
            type.string(), phoneId, (*mServicePowerOffFlag)[phoneId]);
    return (*mDcSwitchAsyncChannel)[phoneId]->DisconnectSync(type, result);
}

ECode DctController::IsDataConnectivityPossible(
    /* [in] */ const String& type,
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isValidphoneId;
    IsValidphoneId(phoneId, &isValidphoneId);
    if (phoneId == PHONE_NONE || !isValidphoneId) {
        Logw("isDataConnectivityPossible(): with PHONE_NONE or Invalid PHONE ID");
        *result = FALSE;
        return NOERROR;
    } else {
        return IPhone::Probe((*mPhones)[phoneId])->IsDataConnectivityPossible(type, result);
    }
    return NOERROR;
}

ECode DctController::IsIdleOrDeacting(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isIdleOrDeactingSync;
    (*mDcSwitchAsyncChannel)[phoneId]->IsIdleOrDeactingSync(&isIdleOrDeactingSync);
    if (isIdleOrDeactingSync) {
        *result = TRUE;
        return NOERROR;
    } else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DctController::IsValidphoneId(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = phoneId >= 0 && phoneId < mPhoneNum;
    return NOERROR;
}

ECode DctController::IsValidApnType(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (apnType.Equals(IPhoneConstants::APN_TYPE_DEFAULT)
            || apnType.Equals(IPhoneConstants::APN_TYPE_MMS)
            || apnType.Equals(IPhoneConstants::APN_TYPE_SUPL)
            || apnType.Equals(IPhoneConstants::APN_TYPE_DUN)
            || apnType.Equals(IPhoneConstants::APN_TYPE_HIPRI)
            || apnType.Equals(IPhoneConstants::APN_TYPE_FOTA)
            || apnType.Equals(IPhoneConstants::APN_TYPE_IMS)
            || apnType.Equals(IPhoneConstants::APN_TYPE_CBS))
    {
        *result = TRUE;
        return NOERROR;
    } else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode DctController::GetDataConnectionFromSetting(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 subId;
    IISub::Probe(mSubController)->GetDefaultDataSubId(&subId);
    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    *result = phoneId;
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DctController::Logv(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::V(LOG__TAG, "[DctController] %s", msgBuf);
}

ECode DctController::Logd(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(LOG__TAG, "[DctController] %s", msgBuf);
}

ECode DctController::Logw(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::W(LOG__TAG, "[DctController] %s", msgBuf);
}

ECode DctController::Loge(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::E(LOG__TAG, "[DctController] %s", msgBuf);
}

ECode DctController::DoDetach(
    /* [in] */ Int32 phoneId)
{
    AutoPtr<IPhone> phone;
    (*mPhones)[phoneId]->GetActivePhone((IPhone**)&phone);
    AutoPtr<IDcTrackerBase> dcTracker = ((PhoneBase*) phone.Get())->mDcTracker;
    dcTracker->SetDataAllowed(FALSE, NULL);
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
        //cleanup data from apss as there is no detach procedure for CDMA
        dcTracker->CleanUpAllConnections(String("DDS switch"));
    }
    return NOERROR;
}

ECode DctController::SetDefaultDataSubId(
    /* [in] */ Int64 reqSubId)
{
    Int32 reqPhoneId;
    IISub::Probe(mSubController)->GetPhoneId(reqSubId, &reqPhoneId);
    Int64 currentDds;
    mSubController->GetCurrentDds(&currentDds);
    Int64 defaultDds;
    IISub::Probe(mSubController)->GetDefaultDataSubId(&defaultDds);
    AutoPtr<SwitchInfo> s = new SwitchInfo(this);
    s->constructor(reqPhoneId, TRUE);
    Int32 currentDdsPhoneId;
    IISub::Probe(mSubController)->GetPhoneId(currentDds, &currentDdsPhoneId);
    if (currentDdsPhoneId < 0 || currentDdsPhoneId >= mPhoneNum) {
        // If Current dds subId is invalid set the received subId as current DDS
        // This generally happens when device power-up first time.
        Logd(" setDefaultDataSubId,  reqSubId = %d currentDdsPhoneId  %d",
                reqSubId, currentDdsPhoneId);
        mSubController->SetDataSubId(reqSubId);
        defaultDds = reqSubId;
        IISub::Probe(mSubController)->GetPhoneId(defaultDds, &currentDdsPhoneId);
    }
    Logger::D(LOG__TAG, "setDefaultDataSubId reqSubId :%d reqPhoneId = %d",
            reqSubId, reqPhoneId);
    // Avoid sending data allow false and true on same sub .
    if ((reqSubId != defaultDds) && (reqPhoneId != currentDdsPhoneId)) {
        DoDetach(currentDdsPhoneId);
    } else {
        Logd("setDefaultDataSubId for default DDS, skip PS detach on DDS subs");
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_LEGACY_SET_DATA_SUBSCRIPTION,
                    TO_IINTERFACE(new AsyncResult(TO_IINTERFACE(s), NULL, NULL)), (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
        return NOERROR;
    }
    (*mPhones)[currentDdsPhoneId]->RegisterForAllDataDisconnected(
            this, EVENT_ALL_DATA_DISCONNECTED, TO_IINTERFACE(s));
    return NOERROR;
}

ECode DctController::InformDefaultDdsToPropServ(
    /* [in] */ Int32 defDdsPhoneId)
{
    if (mDdsSwitchPropService != NULL) {
        Logd("Inform OemHookDDS service of current DDS = %d", defDdsPhoneId);
        AutoPtr<IMessage> msg;
        mDdsSwitchPropService->SendMessageSynchronously(1, defDdsPhoneId,
                mPhoneNum, (IMessage**)&msg);
        Logd("OemHookDDS service finished");
    } else {
        Logd("OemHookDds service not ready yet");
    }
    return NOERROR;
}

ECode DctController::DoPsAttach(
    /* [in] */ INetworkRequest* n)
{
    Logger::D(LOG__TAG, "doPsAttach for :%s", TO_CSTR(n));
    Int64 subId;
    mSubController->GetSubIdFromNetworkRequest(n, &subId);
    Int32 phoneId;
    IISub::Probe(mSubController)->GetPhoneId(subId, &phoneId);
    AutoPtr<IPhone> phone;
    (*mPhones)[phoneId]->GetActivePhone((IPhone**)&phone);
    AutoPtr<IDcTrackerBase> dcTracker = ((PhoneBase*) phone.Get())->mDcTracker;
    //request only PS ATTACH on requested subscription.
    //No DdsSerealization lock required.
    AutoPtr<SwitchInfo> s = new SwitchInfo(this);
    s->constructor(phoneId, n, FALSE, TRUE);
    AutoPtr<IMessage> psAttachDone = CMessage::Obtain(this,
            EVENT_SET_DATA_ALLOW_DONE, TO_IINTERFACE(s));
    Int32 defDdsPhoneId;
    GetDataConnectionFromSetting(&defDdsPhoneId);
    InformDefaultDdsToPropServ(defDdsPhoneId);
    dcTracker->SetDataAllowed(TRUE, psAttachDone);
    return NOERROR;
}

ECode DctController::DoPsDetach()
{
    Int64 currentDds;
    mSubController->GetCurrentDds(&currentDds);
    Int64 defaultDds;
    IISub::Probe(mSubController)->GetDefaultDataSubId(&defaultDds);
    if (currentDds == defaultDds) {
        Logger::D(LOG__TAG, "PS DETACH on DDS sub is not allowed.");
        return NOERROR;
    }
    Logger::D(LOG__TAG, "doPsDetach for sub:%lld", currentDds);
    Int32 phoneId;
    IISub::Probe(mSubController)->GetPhoneId(currentDds, &phoneId);
    AutoPtr<IPhone> phone;
    (*mPhones)[phoneId]->GetActivePhone((IPhone**)&phone);;
    AutoPtr<IDcTrackerBase> dcTracker = ((PhoneBase*) phone.Get())->mDcTracker;
    dcTracker->SetDataAllowed(FALSE, NULL);
    return NOERROR;
}

ECode DctController::SetOnDemandDataSubId(
    /* [in] */ INetworkRequest* n)
{
    Logger::D(LOG__TAG, "setDataAllowed for :%s", TO_CSTR(n));
    AutoPtr<IMessage> msg;
    mDdsSwitchSerializer->ObtainMessage(EVENT_START_DDS_SWITCH, n, (IMessage**)&msg);
    Boolean b;
    mDdsSwitchSerializer->SendMessage(msg, &b);
    return NOERROR;
}

ECode DctController::RegisterForDefaultDataSwitchInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    {
        AutoLock lock(mNotifyDefaultDataSwitchInfo);
        ((RegistrantList*) mNotifyDefaultDataSwitchInfo.Get())->Add(r);
    }
    return NOERROR;
}

ECode DctController::RegisterForOnDemandDataSwitchInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    {
        AutoLock lock(mNotifyOnDemandDataSwitchInfo);
        ((RegistrantList*) mNotifyOnDemandDataSwitchInfo.Get())->Add(r);
    }
    return NOERROR;
}

ECode DctController::RegisterForOnDemandPsAttach(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    {
        AutoLock lock(mNotifyOnDemandPsAttach);
        ((RegistrantList*) mNotifyOnDemandPsAttach.Get())->Add(r);
    }
    return NOERROR;
}

ECode DctController::RegisterDdsSwitchPropService(
    /* [in] */ IMessenger* messenger)
{
    Logd("Got messenger from DDS switch service, messenger = %s", TO_CSTR(messenger));
    AutoPtr<IAsyncChannel> ac;
    CAsyncChannel::New((IAsyncChannel**)&ac);
    ac->Connect(mContext, IHandler::Probe(sDctController), messenger);
    return NOERROR;
}

ECode DctController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Boolean isLegacySetDds = FALSE;
    Logger::D(LOG__TAG, "handleMessage msg=%s", TO_CSTR(msg));
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    switch (msgWhat) {
        case EVENT_LEGACY_SET_DATA_SUBSCRIPTION:
            isLegacySetDds = TRUE;
            //intentional fall through, no break.
        case EVENT_ALL_DATA_DISCONNECTED: {
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
            AutoPtr<SwitchInfo> s = (SwitchInfo*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
            AutoPtr<IInteger32> phoneId;
            CInteger32::New(s->mPhoneId, (IInteger32**)&phoneId);
            Logger::D(LOG__TAG, "EVENT_ALL_DATA_DISCONNECTED switchInfo :%s isLegacySetDds = %d",
                    TO_CSTR(s), isLegacySetDds);
            // In this case prefPhoneId points to the newDds we are trying to
            // set, hence we do not need to call unregister for data disconnected
            if (!isLegacySetDds) {
                Int64 currentDds;
                mSubController->GetCurrentDds(&currentDds);
                Int32 prefPhoneId;
                IISub::Probe(mSubController)->GetPhoneId(currentDds, &prefPhoneId);
                (*mPhones)[prefPhoneId]->UnregisterForAllDataDisconnected(this);
            }
            AutoPtr<IMessage> allowedDataDone = CMessage::Obtain(this,
                    EVENT_SET_DATA_ALLOW_DONE, TO_IINTERFACE(s));
            Int32 i32;
            phoneId->GetValue(&i32);
            AutoPtr<IPhone> phone;
            (*mPhones)[i32]->GetActivePhone((IPhone**)&phone);
            InformDefaultDdsToPropServ(i32);
            AutoPtr<IDcTrackerBase> dcTracker = ((PhoneBase*) phone.Get())->mDcTracker;
            dcTracker->SetDataAllowed(TRUE, allowedDataDone);
            break;
        }
        case EVENT_DELAYED_RETRY: {
            Logger::D(LOG__TAG, "EVENT_DELAYED_RETRY");
            AutoPtr<SomeArgs> args = (SomeArgs*) IObject::Probe(msgObj);
            // try {
            ECode ec;
            do {
                AutoPtr<SwitchInfo> s = (SwitchInfo*) IObject::Probe(args->mArg1);
                Boolean psAttach;
                IBoolean::Probe(args->mArg2)->GetValue(&psAttach);
                Logger::D(LOG__TAG, " Retry, switchInfo = %s", TO_CSTR(s));
                AutoPtr<IInteger32> phoneId;
                CInteger32::New(s->mPhoneId, (IInteger32**)&phoneId);
                AutoPtr<ArrayOf<Int64> > subId;
                ec = IISub::Probe(mSubController)->GetSubId(s->mPhoneId, (ArrayOf<Int64>**)&subId);
                if (FAILED(ec)) break;
                AutoPtr<IPhone> phone;
                ec = (*mPhones)[s->mPhoneId]->GetActivePhone((IPhone**)&phone);;
                if (FAILED(ec)) break;
                AutoPtr<IDcTrackerBase> dcTracker = ((PhoneBase*) phone.Get())->mDcTracker;
                if (psAttach) {
                    AutoPtr<IMessage> psAttachDone = CMessage::Obtain(this, EVENT_SET_DATA_ALLOW_DONE, TO_IINTERFACE(s));
                    ec = dcTracker->SetDataAllowed(TRUE, psAttachDone);
                    if (FAILED(ec)) break;
                } else {
                    AutoPtr<IMessage> psDetachDone = CMessage::Obtain(this,
                            EVENT_SET_DATA_ALLOW_FALSE, TO_IINTERFACE(s));
                    ec = dcTracker->SetDataAllowed(FALSE, psDetachDone);
                }
            } while(FALSE);
            // } finally {
            args->Recycle();
            if (FAILED(ec)) return ec;
            // }
            break;
        }
        case EVENT_SET_DATA_ALLOW_DONE: {
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
            AutoPtr<SwitchInfo> s = (SwitchInfo*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
            AutoPtr<IThrowable> errorEx;
            AutoPtr<IInteger32> phoneId;
            CInteger32::New(s->mPhoneId, (IInteger32**)&phoneId);
            AutoPtr<ArrayOf<Int64> > subId;
            IISub::Probe(mSubController)->GetSubId(s->mPhoneId, (ArrayOf<Int64>**)&subId);
            Logger::D(LOG__TAG, "EVENT_SET_DATA_ALLOWED_DONE  phoneId :%lld, switchInfo = %s",
                    (*subId)[0], TO_CSTR(s));
            if (((AsyncResult*) ar.Get())->mException != NULL) {
                Logger::D(LOG__TAG, "Failed, switchInfo = %s attempt delayed retry", TO_CSTR(s));
                s->IncRetryCount();
                Boolean isRetryPossible;
                s->IsRetryPossible(&isRetryPossible);
                if (isRetryPossible) {
                    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
                    args->mArg1 = TO_IINTERFACE(s);
                    AutoPtr<IBoolean> ib;
                    CBoolean::New(TRUE, (IBoolean**)&ib);
                    args->mArg2 = ib;
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_DELAYED_RETRY, TO_IINTERFACE(args), (IMessage**)&msg);
                    Boolean b;
                    SendMessageDelayed(msg, ATTACH_RETRY_DELAY, &b);
                    return NOERROR;
                } else {
                    Logger::D(LOG__TAG, "Already did max retries, notify failure");
                    CThrowable::New(String("PS ATTACH failed"), (IThrowable**)&errorEx);
               }
            } else {
                Logger::D(LOG__TAG, "PS ATTACH success = %s", TO_CSTR(s));
            }
            mDdsSwitchSerializer->UnLock();
            if (s->mIsDefaultDataSwitchRequested) {
                AutoPtr<IInteger64> i64;
                CInteger64::New((*subId)[0], (IInteger64**)&i64);
                ((RegistrantList*) mNotifyDefaultDataSwitchInfo.Get())->NotifyRegistrants(
                        new AsyncResult(NULL, i64, errorEx));
            } else if (s->mIsOnDemandPsAttachRequested) {
                ((RegistrantList*) mNotifyOnDemandPsAttach.Get())->NotifyRegistrants(
                        new AsyncResult(NULL, s->mNetworkRequest, errorEx));
            } else {
                ((RegistrantList*) mNotifyOnDemandDataSwitchInfo.Get())->NotifyRegistrants(
                        new AsyncResult(NULL, s->mNetworkRequest, errorEx));
            }
            break;
        }
        case EVENT_SET_DATA_ALLOW_FALSE: {
            AutoPtr<IAsyncResult> ar = IAsyncResult::Probe(msgObj);
            AutoPtr<SwitchInfo> s = (SwitchInfo*) IObject::Probe(((AsyncResult*) ar.Get())->mUserObj);
            AutoPtr<IThrowable> errorEx;
            AutoPtr<IInteger32> phoneId;
            CInteger32::New(s->mPhoneId, (IInteger32**)&phoneId);
            AutoPtr<ArrayOf<Int64> > subId;
            IISub::Probe(mSubController)->GetSubId(s->mPhoneId, (ArrayOf<Int64>**)&subId);
            Logger::D(LOG__TAG, "EVENT_SET_DATA_FALSE  phoneId :%lld, switchInfo = %s",
                    (*subId)[0], TO_CSTR(s));
            if (((AsyncResult*) ar.Get())->mException != NULL) {
                Logger::D(LOG__TAG, "Failed, switchInfo = %s attempt delayed retry", TO_CSTR(s));
                s->IncRetryCount();
                Boolean isRetryPossible;
                s->IsRetryPossible(&isRetryPossible);
                if (isRetryPossible) {
                    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
                    args->mArg1 = TO_IINTERFACE(s);
                    AutoPtr<IBoolean> ib;
                    CBoolean::New(FALSE, (IBoolean**)&ib);
                    args->mArg2 = ib;
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_DELAYED_RETRY, TO_IINTERFACE(args), (IMessage**)&msg);
                    Boolean b;
                    SendMessageDelayed(msg, ATTACH_RETRY_DELAY, &b);
                    return NOERROR;
                } else {
                    Logger::D(LOG__TAG, "Already did max retries, notify failure");
                    CThrowable::New(String("PS DETACH failed"), (IThrowable**)&errorEx);
                    ((RegistrantList*) mNotifyOnDemandDataSwitchInfo.Get())->NotifyRegistrants(
                            new AsyncResult(NULL, s->mNetworkRequest, errorEx));
               }
            } else {
                Logger::D(LOG__TAG, "PS DETACH success = %s", TO_CSTR(s));
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            Int32 msgArg1;
            msg->GetArg1(&msgArg1);
            if (msgArg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                Logd("HALF_CONNECTED: Connection successful with DDS switch"
                        " service");
                mDdsSwitchPropService = IAsyncChannel::Probe(msgObj);
            } else {
                Logd("HALF_CONNECTED: Connection failed with"
                        " DDS switch service, err = %d", msgArg1);
            }
                break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            Logd("Connection disconnected with DDS switch service");
            mDdsSwitchPropService = NULL;
            break;
        }
    }
    return NOERROR;
}

ECode DctController::IsDctControllerLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isLocked;
    mDdsSwitchSerializer->IsLocked(&isLocked);
    *result = isLocked;
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
