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

#include "elastos/droid/internal/telephony/IccSmsInterfaceManager.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSmsBroadcastConfigInfo.h"
#include "elastos/droid/internal/telephony/gsm/CSmsBroadcastConfigInfo.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include "elastos/droid/telephony/SmsManager.h"
#include "elastos/droid/internal/telephony/CSmsNumberUtils.h"
#include "elastos/droid/internal/telephony/CSmsRawData.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/ImsSMSDispatcher.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Telephony::Cdma::CdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Gsm::CSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::ITelephonyTextBasedSmsColumns;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::SmsMessage;
using Elastos::Droid::Telephony::SmsManager;
using Elastos::Core::AutoLock;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String IccSmsInterfaceManager::TAG("IccSmsInterfaceManager");
const Boolean IccSmsInterfaceManager::DBG = FALSE;
const Int32 IccSmsInterfaceManager::EVENT_SET_BROADCAST_ACTIVATION_DONE = 3;
const Int32 IccSmsInterfaceManager::EVENT_SET_BROADCAST_CONFIG_DONE = 4;
const Int32 IccSmsInterfaceManager::EVENT_LOAD_DONE = 1;
const Int32 IccSmsInterfaceManager::EVENT_UPDATE_DONE = 2;
const Int32 IccSmsInterfaceManager::SMS_CB_CODE_SCHEME_MIN = 0;
const Int32 IccSmsInterfaceManager::SMS_CB_CODE_SCHEME_MAX = 255;

IccSmsInterfaceManager::InnerHandler::InnerHandler(
    /* [in] */ IccSmsInterfaceManager* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode IccSmsInterfaceManager::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AsyncResult* ar = NULL;
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_UPDATE_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            {
                AutoLock lock(mHost->mLock);
                mHost->mSuccess = (ar->mException == NULL);
                mHost->mLock.NotifyAll();
            }
            break;
        }
        case EVENT_LOAD_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            {
                AutoLock lock(mHost->mLock);
                if (ar->mException == NULL) {
                    assert(IArrayList::Probe(ar->mResult));
                    mHost->mSms = IList::Probe(mHost->BuildValidRawData(IArrayList::Probe(ar->mResult)));
                    //Mark SMS as read after importing it from card.
                    mHost->MarkMessagesAsRead(IArrayList::Probe(ar->mResult));
                }
                else {
                    if (Logger::IsLoggable("SMS", Logger::___DEBUG)) {
                        mHost->Log(String("Cannot load Sms records"));
                    }
                    if (mHost->mSms != NULL) {
                        mHost->mSms->Clear();
                    }
                }
                mHost->mLock.NotifyAll();
            }
            break;
        }
        case EVENT_SET_BROADCAST_ACTIVATION_DONE:
        case EVENT_SET_BROADCAST_CONFIG_DONE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            {
                AutoLock lock(mHost->mLock);
                mHost->mSuccess = (ar->mException == NULL);
                mHost->mLock.NotifyAll();
            }
            break;
        }
    }
    return NOERROR;
}

IccSmsInterfaceManager::CellBroadcastRangeManager::CellBroadcastRangeManager(
    /* [in] */ IccSmsInterfaceManager* host)
    : mHost(host)
{
    CArrayList::New((IArrayList**)&mConfigList);
}

void IccSmsInterfaceManager::CellBroadcastRangeManager::StartUpdate()
{
    mConfigList->Clear();
}

void IccSmsInterfaceManager::CellBroadcastRangeManager::AddRange(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ Boolean selected)
{
    AutoPtr<ISmsBroadcastConfigInfo> info;
    CSmsBroadcastConfigInfo::New(startId, endId, SMS_CB_CODE_SCHEME_MIN,
            SMS_CB_CODE_SCHEME_MAX, selected, (ISmsBroadcastConfigInfo**)&info);
    mConfigList->Add(info);
}

Boolean IccSmsInterfaceManager::CellBroadcastRangeManager::FinishUpdate()
{
    Boolean tmp = FALSE;
    if (mConfigList->IsEmpty(&tmp), tmp) {
        return TRUE;
    }

    Int32 size = 0;
    mConfigList->GetSize(&size);
    AutoPtr<ArrayOf<IInterface *> > tmpConfigs = ArrayOf<IInterface *>::Alloc(size);
    AutoPtr<ArrayOf<IInterface *> > configs;
    // SmsBroadcastConfigInfo[] configs =
    //         mConfigList.toArray(new SmsBroadcastConfigInfo[mConfigList.size()]);
    mConfigList->ToArray(tmpConfigs, (ArrayOf<IInterface*>**)&configs);

    AutoPtr<ArrayOf<ISmsBroadcastConfigInfo*> > array =
        ArrayOf<ISmsBroadcastConfigInfo*>::Alloc(size);

    for (Int32 i = 0; i < size; i++) {
        array->Set(i, ISmsBroadcastConfigInfo::Probe((*configs)[i]));
    }
    return mHost->SetCellBroadcastConfig(array);
}

IccSmsInterfaceManager::CdmaBroadcastRangeManager::CdmaBroadcastRangeManager(
    /* [in] */ IccSmsInterfaceManager* host)
    : mHost(host)
{
    CArrayList::New((IArrayList**)&mConfigList);
}

void IccSmsInterfaceManager::CdmaBroadcastRangeManager::StartUpdate()
{
    mConfigList->Clear();
}

void IccSmsInterfaceManager::CdmaBroadcastRangeManager::AddRange(
    /* [in] */ Int32 startId,
    /* [in] */ Int32 endId,
    /* [in] */ Boolean selected)
{
    AutoPtr<CdmaSmsBroadcastConfigInfo> info = new CdmaSmsBroadcastConfigInfo();
    info->constructor(startId, endId, 1, selected);
    mConfigList->Add(info->Probe(EIID_IInterface));
}

Boolean IccSmsInterfaceManager::CdmaBroadcastRangeManager::FinishUpdate()
{
    Boolean isEmpty = FALSE;
    if (mConfigList->IsEmpty(&isEmpty), isEmpty) {
        return TRUE;
    }

    Int32 size = 0;
    mConfigList->GetSize(&size);
    AutoPtr<ArrayOf<IInterface *> > tmpConfigs = ArrayOf<IInterface *>::Alloc(size);
    AutoPtr<ArrayOf<IInterface *> > configs;
    // CdmaSmsBroadcastConfigInfo[] configs =
    //         mConfigList.toArray(new CdmaSmsBroadcastConfigInfo[mConfigList.size()]);
    mConfigList->ToArray(tmpConfigs, (ArrayOf<IInterface*>**)&configs);

    AutoPtr<ArrayOf<ICdmaSmsBroadcastConfigInfo*> > array =
        ArrayOf<ICdmaSmsBroadcastConfigInfo*>::Alloc(size);

    for (Int32 i = 0; i < size; i++) {
        array->Set(i, ICdmaSmsBroadcastConfigInfo::Probe((*configs)[i]));
    }
    return mHost->SetCdmaBroadcastConfig(array);
}


