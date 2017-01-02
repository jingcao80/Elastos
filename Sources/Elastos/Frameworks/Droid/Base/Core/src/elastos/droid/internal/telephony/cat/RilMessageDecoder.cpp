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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/RilMessageDecoder.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"
#include "elastos/droid/internal/telephony/cat/CatService.h"
#include "elastos/droid/internal/telephony/uicc/CIccUtils.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::CIccUtils;

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                    RilMessageDecoder::StateStart
//=====================================================================
RilMessageDecoder::StateStart::StateStart(
    /* [in] */ RilMessageDecoder* host)
    : mHost(host)
{
}

String RilMessageDecoder::StateStart::GetName()
{
    return String("RilMessageDecoder::StateStart");
}

ECode RilMessageDecoder::StateStart::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == CMD_START) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        if (mHost->DecodeMessageParams((RilMessage*)(IObject*)obj.Get())) {
            mHost->TransitionTo(mHost->mStateCmdParamsReady);
        }
    }
    else {
        CatLog::D(IState::Probe(this), String("StateStart unexpected expecting START=") +
                StringUtils::ToString(CMD_START) + String(" got ") + StringUtils::ToString(what));
    }
    *result = TRUE;
    return NOERROR;
}

//=====================================================================
//                RilMessageDecoder::StateCmdParamsReady
//=====================================================================
RilMessageDecoder::StateCmdParamsReady::StateCmdParamsReady(
    /* [in] */ RilMessageDecoder* host)
    : mHost(host)
{
}

String RilMessageDecoder::StateCmdParamsReady::GetName()
{
    return String("RilMessageDecoder::StateCmdParamsReady");
}

ECode RilMessageDecoder::StateCmdParamsReady::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg1 = 0;
    msg->GetArg1(&arg1);
    if (what == CMD_PARAMS_READY) {
        mHost->mCurrentRilMessage->mResCode = arg1;
        mHost->mCurrentRilMessage->mData = obj;
        mHost->SendCmdForExecution(mHost->mCurrentRilMessage);
        mHost->TransitionTo(mHost->mStateStart);
    }
    else {
        CatLog::D(IState::Probe(this), String("StateCmdParamsReady expecting CMD_PARAMS_READY=")
                + StringUtils::ToString(CMD_PARAMS_READY) + String(" got ") + StringUtils::ToString(what));
        mHost->DeferMessage(msg);
    }
    *result = TRUE;
    return NOERROR;
}

//=====================================================================
//                          RilMessageDecoder
//=====================================================================
const Int32 RilMessageDecoder::CMD_START;
const Int32 RilMessageDecoder::CMD_PARAMS_READY;
Int32 RilMessageDecoder::mSimCount = 0;
AutoPtr<ArrayOf<RilMessageDecoder*> > RilMessageDecoder::mInstance = NULL;

// synchronized
AutoPtr<RilMessageDecoder> RilMessageDecoder::GetInstance(
    /* [in] */ IHandler* caller,
    /* [in] */ IIccFileHandler* fh,
    /* [in] */ Int32 slotId)
{
    if (NULL == mInstance) {
        AutoPtr<ITelephonyManagerHelper> hlp;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
        AutoPtr<ITelephonyManager> tm;
        hlp->GetDefault((ITelephonyManager**)&tm);
        tm->GetSimCount(&mSimCount);
        mInstance = ArrayOf<RilMessageDecoder*>::Alloc(mSimCount);
        for (Int32 i = 0; i < mSimCount; i++) {
            mInstance->Set(i, NULL);
        }
    }

    if (slotId != ISubscriptionManager::INVALID_SLOT_ID && slotId < mSimCount) {
        if (NULL == (*mInstance)[slotId]) {
            mInstance->Set(slotId, new RilMessageDecoder(caller, fh));
        }
    }
    else {
        CatLog::D(String("RilMessageDecoder"), String("invaild slot id: ") + StringUtils::ToString(slotId));
        return NULL;
    }

    return (*mInstance)[slotId];
}

