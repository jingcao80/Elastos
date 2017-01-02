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

#include "elastos/droid/internal/telephony/WapPushOverSms.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/database/CDatabaseUtils.h"
#include "elastos/droid/provider/CTelephonySmsInbox.h"
#include "elastos/droid/provider/CTelephonyMms.h"
#include "elastos/droid/telephony/CSmsManagerHelper.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"

#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::CDatabaseUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::ITelephonyBaseMmsColumns;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Droid::Provider::ITelephonySmsInbox;
using Elastos::Droid::Provider::CTelephonySmsInbox;
using Elastos::Droid::Provider::ITelephonyMms;
using Elastos::Droid::Provider::CTelephonyMms;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ISmsManagerHelper;
using Elastos::Droid::Telephony::CSmsManagerHelper;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Google::Mms::Pdu::IDeliveryInd;
using Elastos::Droid::Google::Mms::Pdu::IPduHeaders;
using Elastos::Droid::Google::Mms::Pdu::IPduParser;
using Elastos::Droid::Google::Mms::Pdu::IPduPersister;
using Elastos::Droid::Google::Mms::Pdu::IPduPersisterHelper;
using Elastos::Droid::Google::Mms::Pdu::IReadOrigInd;
using Elastos::Droid::Google::Mms::Pdu::IPduHeaders;
using Elastos::Droid::Google::Mms::Utility::ISqliteWrapper;
using Elastos::Droid::R;
using Elastos::Droid::Manifest;

using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  WapPushOverSms::
//==============================================================
const String WapPushOverSms::TAG("WAP PUSH");
const Boolean WapPushOverSms::DBG = TRUE;

const String WapPushOverSms::THREAD_ID_SELECTION =
        ITelephonyBaseMmsColumns::MESSAGE_ID + "=? AND " + ITelephonyBaseMmsColumns::MESSAGE_TYPE + "=?";

const String WapPushOverSms::LOCATION_SELECTION =
        ITelephonyBaseMmsColumns::MESSAGE_TYPE + "=? AND " + ITelephonyBaseMmsColumns::CONTENT_LOCATION + " =?";

CAR_INTERFACE_IMPL_2(WapPushOverSms, Object, IServiceConnection, IWapPushOverSms)

ECode WapPushOverSms::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    // mWapPushManager = IWapPushManager.Stub->AsInterface(service);
    // if (DBG) Rlog->V(TAG, "wappush manager connected to " + HashCode());
    return NOERROR;
}

ECode WapPushOverSms::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mWapPushManager = NULL;
    // if (DBG) Rlog::V(TAG, "wappush manager disconnected.");
    return NOERROR;
}

ECode WapPushOverSms::constructor(
    /* [in] */ IContext* context)
{
    Logger::E("WapPushOverSms", "TODO WAPPushManage not ready line:%d, func:%s\n", __LINE__, __func__);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
    mContext = context;
    AutoPtr<IIntent> intent;
    AutoPtr<IComponentName> comp;
    Boolean bBindService = FALSE;
    //TODO depends on base/packages/WAPPushManage
    //CIntent::New(IWapPushManager.class->GetName(), (IIntent**)&intent);
    //AutoPtr<IPackageManager> pm;
    //context->GetPackageManager((IPackageManager**)&pm);
    //intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    //intent->SetComponent(comp);
    //context->BindService(intent, this, IContext::BIND_AUTO_CREATE, &bBindService);
    if (comp == NULL || !bBindService) {
        // Rlog::E(TAG, "BindService() for wappush manager failed");
    }
    else {
        // if (DBG) Rlog::V(TAG, "BindService() for wappush manager succeeded");
    }
    return NOERROR;
}

ECode WapPushOverSms::Dispose()
{
    if (mWapPushManager != NULL) {
        // if (DBG) Rlog::V(TAG, "dispose: unbind wappush manager");
        mContext->UnbindService(this);
    }
    else {
        // Rlog->E(TAG, "dispose: not bound to a wappush manager");
    }
    return NOERROR;
}