CAR_INTERFACE_IMPL(IccSmsInterfaceManager, Object, IIccSmsInterfaceManager)
IccSmsInterfaceManager::IccSmsInterfaceManager(
    /* [in] */ IPhoneBase* phone)
    : mSuccess(FALSE)
{
    mHandler = new InnerHandler(this);
    mCellBroadcastRangeManager = new CellBroadcastRangeManager(this);
    mCdmaBroadcastRangeManager = new CdmaBroadcastRangeManager(this);
    mPhone = phone;

    IPhone::Probe(phone)->GetContext((IContext**)&mContext);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOps = IAppOpsManager::Probe(obj);

    obj = NULL;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);

    Logger::E("IccSmsInterfaceManager", "TODO  ImsSMSDispatcher is not ready!");
    PhoneBase* pb = (PhoneBase*)phone;
    mDispatcher = new ImsSMSDispatcher(phone, pb->mSmsStorageMonitor, pb->mSmsUsageMonitor);
}

void IccSmsInterfaceManager::MarkMessagesAsRead(
    /* [in] */ IArrayList* messages)
{
    if (messages == NULL) {
        return;
    }

    //IccFileHandler can be NULL, if icc card is absent.
    AutoPtr<IIccFileHandler> fh;
    mPhone->GetIccFileHandler((IIccFileHandler**)&fh);
    if (fh == NULL) {
        //shouldn't really happen, as messages are marked as read, only
        //after importing it from icc.
        if (Logger::IsLoggable("SMS", Logger::___DEBUG)) {
            Log(String("markMessagesAsRead - aborting, no icc card present."));
        }
        return;
    }

    Int32 count = 0;
    messages->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        messages->Get(i, (IInterface**)&obj);
        assert(IArrayOf::Probe(obj));
        // TODO ?
        // Byte[] ba = messages.get(i);
        // if (ba[0] == STATUS_ON_ICC_UNREAD) {
        //     Int32 n = ba.length;
        //     Byte[] nba = new Byte[n - 1];
        //     System.arraycopy(ba, 1, nba, 0, n - 1);
        //     Byte[] record = makeSmsRecordData(STATUS_ON_ICC_READ, nba);
        //     fh.updateEFLinearFixed(IccConstants.EF_SMS, i + 1, record, NULL, NULL);
        //     if (Logger::IsLoggable("SMS", Logger::___DEBUG)) {
        //         Log("SMS " + (i + 1) + " marked as read");
        //     }
        // }
        AutoPtr<IArrayOf> ba = IArrayOf::Probe(obj);
        AutoPtr<IInterface> bObj;
        ba->Get(0, (IInterface**)&bObj);
        Byte b;
        IByte::Probe(bObj)->GetValue(&b);
        if (b == SmsManager::STATUS_ON_ICC_UNREAD) {
            Int32 n = 0;
            ba->GetLength(&n);
            AutoPtr<ArrayOf<Byte> > nba = ArrayOf<Byte>::Alloc(n - 1);
            // System.arraycopy(ba, 1, nba, 0, n - 1);
            for (Int32 j = 1; j < n; j++) {
                bObj = NULL;
                ba->Get(j, (IInterface**)&bObj);
                IByte::Probe(bObj)->GetValue(&b);
                (*nba)[j - 1] = b;
            }

            AutoPtr<ArrayOf<Byte> > record = MakeSmsRecordData(SmsManager::STATUS_ON_ICC_READ, nba);
            fh->UpdateEFLinearFixed(IIccConstants::EF_SMS, i + 1, record, String(NULL), NULL);
            if (Logger::IsLoggable("SMS", Logger::___DEBUG)) {
                Log(String("SMS ") + StringUtils::ToString(i + 1) + " marked as read");
            }
        }
    }
}

void IccSmsInterfaceManager::UpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    mPhone = phone;
    ((SMSDispatcher*)mDispatcher.Get())->UpdatePhoneObject(phone);
}

void IccSmsInterfaceManager::EnforceReceiveAndSend(
    /* [in] */ const String& message)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::RECEIVE_SMS, message);
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::SEND_SMS, message);
}

/**
 * Update the specified message on the Icc.
 *
 * @param index record index of message to update
 * @param status new message status (STATUS_ON_ICC_READ,
 *                  STATUS_ON_ICC_UNREAD, STATUS_ON_ICC_SENT,
 *                  STATUS_ON_ICC_UNSENT, STATUS_ON_ICC_FREE)
 * @param pdu the raw PDU to store
 * @return success or not
 *
 */
ECode IccSmsInterfaceManager::UpdateMessageOnIccEf(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 index,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) {
        // Log(String("updateMessageOnIccEf: index=") + StringUtils::ToString(index) +
        //     " status=" + StringUtils::ToString(status) + " ==> " +
        //     "("+ Arrays.toString(pdu) + ")");
    }
    EnforceReceiveAndSend(String("Updating message on Icc"));
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_WRITE_ICC_SMS, Binder::GetCallingUid(), callingPackage, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        *result = FALSE;
        return NOERROR;
    }
    {
        AutoLock lock(mLock);
        mSuccess = FALSE;
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_UPDATE_DONE, (IMessage**)&response);

        if (status == SmsManager::STATUS_ON_ICC_FREE) {
            // RIL_REQUEST_DELETE_SMS_ON_SIM vs RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM
            // Special case FREE: call deleteSmsOnSim/Ruim instead of
            // manipulating the record
            // Will eventually fail if icc card is not present.
            IPhone::Probe(mPhone)->GetPhoneType(&v);
            if (IPhoneConstants::PHONE_TYPE_GSM == v) {
                ((PhoneBase*)mPhone.Get())->mCi->DeleteSmsOnSim(index, response);
            }
            else {
                ((PhoneBase*)mPhone.Get())->mCi->DeleteSmsOnRuim(index, response);
            }
        }
        else {
            //IccFilehandler can be NULL if ICC card is not present.
            AutoPtr<IIccFileHandler> fh;
            mPhone->GetIccFileHandler((IIccFileHandler**)&fh);
            if (fh == NULL) {
                response->Recycle();
                *result = mSuccess; /* is FALSE */
                return NOERROR;
            }
            AutoPtr<ArrayOf<Byte> > record = MakeSmsRecordData(status, pdu);
            fh->UpdateEFLinearFixed(
                    IIccConstants::EF_SMS,
                    index, record, String(NULL), response);
        }
        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to update by index"));
        }
    }
    *result = mSuccess;
    return NOERROR;
}

/**
 * Copy a raw SMS PDU to the Icc.
 *
 * @param pdu the raw PDU to store
 * @param status message status (STATUS_ON_ICC_READ, STATUS_ON_ICC_UNREAD,
 *               STATUS_ON_ICC_SENT, STATUS_ON_ICC_UNSENT)
 * @return success or not
 *
 */