ECode RilMessageDecoder::SendStartDecodingMessageParams(
    /* [in] */ RilMessage* rilMsg)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(CMD_START, (IMessage**)&msg);
    msg->SetObj((IInterface*)(IObject*)rilMsg);
    SendMessage(msg);
    return NOERROR;
}

ECode RilMessageDecoder::SendMsgParamsDecoded(
    /* [in] */ ResultCode resCode,
    /* [in] */ CommandParams* cmdParams)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(CMD_PARAMS_READY, (IMessage**)&msg);
    msg->SetArg1(resCode);
    msg->SetObj((IInterface*)(IObject*)cmdParams);
    SendMessage(msg);
    return NOERROR;
}

ECode RilMessageDecoder::Dispose(
    /* [in] */ Int32 slotId)
{
    QuitNow();
    mStateStart = NULL;
    mStateCmdParamsReady = NULL;
    mCmdParamsFactory->Dispose();
    mCmdParamsFactory = NULL;
    mCurrentRilMessage = NULL;
    mCaller = NULL;
    (*mInstance)[slotId] = NULL;
    return NOERROR;
}

RilMessageDecoder::RilMessageDecoder(
    /* [in] */ IHandler* caller,
    /* [in] */ IIccFileHandler* fh)
{
    StateMachine::constructor(String("RilMessageDecoder"));
    mStateStart = new  StateStart(this);
    mStateCmdParamsReady = new StateCmdParamsReady(this);
    AddState(mStateStart);
    AddState(mStateCmdParamsReady);
    SetInitialState(mStateStart);

    mCaller = caller;
    mCmdParamsFactory = CommandParamsFactory::GetInstance(this, fh);
}

RilMessageDecoder::RilMessageDecoder()
{
    StateMachine::constructor(String("RilMessageDecoder"));
}

void RilMessageDecoder::SendCmdForExecution(
    /* [in] */ RilMessage* rilMsg)
{
    AutoPtr<RilMessage> p = new RilMessage(rilMsg);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(CatService::MSG_ID_RIL_MSG_DECODED,
            (IInterface*)(IObject*)p.Get(), (IMessage**)&msg);
    msg->SendToTarget();
}

Boolean RilMessageDecoder::DecodeMessageParams(
    /* [in] */ RilMessage* rilMsg)
{
    Boolean decodingStarted;

    mCurrentRilMessage = rilMsg;
    switch(rilMsg->mId) {
    case CatService::MSG_ID_SESSION_END:
    case CatService::MSG_ID_CALL_SETUP: {
        mCurrentRilMessage->mResCode = ResultCode_OK;
        SendCmdForExecution(mCurrentRilMessage);
        decodingStarted = FALSE;
        break;
    }
    case CatService::MSG_ID_PROACTIVE_COMMAND:
    case CatService::MSG_ID_EVENT_NOTIFY:
    case CatService::MSG_ID_REFRESH: {
        AutoPtr<ArrayOf<Byte> > rawData;
        // try {
            String str;
            ICharSequence::Probe(rilMsg->mData)->ToString(&str);
            AutoPtr<Elastos::Droid::Internal::Telephony::Uicc::IIccUtils> iccu;
            CIccUtils::AcquireSingleton((Elastos::Droid::Internal::Telephony::Uicc::IIccUtils**)&iccu);
            iccu->HexStringToBytes(str, (ArrayOf<Byte>**)&rawData);
        // } catch (Exception e) {
        //     // zombie messages are dropped
        //     CatLog.d(this, "decodeMessageParams dropping zombie messages");
        //     decodingStarted = FALSE;
        //     break;
        // }
        // try {
            // Start asynch parsing of the command parameters.
            mCmdParamsFactory->Make(BerTlv::Decode(rawData));
            decodingStarted = TRUE;
        // } catch (ResultException e) {
        //     // send to Service for proper RIL communication.
        //     CatLog.d(this, "decodeMessageParams: caught ResultException e=" + e);
        //     mCurrentRilMessage.mResCode = e.result();
        //     sendCmdForExecution(mCurrentRilMessage);
        //     decodingStarted = FALSE;
        // }
        break;
    }
    default:
        decodingStarted = FALSE;
        break;
    }
    return decodingStarted;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