ECode WapPushOverSms::DispatchWapPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IInboundSmsHandler* handler,
    /* [in] */ const String& address,
    /* [out] */ Int32* result)
{
    // if (DBG) Rlog->D(TAG, "Rx: " + IccUtils->BytesToHexString(pdu));

    // try {
    Int32 index = 0;
    Int32 transactionId = (*pdu)[index++] & 0xFF;
    Int32 pduType = (*pdu)[index++] & 0xFF;

    // Should we "abort" if no subId for now just no supplying extra param below
    AutoPtr<IPhoneBase> phone;
    handler->GetPhone((IPhoneBase**)&phone);
    Int32 phoneId = 0;
    phone->GetPhoneId(&phoneId);

    if ((pduType != IWspTypeDecoder::PDU_TYPE_PUSH) &&
            (pduType != IWspTypeDecoder::PDU_TYPE_CONFIRMED_PUSH)) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetInteger(
                R::integer::config_valid_wappush_index,
                &index);
        if (index != -1) {
            transactionId = (*pdu)[index++] & 0xff;
            pduType = (*pdu)[index++] & 0xff;
            if (DBG) {
                // Rlog::D(TAG, "index = " + index + " PDU Type = " + pduType +
                //         " transactionID = " + transactionId);
            }

            // recheck wap push pduType
            if ((pduType != IWspTypeDecoder::PDU_TYPE_PUSH)
                    && (pduType != IWspTypeDecoder::PDU_TYPE_CONFIRMED_PUSH)) {
                if (DBG) {
                    // Rlog::W(TAG, "Received non-PUSH WAP PDU. Type = " + pduType);
                }
                *result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
                return NOERROR;
            }
        }
        else {
            // if (DBG) Rlog->W(TAG, "Received non-PUSH WAP PDU. Type = " + pduType);
            *result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
            return NOERROR;
        }
    }

    AutoPtr<IWspTypeDecoder> pduDecoder;
    assert(0 && "TODO");
    // CWspTypeDecoder::New(pdu, (IWspTypeDecoder**)&pduDecoder);

    /**
     * Parse HeaderLen(unsigned integer).
     * From wap-230-wsp-20010705-a section 8.1.2
     * The maximum size of a uintvar is 32 bits.
     * So it will be encoded in no more than 5 octets.
     */
    Boolean bInteger = FALSE;
    pduDecoder->DecodeUintvarInteger(index, &bInteger);
    if (bInteger == FALSE) {
        // if (DBG) Rlog->W(TAG, "Received PDU. Header Length error.");
        *result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
        return NOERROR;
    }
    Int64 val = 0;
    pduDecoder->GetValue32(&val);
    Int32 headerLength = (Int32) val;
    Int32 length = 0;
    pduDecoder->GetDecodedDataLength(&length);
    index += length;

    Int32 headerStartIndex = index;

    /**
     * Parse Content-Type.
     * From wap-230-wsp-20010705-a section 8.4.2.24
     *
     * Content-type-value = Constrained-media | Content-general-form
     * Content-general-form = Value-length Media-type
     * Media-type = (Well-known-media | Extension-Media) *(Parameter)
     * Value-length = Short-length | (Length-quote Length)
     * Short-length = <Any octet 0-30>   (octet <= WAP_PDU_SHORT_LENGTH_MAX)
     * Length-quote = <Octet 31>         (WAP_PDU_LENGTH_QUOTE)
     * Length = Uintvar-integer
     */
    Boolean bType = FALSE;
    pduDecoder->DecodeContentType(index, &bType);
    if (bType == FALSE) {
        // if (DBG) Rlog->W(TAG, "Received PDU. Header Content-Type error.");
        *result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
        return NOERROR;
    }

    String mimeType;
    pduDecoder->GetValueString(&mimeType);
    Int64 binaryContentType = 0;
    pduDecoder->GetValue32(&binaryContentType);
    Int32 dataLength = 0;
    pduDecoder->GetDecodedDataLength(&dataLength);
    index += dataLength;

    AutoPtr<ArrayOf<Byte> > header = ArrayOf<Byte>::Alloc(headerLength);
    header->Copy(0, pdu, headerStartIndex, header->GetLength());

    AutoPtr<ArrayOf<Byte> > intentData;

    if (!mimeType.IsNull() && mimeType.Equals(IWspTypeDecoder::CONTENT_TYPE_B_PUSH_CO)) {
        intentData = pdu;
    }
    else {
        Int32 dataIndex = headerStartIndex + headerLength;
        intentData = ArrayOf<Byte>::Alloc(pdu->GetLength() - dataIndex);
        intentData->Copy(0, pdu, dataIndex, intentData->GetLength());
    }

    AutoPtr<ISmsManagerHelper> smhlp;
    CSmsManagerHelper::AcquireSingleton((ISmsManagerHelper**)&smhlp);
    AutoPtr<ISmsManager> sm;
    smhlp->GetDefault((ISmsManager**)&sm);
    Boolean bPersis = FALSE;
    sm->GetAutoPersisting(&bPersis);
    AutoPtr<ISubscriptionManager> submg;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&submg);
    if (bPersis) {
        // Store the wap push data in telephony
        AutoPtr<ArrayOf<Int64> > subIds;
        submg->GetSubId(phoneId, (ArrayOf<Int64>**)&subIds);
        // FIXME (tomtaylor) - when we've updated SubscriptionManager, change
        // SubscriptionManager.DEFAULT_SUB_ID to SubscriptionManager->GetDefaultSmsSubId()
        Int64 subId = 0;
        if ((subIds != NULL) && (subIds->GetLength() > 0)) {
            subId = (*subIds)[0];
        }
        else {
            smhlp->GetDefaultSmsSubId(&subId);
        }
        WriteInboxMessage(subId, intentData);
    }

    /**
     * Seek for application ID field in WSP header.
     * if application ID is found, WapPushManager substitute the message
     * processing. Since WapPushManager is optional module, if WapPushManager
     * is not found, legacy message processing will be continued.
     */
    Boolean bAppId = FALSE;
    pduDecoder->SeekXWapApplicationId(index, index + headerLength - 1, &bAppId);
    if (bAppId) {
        Int64 val = 0;
        pduDecoder->GetValue32(&val);
        index = (Int32) val;
        Boolean bId = FALSE;
        pduDecoder->DecodeXWapApplicationId(index, &bId);
        String wapAppId;
        pduDecoder->GetValueString(&wapAppId);
        if (wapAppId.IsNull()) {
            pduDecoder->GetValue32(&val);
            wapAppId = StringUtils::ToString((Int32) val);
        }

        String contentType = mimeType;
        if (mimeType.IsNull()) {
            contentType = StringUtils::ToString(binaryContentType);
        }

        // if (DBG) Rlog->V(TAG, "appid found: " + wapAppId + ":" + contentType);

        // try {
        Boolean processFurther = TRUE;
        AutoPtr<IIWapPushManager> wapPushMan = mWapPushManager;

        if (wapPushMan == NULL) {
            // if (DBG) Rlog->W(TAG, "wap push manager not found!");
        }
        else {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->PutExtra(String("transactionId"), transactionId);
            intent->PutExtra(String("pduType"), pduType);
            assert(0 && "TODO");
            // intent->PutExtra(String("header"), header);
            // intent->PutExtra(String("data"), intentData);
            AutoPtr<IHashMap> parameters;
            pduDecoder->GetContentParameters((IHashMap**)&parameters);
            intent->PutExtra(String("contentTypeParameters"),
                    IParcelable::Probe(parameters));
            if (!TextUtils::IsEmpty(address)) {
                intent->PutExtra(String("address"), address);
            }
            submg->PutPhoneIdAndSubIdExtra(intent, phoneId);

            Int32 procRet = 0;
            wapPushMan->ProcessMessage(wapAppId, contentType, intent, &procRet);
            // if (DBG) Rlog->V(TAG, "procRet:" + procRet);
            if ((procRet & IWapPushManagerParams::MESSAGE_HANDLED) > 0
                    && (procRet & IWapPushManagerParams::FURTHER_PROCESSING) == 0) {
                processFurther = FALSE;
            }
        }
        if (!processFurther) {
            *result = ITelephonySmsIntents::RESULT_SMS_HANDLED;
            return NOERROR;
        }
        // } Catch (RemoteException e) {
        //     if (DBG) Rlog->W(TAG, "remote func failed...");
        // }
    }
    // if (DBG) Rlog->V(TAG, "fall back to existing handler");

    if (mimeType.IsNull()) {
        // if (DBG) Rlog->W(TAG, "Header Content-Type error.");
        *result = ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR;
        return NOERROR;
    }

    String permission;
    Int32 appOp;

    if (mimeType.Equals(IWspTypeDecoder::CONTENT_TYPE_B_MMS)) {
        permission = Manifest::permission::RECEIVE_MMS;
        appOp = IAppOpsManager::OP_RECEIVE_MMS;
    }
    else {
        permission = Manifest::permission::RECEIVE_WAP_PUSH;
        appOp = IAppOpsManager::OP_RECEIVE_WAP_PUSH;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonySmsIntents::WAP_PUSH_DELIVER_ACTION, (IIntent**)&intent);
    intent->SetType(mimeType);
    intent->PutExtra(String("transactionId"), transactionId);
    intent->PutExtra(String("pduType"), pduType);
    assert(0 && "TODO");
    // intent->PutExtra(String("header"), header);
    // intent->PutExtra(String("data"), intentData);
    AutoPtr<IHashMap> parameters;
    pduDecoder->GetContentParameters((IHashMap**)&parameters);
    // intent->PutExtra(String("contentTypeParameters"), parameters);
    if (!TextUtils::IsEmpty(address)) {
        intent->PutExtra(String("address"), address);
    }
    submg->PutPhoneIdAndSubIdExtra(intent, phoneId);

    // Direct the intent to only the default MMS app. if we can't find a default MMS app
    // then sent it to all broadcast receivers.
    AutoPtr<ISmsApplication> smsApp;
    assert(0 && "TODO");
    // CSmsApplication::AcquireSingleton((ISmsApplication**)&smsApp);
    AutoPtr<IComponentName> componentName;
    smsApp->GetDefaultMmsApplication(mContext, TRUE, (IComponentName**)&componentName);
    if (componentName != NULL) {
        // Deliver MMS message only to this receiver
        intent->SetComponent(componentName);
        // if (DBG) Rlog->V(TAG, "Delivering MMS to: " + componentName->GetPackageName() +
        //         " " + componentName->GetClassName());
    }

    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> owner;
    uhhlp->GetOWNER((IUserHandle**)&owner);
    assert(0 && "TODO protected method");
    // handler->DispatchIntent(intent, permission, appOp, receiver, owner);
    *result = IActivity::RESULT_OK;
    return NOERROR;
    // } Catch (ArrayIndexOutOfBoundsException aie) {
    //     // 0-Byte WAP PDU or other unexpected WAP PDU contents can easily throw this;
    //     // log exception string without stack trace and return FALSE.
    //     Rlog->E(TAG, "ignoring DispatchWapPdu() array index exception: " + aie);
    //     return Intents.RESULT_SMS_GENERIC_ERROR;
    // }
}