ECode IccSmsInterfaceManager::CopyMessageToIccEf(
    /* [in] */ const String& callingPackage,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ ArrayOf<Byte>* smsc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //NOTE smsc not used in RUIM
    if (DBG) {
        // Log("copyMessageToIccEf: status=" + status + " ==> " +
        //     "pdu=("+ Arrays.toString(pdu) +
        //     "), smsc=(" + Arrays.toString(smsc) +")");
    }
    EnforceReceiveAndSend(String("Copying message to Icc"));
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_WRITE_ICC_SMS, Binder::GetCallingUid(), callingPackage, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        *result = FALSE;
        return NOERROR;
    }

    {
        AutoLock lock(mLock);
        mSuccess = FALSE;
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_UPDATE_DONE, (IMessage**)&response);

        //RIL_REQUEST_WRITE_SMS_TO_SIM vs RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM
        IPhone::Probe(mPhone)->GetPhoneType(&v);
        if (IPhoneConstants::PHONE_TYPE_GSM == v) {
            ((PhoneBase*)mPhone.Get())->mCi->WriteSmsToSim(status, IccUtils::BytesToHexString(smsc),
                    IccUtils::BytesToHexString(pdu), response);
        }
        else {
            ((PhoneBase*)mPhone.Get())->mCi->WriteSmsToRuim(status, IccUtils::BytesToHexString(pdu),
                    response);
        }

        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to update by index"));
        }

    }
    *result = mSuccess;
    return NOERROR;
}

ECode IccSmsInterfaceManager::SynthesizeMessages(
    /* [in] */ const String& originatingAddress,
    /* [in] */ const String& scAddress,
    /* [in] */ IList/*<String>*/* messages,
    /* [in] */ Int64 timestampMillis) /*throws RemoteException*/
{
    return NOERROR;
}

/**
 * Retrieves all messages currently stored on Icc.
 *
 * @return list of SmsRawData of all sms on Icc
 */
ECode IccSmsInterfaceManager::GetAllMessagesFromIccEf(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Log(String("getAllMessagesFromEF"));

    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::RECEIVE_SMS,
            String("Reading messages from Icc"));
    {
        AutoLock lock(mLock);
        AutoPtr<IIccFileHandler> fh;
        mPhone->GetIccFileHandler((IIccFileHandler**)&fh);
        if (fh == NULL) {
            Logger::E(TAG, "Cannot load Sms records. No icc card?");
            if (mSms != NULL) {
                mSms->Clear();
                *result = mSms;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
        }

        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_LOAD_DONE, (IMessage**)&response);
        fh->LoadEFLinearFixedAll(IIccConstants::EF_SMS, response);

        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to load from the Icc"));
        }
    }
    *result = mSms;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Send a data based SMS to a specific application port.
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *  the current default SMSC
 * @param destPort the port to deliver the message to
 * @param data the body of the message to send
 * @param sentIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is successfully sent, or failed.
 *  The result code will be <code>Activity.RESULT_OK<code> for success,
 *  or one of these errors:<br>
 *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
 *  <code>RESULT_ERROR_RADIO_OFF</code><br>
 *  <code>RESULT_ERROR_NULL_PDU</code><br>
 *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
 *  the extra "errorCode" containing a radio technology specific value,
 *  generally only useful for troubleshooting.<br>
 *  The per-application based SMS control checks sentIntent. If sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is delivered to the recipient.  The
 *  raw pdu of the status report is in the extended data ("pdu").
 */
ECode IccSmsInterfaceManager::SendData(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->EnforceCallingPermission(
            Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        // Log("sendData: destAddr=" + destAddr + " scAddr=" + scAddr + " destPort=" +
        //     destPort + " data='"+ HexDump.toHexString(data)  + "' sentIntent=" +
        //     sentIntent + " deliveryIntent=" + deliveryIntent);
    }
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPackage, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    String _destAddr = FilterDestAddress(destAddr);
    ((SMSDispatcher*)mDispatcher.Get())->SendData(_destAddr, scAddr, destPort, 0, data, sentIntent, deliveryIntent);
    return NOERROR;
}

/**
 * Send a data based SMS to a specific application port.
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *  the current default SMSC
 * @param destPort the port to deliver the message to
 * @param origPort the originator port set by sender
 * @param data the body of the message to send
 * @param sentIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is successfully sent, or failed.
 *  The result code will be <code>Activity.RESULT_OK<code> for success,
 *  or one of these errors:<br>
 *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
 *  <code>RESULT_ERROR_RADIO_OFF</code><br>
 *  <code>RESULT_ERROR_NULL_PDU</code><br>
 *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
 *  the extra "errorCode" containing a radio technology specific value,
 *  generally only useful for troubleshooting.<br>
 *  The per-application based SMS control checks sentIntent. If sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is delivered to the recipient.  The
 *  raw pdu of the status report is in the extended data ("pdu").
 */
ECode IccSmsInterfaceManager::SendDataWithOrigPort(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ Int32 destPort,
    /* [in] */ Int32 origPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->EnforceCallingPermission(
            Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        // Log("sendDataWithOrigPort: destAddr=" + destAddr + " scAddr=" + scAddr
        //     + " destPort=" +destPort + "origPort=" + origPort
        //     + " data='"+ HexDump.toHexString(data) +
        //     "' sentIntent=" + sentIntent + " deliveryIntent=" + deliveryIntent);
    }

    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPackage, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    String _destAddr = FilterDestAddress(destAddr);
    ((SMSDispatcher*)mDispatcher.Get())->SendData(_destAddr, scAddr, destPort, origPort,
            data, sentIntent, deliveryIntent);
    return NOERROR;
}

/**
 * Send a text based SMS.
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *  the current default SMSC
 * @param text the body of the message to send
 * @param sentIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is successfully sent, or failed.
 *  The result code will be <code>Activity.RESULT_OK<code> for success,
 *  or one of these errors:<br>
 *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
 *  <code>RESULT_ERROR_RADIO_OFF</code><br>
 *  <code>RESULT_ERROR_NULL_PDU</code><br>
 *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
 *  the extra "errorCode" containing a radio technology specific value,
 *  generally only useful for troubleshooting.<br>
 *  The per-application based SMS control checks sentIntent. If sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is delivered to the recipient.  The
 *  raw pdu of the status report is in the extended data ("pdu").
 */
ECode IccSmsInterfaceManager::SendText(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<ArrayOf<String> > callingParts;
    StringUtils::Split(callingPackage, String("\\\\"), (ArrayOf<String>**)&callingParts);
    if (callingUid == IProcess::PHONE_UID && callingParts->GetLength() > 1) {
        StringUtils::Parse((*callingParts)[1], &callingUid);
    }

    // Reset the calling package, remove the trailing uid so
    // shouldWriteMessageForPackage can match correctly
    // if our message has been synthesized by an
    // external package
    String _callingPackage = (*callingParts)[0];

    if (Binder::GetCallingPid() != Process::MyPid()) {
        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        ctx->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                String("Sending SMS message"));
    }
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        String ss;
        if (sentIntent != NULL) {
            ss = TO_CSTR(sentIntent);
        }
        String ds;
        if (deliveryIntent != NULL) {
            ds = TO_CSTR(deliveryIntent);
        }
        Log(String("sendText: destAddr=") + destAddr + " scAddr=" + scAddr +
            " text='"+ text + "' sentIntent=" + ss + " deliveryIntent=" + ds);
    }

    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, callingUid,
            (*callingParts)[0], &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    String _destAddr = FilterDestAddress(destAddr);
    ((SMSDispatcher*)mDispatcher.Get())->SendText(_destAddr, scAddr, text, sentIntent, deliveryIntent,
            NULL/*messageUri*/, _callingPackage, -1, FALSE, -1);
    return NOERROR;
}

