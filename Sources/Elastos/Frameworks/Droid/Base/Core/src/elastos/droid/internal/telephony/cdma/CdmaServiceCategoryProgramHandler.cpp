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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaServiceCategoryProgramHandler.h"
#include "elastos/droid/internal/telephony/cdma/CdmaServiceCategoryProgramHandler.h"
#include "elastos/droid/internal/telephony/cdma/CDMASmsMessage.h"
#include "elastos/droid/internal/telephony/cdma/sms/CBearerData.h"
#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsAddress.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CBearerData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::CdmaSmsAddress;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::IBearerData;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ISmsEnvelope;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Manifest;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//      CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver
//=====================================================================
CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ CdmaServiceCategoryProgramHandler* owner)
    : mOwner(owner)
{
}

ECode CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    SendScpResults();
    if (DBG) mOwner->Log(String("mScpResultsReceiver finished"));
    mOwner->SendMessage(EVENT_BROADCAST_COMPLETE);
    return NOERROR;
}

void CdmaServiceCategoryProgramHandler::InnerBroadcastReceiver::SendScpResults()
{
    Int32 resultCode;
    GetResultCode(&resultCode);
    if ((resultCode != IActivity::RESULT_OK) &&
        (resultCode != ITelephonySmsIntents::RESULT_SMS_HANDLED)) {
        mOwner->Loge(String("SCP results error: result code = ") + StringUtils::ToString(resultCode));
        return;
    }
    AutoPtr<IBundle> extras;
    GetResultExtras(FALSE, (IBundle**)&extras);
    if (extras == NULL) {
        mOwner->Loge(String("SCP results error: missing extras"));
        return;
    }
    String sender;
    extras->GetString(String("sender"), &sender);
    if (sender == NULL) {
        mOwner->Loge(String("SCP results error: missing sender extra."));
        return;
    }
    AutoPtr<IArrayList> results;
    extras->GetParcelableArrayList(String("results"), (IArrayList**)&results);
    if (results == NULL) {
        mOwner->Loge(String("SCP results error: missing results extra."));
        return;
    }

    AutoPtr<CBearerData> bData;
    CBearerData::NewByFriend((CBearerData**)&bData);
    bData->messageType = IBearerData::MESSAGE_TYPE_SUBMIT;
    bData->messageId = CDMASmsMessage::GetNextMessageId();
    bData->serviceCategoryProgramResults = results;
    AutoPtr<ArrayOf<Byte> > encodedBearerData = CBearerData::Encode(IBearerData::Probe(bData));

    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New(100, (IByteArrayOutputStream**)&baos);
    AutoPtr<IDataOutputStream> dos;
    CDataOutputStream::New(IOutputStream::Probe(baos), (IDataOutputStream**)&dos);
    // try {
    dos->WriteInt32(ISmsEnvelope::TELESERVICE_SCPT);
    dos->WriteInt32(0); //servicePresent
    dos->WriteInt32(0); //serviceCategory
    String str;
    PhoneNumberUtils::CdmaCheckAndProcessPlusCodeForSms(sender, &str);
    AutoPtr<CdmaSmsAddress> destAddr = CdmaSmsAddress::Parse(str);
    dos->WriteInt32(destAddr->digitMode);
    dos->WriteInt32(destAddr->numberMode);
    dos->WriteInt32(destAddr->ton); // number_type
    dos->WriteInt32(destAddr->numberPlan);
    dos->WriteInt32(destAddr->numberOfDigits);
    dos->WriteBytes(destAddr->origBytes); // digits
    // Subaddress is not supported.
    dos->WriteInt32(0); //subaddressType
    dos->WriteInt32(0); //subaddr_odd
    dos->WriteInt32(0); //subaddr_nbr_of_digits
    dos->WriteInt32(encodedBearerData->GetLength());
    dos->WriteBytes(encodedBearerData);
    // Ignore the RIL response. TODO: implement retry if SMS send fails.
    AutoPtr<ArrayOf<Byte> > array;
    baos->ToByteArray((ArrayOf<Byte>**)&array);
    mOwner->mCi->SendCdmaSms(array, NULL);
    // } catch (IOException e) {
    //     mOwner->Loge("exception creating SCP results PDU", e);
    // } finally {
        // try {
    ICloseable::Probe(dos)->Close();
        // } catch (IOException ignored) {
        // }
    // }
}

//=====================================================================
//                  CdmaServiceCategoryProgramHandler
//=====================================================================
CAR_INTERFACE_IMPL(CdmaServiceCategoryProgramHandler, WakeLockStateMachine, ICdmaServiceCategoryProgramHandler);

CdmaServiceCategoryProgramHandler::CdmaServiceCategoryProgramHandler()
{
}

ECode CdmaServiceCategoryProgramHandler::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* commandsInterface)
{
    WakeLockStateMachine::constructor(String("CdmaServiceCategoryProgramHandler"), context, NULL);
    mContext = context;
    mCi = commandsInterface;
    return NOERROR;
}

AutoPtr<ICdmaServiceCategoryProgramHandler> CdmaServiceCategoryProgramHandler::MakeScpHandler(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* commandsInterface)
{
    AutoPtr<ICdmaServiceCategoryProgramHandler> handler;
    CCdmaServiceCategoryProgramHandler::New(context, commandsInterface,
            (ICdmaServiceCategoryProgramHandler**)&handler);
    IStateMachine::Probe(handler)->Start();
    return handler;
}

ECode CdmaServiceCategoryProgramHandler::HandleSmsMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    if (ISmsMessage::Probe(obj) != NULL) {
        *result = HandleServiceCategoryProgramData(ISmsMessage::Probe(obj));
    }
    else {
        // Loge(String("handleMessage got object of type: ") + message.obj.getClass().getName());
        *result = FALSE;
    }
    return NOERROR;
}

Boolean CdmaServiceCategoryProgramHandler::HandleServiceCategoryProgramData(
    /* [in] */ ISmsMessage* sms)
{
    AutoPtr<IArrayList> programDataList;
    sms->GetSmsCbProgramData((IArrayList**)&programDataList);
    if (programDataList == NULL) {
        Loge(String("handleServiceCategoryProgramData: program data list is NULL!"));
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonySmsIntents::SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED_ACTION, (IIntent**)&intent);
    String str;
    ISmsMessageBase::Probe(sms)->GetOriginatingAddress(&str);
    intent->PutExtra(String("sender"), str);
    intent->PutParcelableArrayListExtra(String("program_data"), programDataList);
    Int32 id;
    mPhone->GetPhoneId(&id);
    SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, id);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    mContext->SendOrderedBroadcast(intent, Manifest::permission::RECEIVE_SMS,
            IAppOpsManager::OP_RECEIVE_SMS, mScpResultsReceiver,
            handler, IActivity::RESULT_OK, String(NULL), NULL);
    return TRUE;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