void WapPushOverSms::WriteInboxMessage(
    /* [in] */ Int64 subId,
    /* [in] */ ArrayOf<Byte>* pushData)
{
    AutoPtr<IPduParser> parser;
    assert(0 && "TODO");
    // CPduParser::New((IPduParser**)&parser);
    AutoPtr<IGenericPdu> pdu;
    parser->Parse((IGenericPdu**)&pdu);
    if (pdu == NULL) {
        // Rlog::E(TAG, "Invalid PUSH PDU");
    }
    AutoPtr<IPduPersisterHelper> pphlp;
    assert(0 && "TODO");
    // CPduPersisterHelper::AcquireSingleton((IPduPersisterHelper**)&pphlp);
    AutoPtr<IPduPersister> persister;
    pphlp->GetPduPersister(mContext, (IPduPersister**)&persister);
    Int32 type = 0;
    pdu->GetMessageType(&type);
    // try {
    switch (type) {
        case IPduHeaders::MESSAGE_TYPE_DELIVERY_IND:
        case IPduHeaders::MESSAGE_TYPE_READ_ORIG_IND: {
            Int64 threadId = GetDeliveryOrReadReportThreadId(mContext, pdu);
            if (threadId == -1) {
                // The associated SendReq isn't found, therefore skip
                // processing this PDU.
                // Rlog::E(TAG, "Failed to find delivery or read report's thread id");
                break;
            }
            AutoPtr<ITelephonySmsInbox> inbox;
            CTelephonySmsInbox::AcquireSingleton((ITelephonySmsInbox**)&inbox);
            AutoPtr<IUri> content_uri;
            inbox->GetCONTENT_URI((IUri**)&content_uri);
            AutoPtr<IUri> uri;
            persister->Persist(
                    pdu,
                    content_uri,
                    TRUE/*createThreadId*/,
                    TRUE/*groupMmsEnabled*/,
                    NULL/*preOpenedFiles*/,
                    (IUri**)&uri);
            if (uri == NULL) {
                // Rlog->E(TAG, "Failed to persist delivery or read report");
                break;
            }
            // Update thread ID for ReadOrigInd & DeliveryInd.
            AutoPtr<IContentValues> values;
            CContentValues::New(1, (IContentValues**)&values);
            values->Put(ITelephonyBaseMmsColumns::THREAD_ID, threadId);
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            AutoPtr<ISqliteWrapper> sw;
            assert(0 && "TODO");
            // CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
            Int32 updateRes = 0;
            sw->Update(
                    mContext,
                    cr,
                    uri,
                    values,
                    String(NULL)/*where*/,
                    NULL/*selectionArgs*/,
                    &updateRes);
            if (updateRes != 1) {
                // Rlog->E(TAG, "Failed to update delivery or read report thread id");
            }
            break;
        }
        case IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND: {
            AutoPtr<INotificationInd> nInd = INotificationInd::Probe(pdu);

            AutoPtr<ISmsManagerHelper> smhlp;
            CSmsManagerHelper::AcquireSingleton((ISmsManagerHelper**)&smhlp);
            AutoPtr<ISmsManager> sm;
            smhlp->GetSmsManagerForSubscriber(subId, (ISmsManager**)&sm);
            AutoPtr<IBundle> configs;
            sm->GetCarrierConfigValues((IBundle**)&configs);
            if (configs != NULL) {
                Boolean b = FALSE;
                configs->GetBoolean(
                    ISmsManager::MMS_CONFIG_APPEND_TRANSACTION_ID, FALSE, &b);
                if (b) {
                    AutoPtr<ArrayOf<Byte> > contentLocation;
                    nInd->GetContentLocation((ArrayOf<Byte>**)&contentLocation);
                    if ('=' == (*contentLocation)[contentLocation->GetLength() - 1]) {
                        AutoPtr<ArrayOf<Byte> > transactionId;
                        nInd->GetTransactionId((ArrayOf<Byte>**)&transactionId);
                        AutoPtr<ArrayOf<Byte> > contentLocationWithId = ArrayOf<Byte>::Alloc(contentLocation->GetLength()
                                + transactionId->GetLength());
                        contentLocationWithId->Copy(0, contentLocation, 0, contentLocation->GetLength());
                        contentLocationWithId->Copy(contentLocation->GetLength(), transactionId, 0, transactionId->GetLength());
                        nInd->SetContentLocation(contentLocationWithId);
                    }
                }
            }
            if (!IsDuplicateNotification(mContext, nInd)) {
                AutoPtr<ITelephonySmsInbox> inbox;
                CTelephonySmsInbox::AcquireSingleton((ITelephonySmsInbox**)&inbox);
                AutoPtr<IUri> content_uri;
                inbox->GetCONTENT_URI((IUri**)&content_uri);
                AutoPtr<IUri> uri;
                persister->Persist(
                        pdu,
                        content_uri,
                        TRUE/*createThreadId*/,
                        TRUE/*groupMmsEnabled*/,
                        NULL/*preOpenedFiles*/,
                        (IUri**)&uri);
                if (uri == NULL) {
                    // Rlog->E(TAG, "Failed to save MMS WAP push notification ind");
                }
            }
            else {
                // Rlog->D(TAG, "Skip storing duplicate MMS WAP push notification ind: "
                //         + new String(nInd->GetContentLocation()));
            }
            break;
        }
        default:
            Logger::E(TAG, "Received unrecognized WAP Push PDU.");
    }
    // } Catch (MmsException e) {
    //     Logger::E(TAG, "Failed to save MMS WAP push data: type=" + type, e);
    // } Catch (RuntimeException e) {
    //     Logger::E(TAG, "Unexpected RuntimeException in persisting MMS WAP push data", e);
    // }
}