/**
 * Send a text based SMS with Messaging Options.
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *  the current default SMSC
 * @param text the body of the message to send
 * @param sentIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is successfully sent, or failed.
 *  The result code will be <code>Activity.RESULT_OK<code> for success,
 *  or one of these errors:<br>
 *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
 *  <code>RESULT_ERROR_RADIO_OFF</code><br>
 *  <code>RESULT_ERROR_NULL_PDU</code><br>
 *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
 *  the extra "errorCode" containing a radio technology specific value,
 *  generally only useful for troubleshooting.<br>
 *  The per-application based SMS control checks sentIntent. If sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is delivered to the recipient.  The
 *  raw pdu of the status report is in the extended data ("pdu").
 * @param priority Priority level of the message
 *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
 *  ---------------------------------
 *  PRIORITY      | Level of Priority
 *  ---------------------------------
 *      '00'      |     Normal
 *      '01'      |     Interactive
 *      '10'      |     Urgent
 *      '11'      |     Emergency
 *  ----------------------------------
 *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
 * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
 * @param validityPeriod Validity Period of the message in mins.
 *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
 *  Validity Period(Minimum) -> 5 mins
 *  Validity Period(Maximum) -> 635040 mins(i.e.63 weeks).
 *  Any Other values included Negative considered as Invalid Validity Period of the message.
 */
ECode IccSmsInterfaceManager::SendTextWithOptions(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ const String& text,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->EnforceCallingPermission(
            Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        Log(String("sendText: destAddr=") + destAddr + " scAddr=" + scAddr +
            " text='"+ text + "' sentIntent=" +
            TO_CSTR(sentIntent) + " deliveryIntent=" + TO_CSTR(deliveryIntent) +
            "validityPeriod" + StringUtils::ToString(validityPeriod));
    }
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPackage, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    ((SMSDispatcher*)mDispatcher.Get())->SendText(destAddr, scAddr, text, sentIntent, deliveryIntent,
            NULL/*messageUri*/, callingPackage, priority,
            isExpectMore, validityPeriod);
    return NOERROR;
}

/**
 * Inject an SMS PDU into the android application framework.
 *
 * @param pdu is the Byte array of pdu to be injected into android application framework
 * @param format is the format of SMS pdu (3gpp or 3gpp2)
 * @param receivedIntent if not NULL this <code>PendingIntent</code> is
 *  broadcast when the message is successfully received by the
 *  android application framework. This intent is broadcasted at
 *  the same time an SMS received from radio is acknowledged back.
 */
ECode IccSmsInterfaceManager::InjectSmsPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [in] */ IPendingIntent* receivedIntent)
{
    EnforceCarrierPrivilege();
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        Log(String("pdu: ") + StringUtils::ToString((Int32)pdu) +
            "\n format=" + format +
            "\n receivedIntent=" + TO_CSTR(receivedIntent));
    }
    ((SMSDispatcher*)mDispatcher.Get())->InjectSmsPdu(pdu, format, receivedIntent);
    return NOERROR;
}

/**
 * Update the status of a pending (send-by-IP) SMS message and resend by PSTN if necessary.
 * This outbound message was handled by the carrier app. If the carrier app fails to send
 * this message, it would be resent by PSTN.
 *
 * @param messageRef the reference number of the SMS message.
 * @param success True if and only if the message was sent successfully. If its value is
 *  FALSE, this message should be resent via PSTN.
 * {@hide}
 */
ECode IccSmsInterfaceManager::UpdateSmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Boolean success)
{
    EnforceCarrierPrivilege();
    ((SMSDispatcher*)mDispatcher.Get())->UpdateSmsSendStatus(messageRef, success);
    return NOERROR;
}

/**
 * Send a multi-part text based SMS.
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *   the current default SMSC
 * @param parts an <code>ArrayList</code> of strings that, in order,
 *   comprise the original message
 * @param sentIntents if not NULL, an <code>ArrayList</code> of
 *   <code>PendingIntent</code>s (one for each message part) that is
 *   broadcast when the corresponding message part has been sent.
 *   The result code will be <code>Activity.RESULT_OK<code> for success,
 *   or one of these errors:
 *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
 *   <code>RESULT_ERROR_RADIO_OFF</code>
 *   <code>RESULT_ERROR_NULL_PDU</code>.
 *  The per-application based SMS control checks sentIntent. If sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
 *   <code>PendingIntent</code>s (one for each message part) that is
 *   broadcast when the corresponding message part has been delivered
 *   to the recipient.  The raw pdu of the status report is in the
 *   extended data ("pdu").
 */
ECode IccSmsInterfaceManager::SendMultipartText(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ IList/*<String>*/* parts,
    /* [in] */ IList/*<PendingIntent>*/* sentIntents,
    /* [in] */ IList/*<PendingIntent>*/* deliveryIntents)
{
    Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<ArrayOf<String> > callingParts;
    StringUtils::Split(callingPackage, String("\\\\"), (ArrayOf<String>**)&callingParts);
    if (callingUid == IProcess::PHONE_UID && callingParts->GetLength() > 1) {
        StringUtils::Parse((*callingParts)[1], &callingUid);
    }

    // Reset the calling package, remove the trailing uid so
    // shouldWriteMessageForPackage can match correctly
    // if our message has been synthesized by an
    // external package
    String _callingPackage = (*callingParts)[0];

    if (Binder::GetCallingPid() != Process::MyPid()) {
        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        ctx->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                String("Sending SMS message"));
    }
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        Int32 i = 0;
        AutoPtr<IIterator> itr;
        parts->GetIterator((IIterator**)&itr);
        Boolean hasNext;
        String part;
        while (itr->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            itr->GetNext((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&part);
            Log(String("sendMultipartText: destAddr=") + destAddr + ", srAddr=" + scAddr +
                    ", part[" + StringUtils::ToString(i++) + "]=" + part);
        }
    }

    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, callingUid, (*callingParts)[0], &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    Int32 size = 0;
    parts->GetSize(&size);
    if (size > 1 && size < 10 && !SmsMessage::HasEmsSupport()) {
        for (Int32 i = 0; i < size; i++) {
            // If EMS is not supported, we have to break down EMS into single segment SMS
            // and add page info " x/y".
            AutoPtr<IInterface> cs;
            parts->Get(i, (IInterface**)&cs);
            String singlePart;
            ICharSequence::Probe(cs)->ToString(&singlePart);
            if (SmsMessage::ShouldAppendPageNumberAsPrefix()) {
                singlePart = StringUtils::ToString(i + 1) + String("/") +
                    StringUtils::ToString(size) + " " + singlePart;
            }
            else {
                singlePart += String(" ") + StringUtils::ToString(i + 1) + "/"
                        + StringUtils::ToString(size);
            }

            AutoPtr<IPendingIntent> singleSentIntent;
            Int32 v = 0;
            if (sentIntents != NULL && (sentIntents->GetSize(&v), v) > i) {
                AutoPtr<IInterface> oo;
                sentIntents->Get(i, (IInterface**)&oo);
                singleSentIntent = IPendingIntent::Probe(oo);
            }

            AutoPtr<IPendingIntent> singleDeliveryIntent;
            if (deliveryIntents != NULL && (deliveryIntents->GetSize(&v), v) > i) {
                AutoPtr<IInterface> oo;
                deliveryIntents->Get(i, (IInterface**)&oo);
                singleDeliveryIntent = IPendingIntent::Probe(oo);
            }

            ((SMSDispatcher*)mDispatcher.Get())->SendText(destAddr, scAddr, singlePart,
                    singleSentIntent, singleDeliveryIntent,
                    NULL/*messageUri*/, _callingPackage, -1, FALSE, -1);
        }
        return NOERROR;
    }

    ((SMSDispatcher*)mDispatcher.Get())->SendMultipartText(destAddr, scAddr, IArrayList::Probe(parts),
            IArrayList::Probe(sentIntents), IArrayList::Probe(deliveryIntents),
            NULL/*messageUri*/, _callingPackage, -1, FALSE, -1);
    return NOERROR;
}

/**
 * Send a multi-part text based SMS with Messaging Options.
 *
 * @param destAddr the address to send the message to
 * @param scAddr is the service center address or NULL to use
 *   the current default SMSC
 * @param parts an <code>ArrayList</code> of strings that, in order,
 *   comprise the original message
 * @param sentIntents if not NULL, an <code>ArrayList</code> of
 *   <code>PendingIntent</code>s (one for each message part) that is
 *   broadcast when the corresponding message part has been sent.
 *   The result code will be <code>Activity.RESULT_OK<code> for success,
 *   or one of these errors:
 *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
 *   <code>RESULT_ERROR_RADIO_OFF</code>
 *   <code>RESULT_ERROR_NULL_PDU</code>.
 *  The per-application based SMS control checks sentIntent. If sentIntent
 *  is NULL the caller will be checked against all unknown applications,
 *  which cause smaller number of SMS to be sent in checking period.
 * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
 *   <code>PendingIntent</code>s (one for each message part) that is
 *   broadcast when the corresponding message part has been delivered
 *   to the recipient.  The raw pdu of the status report is in the
 *   extended data ("pdu").
 * @param priority Priority level of the message
 *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
 *  ---------------------------------
 *  PRIORITY      | Level of Priority
 *  ---------------------------------
 *      '00'      |     Normal
 *      '01'      |     Interactive
 *      '10'      |     Urgent
 *      '11'      |     Emergency
 *  ----------------------------------
 *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
 * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
 * @param validityPeriod Validity Period of the message in mins.
 *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
 *  Validity Period(Minimum) -> 5 mins
 *  Validity Period(Maximum) -> 635040 mins(i.e.63 weeks).
 *  Any Other values included Negative considered as Invalid Validity Period of the message.
 */
ECode IccSmsInterfaceManager::SendMultipartTextWithOptions(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& destAddr,
    /* [in] */ const String& scAddr,
    /* [in] */ IList/*<String>*/* parts,
    /* [in] */ IList/*<PendingIntent>*/* sentIntents,
    /* [in] */ IList/*<PendingIntent>*/* deliveryIntents,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean isExpectMore,
    /* [in] */ Int32 validityPeriod)
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->EnforceCallingPermission(
            Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        Int32 i = 0;
        AutoPtr<IIterator> itr;
        parts->GetIterator((IIterator**)&itr);
        Boolean hasNext;
        String part;
        while (itr->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            itr->GetNext((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&part);
            Log(String("sendMultipartTextWithOptions: destAddr=") + destAddr + ", srAddr=" + scAddr +
                    ", part[" + StringUtils::ToString(i++) + "]=" + part);
        }
    }
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPackage, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    ((SMSDispatcher*)mDispatcher.Get())->SendMultipartText(destAddr,
            scAddr,
            IArrayList::Probe(parts),
            IArrayList::Probe(sentIntents),
            IArrayList::Probe(deliveryIntents),
            NULL, callingPackage, priority, isExpectMore, validityPeriod);
    return NOERROR;
}

ECode IccSmsInterfaceManager::GetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mDispatcher->GetPremiumSmsPermission(packageName, result);
}

ECode IccSmsInterfaceManager::SetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 permission)
{
    mDispatcher->SetPremiumSmsPermission(packageName, permission);
    return NOERROR;
}

/**
 * create SmsRawData lists from all sms record Byte[]
 * Use NULL to indicate "free" record
 *
 * @param messages List of message records from EF_SMS.
 * @return SmsRawData list of all in-used records
 */
AutoPtr<IArrayList> IccSmsInterfaceManager::BuildValidRawData(
    /* [in] */ IArrayList* messages)
{
    Int32 count = 0;
    messages->GetSize(&count);
    AutoPtr<IArrayList> ret;/*<SmsRawData*/
    CArrayList::New(count, (IArrayList**)&ret);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        messages->Get(i, (IInterface**)&obj);
        assert(IArrayOf::Probe(obj));
        AutoPtr<IArrayOf> ba = IArrayOf::Probe(obj);
        obj = NULL;
        ba->Get(0, (IInterface**)&obj);
        Byte b;
        IByte::Probe(obj)->GetValue(&b);
        if (b == SmsManager::STATUS_ON_ICC_FREE) {
            ret->Add(NULL);
        }
        else {
            Int32 size;
            ba->GetLength(&size);
            AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
            for (Int32 j = 0; j < size; j++) {
                obj = NULL;
                ba->Get(j, (IInterface**)&obj);
                IByte::Probe(obj)->GetValue(&b);
                (*data)[j] = b;
            }
            AutoPtr<ISmsRawData> srData;
            CSmsRawData::New(data, (ISmsRawData**)&srData);
            ret->Add(srData);
        }
    }

    return ret;
}

/**
 * Generates an EF_SMS record from status and raw PDU.
 *
 * @param status Message status.  See TS 51.011 10.5.3.
 * @param pdu Raw message PDU.
 * @return Byte array for the record.
 */
AutoPtr<ArrayOf<Byte> > IccSmsInterfaceManager::MakeSmsRecordData(
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* pdu)
{
    AutoPtr<ArrayOf<Byte> > data;
    Int32 t = 0;
    if (IPhoneConstants::PHONE_TYPE_GSM == (IPhone::Probe(mPhone)->GetPhoneType(&t), t)) {
        data = ArrayOf<Byte>::Alloc(IIccConstants::SMS_RECORD_LENGTH);
    }
    else {
        data = ArrayOf<Byte>::Alloc(IIccConstants::CDMA_SMS_RECORD_LENGTH);
    }

    // Status bits for this record.  See TS 51.011 10.5.3
    (*data)[0] = (Byte)(status & 7);

    // System.arraycopy(pdu, 0, data, 1, pdu.length);
    data->Copy(1, pdu, 0, pdu->GetLength());

    // Pad out with 0xFF's.
    Int32 pl = pdu->GetLength();
    Int32 dl = data->GetLength();
    for (Int32 j = pl + 1; j < dl; j++) {
        (*data)[j] = -1;
    }

    return data;
}