Int64 WapPushOverSms::GetDeliveryOrReadReportThreadId(
    /* [in] */ IContext* context,
    /* [in] */ IGenericPdu* pdu)
{
    String messageId;
    if (IDeliveryInd::Probe(pdu) != NULL) {
        AutoPtr<ArrayOf<Byte> > ids;
        IDeliveryInd::Probe(pdu)->GetMessageId((ArrayOf<Byte>**)&ids);
        messageId = String(*ids);
    }
    else if (IReadOrigInd::Probe(pdu) != NULL) {
        AutoPtr<ArrayOf<Byte> > ids;
        IReadOrigInd::Probe(pdu)->GetMessageId((ArrayOf<Byte>**)&ids);
        messageId = String(*ids);
    }
    else {
        // Rlog->E(TAG, "WAP Push data is neither delivery or read report type: "
        //         + pdu->GetClass()->GetCanonicalName());
        return -1l;
    }
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<ISqliteWrapper> sw;
    assert(0 && "TODO");
    // CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > arr1 = ArrayOf<String>::Alloc(1);
    (*arr1)[0] = ITelephonyBaseMmsColumns::THREAD_ID;
    AutoPtr<ArrayOf<String> > arr2 = ArrayOf<String>::Alloc(2);
    AutoPtr<IDatabaseUtils> dbUtils;
    CDatabaseUtils::AcquireSingleton((IDatabaseUtils**)&dbUtils);
    dbUtils->SqlEscapeString(messageId, &((*arr2)[0]));
    (*arr2)[1] = StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_SEND_REQ);
    AutoPtr<ITelephonyMms> mms;
    CTelephonyMms::AcquireSingleton((ITelephonyMms**)&mms);
    AutoPtr<IUri> content_uri;
    mms->GetCONTENT_URI((IUri**)&content_uri);
    sw->Query(
            context,
            cr,
            content_uri,
            arr1,
            THREAD_ID_SELECTION,
            arr2,
            String(NULL)/*sortOrder*/,
            (ICursor**)&cursor);
    Boolean bMF = FALSE;
    if (cursor != NULL && (cursor->MoveToFirst(&bMF), bMF)) {
        Int64 res = 0;
        cursor->GetInt64(0, &res);
        return res;
    }
    // } Catch (SQLiteException e) {
    //     Rlog->E(TAG, "Failed to query delivery or read report thread id", e);
    // } finally {
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    // }
    return -1l;
}