ECode IccSmsInterfaceManager::EnableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    return EnableCellBroadcastRange(messageIdentifier, messageIdentifier, result);
}

ECode IccSmsInterfaceManager::DisableCellBroadcast(
    /* [in] */ Int32 messageIdentifier,
    /* [out] */ Boolean* result)
{
    return DisableCellBroadcastRange(messageIdentifier, messageIdentifier, result);
}

ECode IccSmsInterfaceManager::EnableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    Int32 t = 0;
    if (IPhoneConstants::PHONE_TYPE_GSM == (IPhone::Probe(mPhone)->GetPhoneType(&t), t)) {
        return EnableGsmBroadcastRange(startMessageId, endMessageId, result);
    }

    return EnableCdmaBroadcastRange(startMessageId, endMessageId, result);
}

ECode IccSmsInterfaceManager::DisableCellBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    Int32 t = 0;
    if (IPhoneConstants::PHONE_TYPE_GSM == (IPhone::Probe(mPhone)->GetPhoneType(&t), t)) {
        return DisableGsmBroadcastRange(startMessageId, endMessageId, result);
    }

    return DisableCdmaBroadcastRange(startMessageId, endMessageId, result);
}

ECode IccSmsInterfaceManager::EnableGsmBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    AutoLock lock(mSelfLock);
    if (DBG) Log(String("enableGsmBroadcastRange"));

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);

    context->EnforceCallingPermission(
            String("android.permission.RECEIVE_SMS"),
            String("Enabling cell broadcast SMS"));

    AutoPtr<IPackageManager> pm;
    String client;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->GetNameForUid(Binder::GetCallingUid(), &client);

    Boolean tmp = FALSE;
    mCellBroadcastRangeManager->EnableRange(startMessageId, endMessageId, client, &tmp);
    if (!tmp) {
        Log(String("Failed to add cell broadcast subscription for MID range ")
            + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
        *result = FALSE;
        return NOERROR;
    }

    if (DBG)
        Log(String("Added cell broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);

    mCellBroadcastRangeManager->IsEmpty(&tmp);
    SetCellBroadcastActivation(!tmp);

    *result = TRUE;
    return NOERROR;
}

ECode IccSmsInterfaceManager::DisableGsmBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mSelfLock);

    if (DBG) Log(String("disableGsmBroadcastRange"));

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);

    context->EnforceCallingPermission(
            String("android.permission.RECEIVE_SMS"),
            String("Disabling cell broadcast SMS"));

    AutoPtr<IPackageManager> pm;
    String client;
    context->GetPackageManager((IPackageManager**)&pm);
    pm->GetNameForUid(Binder::GetCallingUid(), &client);

    Boolean tmp = FALSE;
    mCellBroadcastRangeManager->DisableRange(startMessageId, endMessageId, client, &tmp);
    if (!tmp) {
        Log(String("Failed to remove cell broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
        *result = FALSE;
        return NOERROR;
    }

    if (DBG) {
        Log(String("Removed cell broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
    }

    mCellBroadcastRangeManager->IsEmpty(&tmp);
    SetCellBroadcastActivation(!tmp);

    *result = TRUE;
    return NOERROR;
}

ECode IccSmsInterfaceManager::EnableCdmaBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mSelfLock);

    if (DBG) Log(String("enableCdmaBroadcastRange"));

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);

    context->EnforceCallingPermission(
            String("android.permission.RECEIVE_SMS"),
            String("Enabling cdma broadcast SMS"));

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    String client;
    pm->GetNameForUid(Binder::GetCallingUid(), &client);

    Boolean tmp = FALSE;
    mCdmaBroadcastRangeManager->EnableRange(startMessageId, endMessageId, client, &tmp);
    if (!tmp) {
        Log(String("Failed to add cdma broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
        *result = FALSE;
        return NOERROR;
    }

    if (DBG) {
        Log(String("Added cdma broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
    }

    mCdmaBroadcastRangeManager->IsEmpty(&tmp);
    SetCdmaBroadcastActivation(!tmp);

    *result = TRUE;
    return NOERROR;
}

ECode IccSmsInterfaceManager::DisableCdmaBroadcastRange(
    /* [in] */ Int32 startMessageId,
    /* [in] */ Int32 endMessageId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mSelfLock);

    if (DBG) Log(String("disableCdmaBroadcastRange"));

    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);

    context->EnforceCallingPermission(
            String("android.permission.RECEIVE_SMS"),
            String("Disabling cell broadcast SMS"));

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    String client;
    pm->GetNameForUid(Binder::GetCallingUid(), &client);

    Boolean tmp = FALSE;
    mCdmaBroadcastRangeManager->DisableRange(startMessageId, endMessageId, client, &tmp);
    if (!tmp) {
        Log(String("Failed to remove cdma broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
        *result = FALSE;
        return NOERROR;
    }

    if (DBG) {
        Log(String("Removed cdma broadcast subscription for MID range ")
                + StringUtils::ToString(startMessageId)
                + " to " + StringUtils::ToString(endMessageId) + " from client " + client);
    }

    mCdmaBroadcastRangeManager->IsEmpty(&tmp);
    SetCdmaBroadcastActivation(!tmp);

    *result = TRUE;
    return NOERROR;
}

Boolean IccSmsInterfaceManager::SetCellBroadcastConfig(
    /* [in] */ ArrayOf<ISmsBroadcastConfigInfo*>* configs)
{
    if (DBG) {
        Log(String("Calling setGsmBroadcastConfig with ")
            + StringUtils::ToString(configs->GetLength()) + " configurations");
    }

    {
        AutoLock lock(mLock);
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_SET_BROADCAST_CONFIG_DONE, (IMessage**)&response);

        mSuccess = FALSE;
        ((PhoneBase*)mPhone.Get())->mCi->SetGsmBroadcastConfig(configs, response);

        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to set cell broadcast config"));
        }
    }

    return mSuccess;
}

Boolean IccSmsInterfaceManager::SetCellBroadcastActivation(
    /* [in] */ Boolean activate)
{
    if (DBG) {
        Log(String("Calling SetCellBroadcastActivation(") + StringUtils::ToString(activate) + ")");
    }

    {
        AutoLock lock(mLock);
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_SET_BROADCAST_ACTIVATION_DONE, (IMessage**)&response);

        mSuccess = FALSE;
        ((PhoneBase*)mPhone.Get())->mCi->SetGsmBroadcastActivation(activate, response);

        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to set cell broadcast activation"));
        }
    }

    return mSuccess;
}

Boolean IccSmsInterfaceManager::SetCdmaBroadcastConfig(
    /* [in] */ ArrayOf<ICdmaSmsBroadcastConfigInfo*>* configs)
{
    if (DBG) {
        Log(String("Calling setCdmaBroadcastConfig with ")
            + StringUtils::ToString(configs->GetLength()) + " configurations");
    }

    {
        AutoLock lock(mLock);
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_SET_BROADCAST_CONFIG_DONE, (IMessage**)&response);

        mSuccess = FALSE;
        ((PhoneBase*)mPhone.Get())->mCi->SetCdmaBroadcastConfig(configs, response);

        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to set cdma broadcast config"));
        }
    }

    return mSuccess;
}

Boolean IccSmsInterfaceManager::SetCdmaBroadcastActivation(
    /* [in] */ Boolean activate)
{
    if (DBG) {
        Log(String("Calling SetCdmaBroadcastActivation(")
            + StringUtils::ToString(activate) + ")");
    }

    {
        AutoLock lock(mLock);
        AutoPtr<IMessage> response;
        mHandler->ObtainMessage(EVENT_SET_BROADCAST_ACTIVATION_DONE, (IMessage**)&response);

        mSuccess = FALSE;
        ((PhoneBase*)mPhone.Get())->mCi->SetCdmaBroadcastActivation(activate, response);

        if (FAILED(mLock.Wait())) {
            Log(String("interrupted while trying to set cdma broadcast activation"));
        }
    }

    return mSuccess;
}

void IccSmsInterfaceManager::Log(
    /* [in] */ const String& msg) {
    Logger::D(TAG, "[IccSmsInterfaceManager] %s", msg.string());
}

ECode IccSmsInterfaceManager::IsImsSmsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mDispatcher->IsIms(result);
}

ECode IccSmsInterfaceManager::GetImsSmsFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mDispatcher->GetImsSmsFormat(result);
}