Boolean WapPushOverSms::IsDuplicateNotification(
    /* [in] */ IContext* context,
    /* [in] */ INotificationInd* nInd)
{
    AutoPtr<ArrayOf<Byte> > rawLocation;
    nInd->GetContentLocation((ArrayOf<Byte>**)&rawLocation);
    if (rawLocation != NULL) {
        String location(*rawLocation);
        AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = location;
        AutoPtr<ICursor> cursor;
        // try {
        AutoPtr<ISqliteWrapper> sw;
        assert(0 && "TODO");
        // CSqliteWrapper::AcquireSingleton((ISqliteWrapper**)&sw);
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ArrayOf<String> > arr1 = ArrayOf<String>::Alloc(1);
        assert(0 && "TODO");
        // (*arr1)[0] = ITelephonyMms::_ID;
        AutoPtr<ArrayOf<String> > arr2 = ArrayOf<String>::Alloc(2);
        (*arr2)[0] = StringUtils::ToString(IPduHeaders::MESSAGE_TYPE_NOTIFICATION_IND);
        (*arr2)[1] = String(*rawLocation);
        AutoPtr<ITelephonyMms> mms;
        CTelephonyMms::AcquireSingleton((ITelephonyMms**)&mms);
        AutoPtr<IUri> content_uri;
        mms->GetCONTENT_URI((IUri**)&content_uri);
        sw->Query(
                context,
                cr,
                content_uri,
                arr1,
                LOCATION_SELECTION,
                arr2,
                String(NULL)/*sortOrder*/,
                (ICursor**)&cursor);
        Int32 count = 0;
        if (cursor != NULL && (cursor->GetCount(&count), count) > 0) {
            // We already received the same notification before.
            return TRUE;
        }
        // } Catch (SQLiteException e) {
        //     Rlog->E(TAG, "failed to query existing notification ind", e);
        // } finally {
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
        // }
    }
    return FALSE;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