ECode IccSmsInterfaceManager::SendStoredText(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IPendingIntent* sentIntent,
    /* [in] */ IPendingIntent* deliveryIntent)
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->EnforceCallingPermission(Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    if (Logger::IsLoggable("SMS", Logger::VERBOSE)) {
        Log(String("sendStoredText: scAddr=") + scAddress + " messageUri=" + TO_CSTR(messageUri)
                + " sentIntent=" + TO_CSTR(sentIntent) + " deliveryIntent=" + TO_CSTR(deliveryIntent));
    }
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPkg, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    AutoPtr<IContentResolver> resolver;
    ctx->GetContentResolver((IContentResolver**)&resolver);
    if (!IsFailedOrDraft(resolver, messageUri)) {
        Logger::E(TAG, "[IccSmsInterfaceManager]sendStoredText: not FAILED or DRAFT message");
        ReturnUnspecifiedFailure(sentIntent);
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > textAndAddress = LoadTextAndAddress(resolver, messageUri);
    if (textAndAddress == NULL) {
        Logger::E(TAG, "[IccSmsInterfaceManager]sendStoredText: can not load text");
        ReturnUnspecifiedFailure(sentIntent);
        return NOERROR;
    }
    (*textAndAddress)[1] = FilterDestAddress((*textAndAddress)[1]);
    ((SMSDispatcher*)mDispatcher.Get())->SendText((*textAndAddress)[1], scAddress, (*textAndAddress)[0],
            sentIntent, deliveryIntent, messageUri, callingPkg, -1, FALSE, -1);
    return NOERROR;
}

ECode IccSmsInterfaceManager::SendStoredMultipartText(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [in] */ const String& scAddress,
    /* [in] */ IList/*<PendingIntent>*/* sentIntents,
    /* [in] */ IList/*<PendingIntent>*/* deliveryIntents)
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->EnforceCallingPermission(Manifest::permission::SEND_SMS,
            String("Sending SMS message"));
    Int32 v = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPkg, &v);
    if (v != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    AutoPtr<IContentResolver> resolver;
    ctx->GetContentResolver((IContentResolver**)&resolver);
    if (!IsFailedOrDraft(resolver, messageUri)) {
        Logger::E(TAG, "[IccSmsInterfaceManager]sendStoredMultipartText: not FAILED or DRAFT message");
        ReturnUnspecifiedFailure(sentIntents);
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > textAndAddress = LoadTextAndAddress(resolver, messageUri);
    if (textAndAddress == NULL) {
        Logger::E(TAG, "[IccSmsInterfaceManager]sendStoredMultipartText: can not load text");
        ReturnUnspecifiedFailure(sentIntents);
        return NOERROR;
    }

    AutoPtr<ISmsManager> sm = SmsManager::GetDefault();
    AutoPtr<IArrayList> parts;
    sm->DivideMessage((*textAndAddress)[0], (IArrayList**)&parts);
    Int32 size = 0;
    if (parts == NULL || (parts->GetSize(&size), size) < 1) {
        Logger::E(TAG, "[IccSmsInterfaceManager]sendStoredMultipartText: can not divide text");
        ReturnUnspecifiedFailure(sentIntents);
        return NOERROR;
    }

    if (size > 1 && size < 10 && !SmsMessage::HasEmsSupport()) {
        for (Int32 i = 0; i < size; i++) {
            // If EMS is not supported, we have to break down EMS into single segment SMS
            // and add page info " x/y".
            AutoPtr<IInterface> obj;
            parts->Get(i, (IInterface**)&obj);
            String singlePart;
            ICharSequence::Probe(obj)->ToString(&singlePart);
            if (SmsMessage::ShouldAppendPageNumberAsPrefix()) {
                singlePart = StringUtils::ToString(i + 1) + "/"
                    + StringUtils::ToString(size) + " " + singlePart;
            }
            else {
                singlePart += String(" ") + StringUtils::ToString(i + 1) + "/" + StringUtils::ToString(size);
            }

            AutoPtr<IPendingIntent> singleSentIntent;
            if (sentIntents != NULL && (sentIntents->GetSize(&v), v) > i) {
                AutoPtr<IInterface> oo;
                sentIntents->Get(i, (IInterface**)&oo);
                singleSentIntent = IPendingIntent::Probe(oo);
            }

            AutoPtr<IPendingIntent> singleDeliveryIntent;
            if (deliveryIntents != NULL && (deliveryIntents->GetSize(&v), v) > i) {
                AutoPtr<IInterface> oo;
                deliveryIntents->Get(i, (IInterface**)&oo);
                singleDeliveryIntent = IPendingIntent::Probe(oo);
            }

            ((SMSDispatcher*)mDispatcher.Get())->SendText((*textAndAddress)[1], scAddress, singlePart,
                    singleSentIntent, singleDeliveryIntent, messageUri, callingPkg,
                    -1, FALSE, -1);
        }
        return NOERROR;
    }

    (*textAndAddress)[1] = FilterDestAddress((*textAndAddress)[1]);
    ((SMSDispatcher*)mDispatcher.Get())->SendMultipartText(
            (*textAndAddress)[1], // destAddress
            scAddress,
            parts,
            IArrayList::Probe(sentIntents),
            IArrayList::Probe(deliveryIntents),
            messageUri,
            callingPkg, -1, FALSE, -1);
    return NOERROR;
}

Boolean IccSmsInterfaceManager::IsFailedOrDraft(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* messageUri)
{
    // Clear the calling identity and query the database using the phone user id
    // Otherwise the AppOps check in TelephonyProvider would complain about mismatch
    // between the calling uid and the package uid
    const Int64 identity = Binder::ClearCallingIdentity();
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(1);
    (*vs)[0] = ITelephonyTextBasedSmsColumns::TYPE;
    resolver->Query(
            messageUri,
            vs,
            String(NULL)/*selection*/,
            NULL/*selectionArgs*/,
            String(NULL)/*sortOrder*/,
            (ICursor**)&cursor);
    if (cursor != NULL) {
        Boolean tmp = FALSE;
        if (FAILED(cursor->MoveToFirst(&tmp))) {
            Logger::E(TAG, "[IccSmsInterfaceManager]IsFailedOrDraft: query message type failed");
        }
        else {
            if (tmp) {
                Int32 type = 0;
                if (FAILED(cursor->GetInt32(0, &type))) {
                    Logger::E(TAG, "[IccSmsInterfaceManager]IsFailedOrDraft: query message type failed");
                }
                else {
                    if (cursor != NULL) {
                        ICloseable::Probe(cursor)->Close();
                    }
                    Binder::RestoreCallingIdentity(identity);
                    return type == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_DRAFT
                            || type == ITelephonyTextBasedSmsColumns::MESSAGE_TYPE_FAILED;
                }
            }
        }
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    Binder::RestoreCallingIdentity(identity);
    return FALSE;
}

// Return an array including both the SMS text (0) and address (1)
AutoPtr<ArrayOf<String> > IccSmsInterfaceManager::LoadTextAndAddress(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* messageUri)
{
    // Clear the calling identity and query the database using the phone user id
    // Otherwise the AppOps check in TelephonyProvider would complain about mismatch
    // between the calling uid and the package uid
    const Int64 identity = Binder::ClearCallingIdentity();
    AutoPtr<ICursor> cursor;
    AutoPtr<ArrayOf<String> > vs = ArrayOf<String>::Alloc(2);
    (*vs)[0] = ITelephonyTextBasedSmsColumns::BODY;
    (*vs)[1] = ITelephonyTextBasedSmsColumns::ADDRESS;
    resolver->Query(
            messageUri,
            vs,
            String(NULL)/*selection*/,
            NULL/*selectionArgs*/,
            String(NULL)/*sortOrder*/,
            (ICursor**)&cursor);

    if (cursor != NULL) {
        Boolean tmp = FALSE;
        if (FAILED(cursor->MoveToFirst(&tmp))) {
            Logger::E(TAG, "[IccSmsInterfaceManager]loadText: query message text failed");
        }
        else {
            if (tmp) {
                if (cursor != NULL) {
                    ICloseable::Probe(cursor)->Close();
                }
                Binder::RestoreCallingIdentity(identity);
                String s;
                cursor->GetString(0, &s);
                (*vs)[0] = s;
                cursor->GetString(1, &s);
                (*vs)[1] = s;
                return vs;
            }
        }
    }

    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
    Binder::RestoreCallingIdentity(identity);
    return NULL;
}

void IccSmsInterfaceManager::ReturnUnspecifiedFailure(
    /* [in] */ IPendingIntent* pi)
{
    if (pi != NULL) {
        // try {
        pi->Send(ISmsManager::RESULT_ERROR_GENERIC_FAILURE);
        // } catch (PendingIntent.CanceledException e) {
        //     // ignore
        // }
    }
}

void IccSmsInterfaceManager::ReturnUnspecifiedFailure(
    /* [in] */ IList/*<PendingIntent>*/* pis)
{
    if (pis == NULL) {
        return;
    }

    AutoPtr<IIterator> itr;
    pis->GetIterator((IIterator**)&itr);
    Boolean hasNext;
    while (itr->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> pi;
        itr->GetNext((IInterface**)&pi);
        ReturnUnspecifiedFailure(IPendingIntent::Probe(pi));
    }
}

ECode IccSmsInterfaceManager::EnforceCarrierPrivilege()
{
    AutoPtr<IUiccController> controller = UiccController::GetInstance();
    AutoPtr<IUiccCard> uc;
    if (controller == NULL || (controller->GetUiccCard((IUiccCard**)&uc), uc) == NULL) {
        // throw new SecurityException("No Carrier Privilege: No UICC");
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 v = 0;
    uc->GetCarrierPrivilegeStatusForCurrentTransaction(pm, &v);
    if (v != ITelephonyManager::CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
        // throw new SecurityException("No Carrier Privilege.");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

String IccSmsInterfaceManager::FilterDestAddress(
    /* [in] */ const String& destAddr)
{
    String result;
    AutoPtr<ISmsNumberUtils> utils;
    CSmsNumberUtils::AcquireSingleton((ISmsNumberUtils**)&utils);
    utils->FilterDestAddr(mPhone, destAddr, &result);
    return (!result.IsNull()) ? result : destAddr;
}

ECode IccSmsInterfaceManager::GetSmsCapacityOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 numberOnIcc = -1;
    AutoPtr<IIccRecords> ir;
    mPhone->GetIccRecords((IIccRecords**)&ir);

    if (ir != NULL) {
        ir->GetSmsCapacityOnIcc(&numberOnIcc);
    }
    else {
        Log(String("getSmsCapacityOnIcc - aborting, no icc card present."));
    }

    Log(String("getSmsCapacityOnIcc().numberOnIcc = ") + StringUtils::ToString(numberOnIcc));
    *result = numberOnIcc;
    return NOERROR;
}

/** @hide **/
ECode IccSmsInterfaceManager::IsShortSMSCode(
    /* [in] */ const String& destAddr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ITelephonyManager> telephonyManager;
    Int32 smsCategory = SmsUsageMonitor::CATEGORY_NOT_SHORT_CODE;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    telephonyManager = ITelephonyManager::Probe(obj);

    String countryIso;
    telephonyManager->GetSimCountryIso(&countryIso);
    if (countryIso.IsNull() || countryIso.GetLength() != 2) {
        telephonyManager->GetNetworkCountryIso(&countryIso);
    }

    Int32 val = 0;
    if (((PhoneBase*)mPhone.Get())->mSmsUsageMonitor != NULL) {
        ((PhoneBase*)mPhone.Get())->mSmsUsageMonitor->CheckDestination(destAddr, countryIso, &val);
    }
    else {
        Logger::D(TAG, "[TODO] ((PhoneBase*)mPhone.Get())->mSmsUsageMonitor = NULL");
    }
    SmsUsageMonitor::MergeShortCodeCategories(smsCategory, val, &smsCategory);

    if (smsCategory == SmsUsageMonitor::CATEGORY_NOT_SHORT_CODE
            || smsCategory == SmsUsageMonitor::CATEGORY_FREE_SHORT_CODE
            || smsCategory == SmsUsageMonitor::CATEGORY_STANDARD_SHORT_CODE) {
        *result = FALSE;    // not a premium short code
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
