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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/internal/telephony/RIL.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/internal/telephony/CSmsResponse.h"
#include "elastos/droid/internal/telephony/CTelephonyDevControllerHelper.h"
#include "elastos/droid/telephony/CSmsMessageHelper.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/gsm/CSuppServiceNotification.h"
#include "elastos/droid/internal/telephony/uicc/CIccIoResult.h"
#include "elastos/droid/internal/telephony/uicc/CIccCardStatus.h"
#include "elastos/droid/internal/telephony/uicc/IccCardApplicationStatus.h"
#include "elastos/droid/internal/telephony/uicc/IccRefreshResponse.h"
#include "elastos/droid/internal/telephony/DriverCall.h"
#include "elastos/droid/internal/telephony/UUSInfo.h"
#include "elastos/droid/internal/telephony/TelephonyDevController.h"
#include "elastos/droid/net/CLocalSocket.h"
#include "elastos/droid/net/CLocalSocketAddress.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/telephony/CPhoneNumberUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CBase64.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Internal::Telephony::CallForwardInfo;
using Elastos::Droid::Internal::Telephony::IOperatorInfo;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaCallWaitingNotification;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaSignalInfoRec;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaNumberInfoRec;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaRedirectingNumberInfoRec;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaLineControlInfoRec;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaT53ClirInfoRec;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecordsCdmaT53AudioControlInfoRec;
using Elastos::Droid::Internal::Telephony::DataConnection::IApnProfileOmh;
using Elastos::Droid::Internal::Telephony::DataConnection::IApnSetting;
using Elastos::Droid::Internal::Telephony::Gsm::CSuppServiceNotification;
using Elastos::Droid::Internal::Telephony::Gsm::ISsData;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;
using Elastos::Droid::Internal::Telephony::Uicc::CIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::CIccCardStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardApplicationStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRefreshResponse;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IccRefreshResponse;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Telephony::ISmsMessageHelper;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::INeighboringCellInfo;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Telephony::CSmsMessageHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::IBase64;

using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CThrowable;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::Thread;
using Elastos::Core::StringBuilder;
using Elastos::Core::Math;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringUtils;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::IO::IOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::IBuffer;
using Elastos::Utility::CRandom;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  RILRequest::
//==============================================================

// const String RILRequest::LOG_TAG("RilRequest");

static AutoPtr<IRandom> InitRandom()
{
    AutoPtr<IRandom> r;
    CRandom::New((IRandom**)&r);
    return r;
}
AutoPtr<IRandom> RILRequest::sRandom = InitRandom();

static AutoPtr<IAtomicInteger32> InitAtomic()
{
    AutoPtr<IAtomicInteger32> a;
    CAtomicInteger32::New(0, (IAtomicInteger32**)&a);
    return a;
}

AutoPtr<IAtomicInteger32> RILRequest::sNextSerial = InitAtomic();
Object RILRequest::sPoolSync; // = new Object();
AutoPtr<RILRequest> RILRequest::sPool;
Int32 RILRequest::sPoolSize = 0;
Int32 RILRequest::MAX_POOL_SIZE = 4;

AutoPtr<RILRequest> RILRequest::Obtain(
    /* [in] */ Int32 request,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr;

    // synchronized(sPoolSync)
    {
        AutoLock lock(sPoolSync);
        if (sPool != NULL) {
            rr = sPool;
            sPool = rr->mNext;
            rr->mNext = NULL;
            sPoolSize--;
        }
    }

    if (rr == NULL) {
        rr = new RILRequest();
    }

    sNextSerial->GetAndIncrement(&(rr->mSerial));

    rr->mRequest = request;
    rr->mResult = result;
    rr->mParcel = new RILParcel();//Parcel::Obtain();
    //CParcel::New((RILParcel**)&(rr->mParcel));

    if (result != NULL) {
        AutoPtr<IHandler> hdl;
        result->GetTarget((IHandler**)&hdl);
        if (hdl == NULL) {
            Logger::E("RIL", "line:%d, Message target must not be NULL\n", __LINE__);
            // throw new NullPointerException("Message target must not be NULL");
            return NULL;
        }
    }

    // first elements in any RIL Parcel
    rr->mParcel->WriteInt32(request);
    rr->mParcel->WriteInt32(rr->mSerial);

    //Logger::E("leliang", "line:%d, func:%s, request:%d, serial:%d\n", __LINE__, __func__, request, rr->mSerial);
    return rr;
}

UInt32 RILRequest::ReleaseRequest()
{
    // synchronized (sPoolSync)
    {
        AutoLock lock(sPoolSync);
        if (sPoolSize < MAX_POOL_SIZE) {
            mNext = sPool;
            sPool = this;
            sPoolSize++;
            mResult = NULL;
        }
    }
    return 0;
}

RILRequest::RILRequest()
{
}

void RILRequest::ResetSerial()
{
    // use a random so that on recovery we probably don't mix old requests
    // with new.
    Int32 num = 0;
    sRandom->NextInt32(&num);
    sNextSerial->Set(num);
}

String RILRequest::SerialString()
{
    //Cheesy way to do %04d
    StringBuilder sb;
    String sn;

    Int64 adjustedSerial = (((Int64)mSerial) - Elastos::Core::Math::INT32_MIN_VALUE)%10000;

    sn = StringUtils::ToString(adjustedSerial);

    sb.AppendChar('[');
    for (Int32 i = 0, s = sn.GetLength() ; i < 4 - s; i++) {
        sb.AppendChar('0');
    }

    sb.Append(sn);
    sb.AppendChar(']');
    return sb.ToString();
}

void RILRequest::OnError(
    /* [in] */ Int32 error,
    /* [in] */ IInterface* ret)
{
    //TODO AutoPtr<ICommandException> ex;
    Logger::E("RILJ", "TODO RILRequest::OnError");
    AutoPtr<IThrowable> ex;
    CThrowable::New(String("RIL::OnError"), (IThrowable**)&ex);

    //assert(0 && "TODO");
    // ex = CommandException::FromRilErrno(error);

    // if (RIL::RILJ_LOGD) {
    //     Rlog.d(LOG_TAG, serialString() + "< "
    //     + RIL.requestToString(mRequest)
    //     + " error: " + ex + " ret=" + RIL.retToString(mRequest, ret));
    // }

    if (mResult != NULL) {
        AsyncResult::ForMessage(mResult, ret, IThrowable::Probe(ex));
        mResult->SendToTarget();
    }

    if (mParcel != NULL) {
        //assert(0 && "TODO");
        // mParcel->Recycle();
        mParcel = NULL;
    }
}

const Int32 RIL::EVENT_SEND                 = 1;
const Int32 RIL::EVENT_WAKE_LOCK_TIMEOUT    = 2;
//==============================================================
//  RIL::RILSender::
//==============================================================
CAR_INTERFACE_IMPL(RIL::RILSender, Handler, IRunnable)

RIL::RILSender::RILSender(
    /* [in] */ ILooper* looper,
    /* [in] */ RIL* host)
    : Handler(looper)
    , mHost(host)
{
    mDataLength = ArrayOf<Byte>::Alloc(4);
}

ECode RIL::RILSender::Run()
{
    //setup if needed
    return NOERROR;
}

ECode RIL::RILSender::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<RILRequest> rr = (RILRequest*)IObject::Probe(obj);
    AutoPtr<RILRequest> req;

    Int32 what = 0;
    msg->GetWhat(&what);
    //Logger::E("leliang", "line:%d, func:%s, what:%d\n", __LINE__, __func__, what);
    switch (what) {
        case EVENT_SEND: {
            // try {
                AutoPtr<ILocalSocket> s;

                s = mHost->mSocket;

                if (s == NULL) {
                    rr->OnError(IRILConstants::RADIO_NOT_AVAILABLE, NULL);
                    rr->ReleaseRequest();
                    mHost->DecrementWakeLock();
                    return NOERROR;
                }

                {
                    AutoLock lock(mHost->mRequestList);
                    mHost->mRequestList->Append(rr->mSerial, (IInterface*)(IObject*)rr.Get());
                }

                AutoPtr<ArrayOf<Byte> > data;

                rr->mParcel->Marshall((ArrayOf<Byte>**)&data);
                // rr->mParcel->Recycle();
                rr->mParcel = NULL;

                Logger::E("RILJ", "writing packet: %d bytes, mSerial:%d", data->GetLength(), rr->mSerial);
                if (data->GetLength() > RIL_MAX_COMMAND_BYTES) {
                    // throw new RuntimeException(
                    //         "Parcel larger than max bytes allowed! "
                    //                               + data.length);
                    return E_RUNTIME_EXCEPTION;
                }

                // parcel length in big endian
                (*mDataLength)[0] = (*mDataLength)[1] = 0;
                (*mDataLength)[2] = (Byte)((data->GetLength() >> 8) & 0xff);
                (*mDataLength)[3] = (Byte)((data->GetLength()) & 0xff);

                //Rlog.v(RILJ_LOG_TAG, "writing packet: " + data.length + " bytes");

                AutoPtr<IOutputStream> os;
                s->GetOutputStream((IOutputStream**)&os);
                os->Write(mDataLength);
                os->Write(data);
            // } catch (IOException ex) {
            //     Rlog.e(RILJ_LOG_TAG, "IOException", ex);
            //     req = findAndRemoveRequestFromList(rr.mSerial);
            //     // make sure this request has not already been handled,
            //     // eg, if RILReceiver cleared the list.
            //     if (req != NULL) {
            //         rr.onError(IRILConstants::RADIO_NOT_AVAILABLE, NULL);
            //         rr.release();
            //         decrementWakeLock();
            //     }
            // } catch (RuntimeException exc) {
            //     Rlog.e(RILJ_LOG_TAG, "Uncaught exception ", exc);
            //     req = findAndRemoveRequestFromList(rr.mSerial);
            //     // make sure this request has not already been handled,
            //     // eg, if RILReceiver cleared the list.
            //     if (req != NULL) {
            //         rr.onError(GENERIC_FAILURE, NULL);
            //         rr.release();
            //         decrementWakeLock();
            //     }
            // }
        }
        break;
        case EVENT_WAKE_LOCK_TIMEOUT: {
            Logger::E("RILJ", "EVENT_WAKE_LOCK_TIMEOUT");
            // Haven't heard back from the last request.  Assume we're
            // not getting a response and  release the wake lock.

            // The timer of WAKE_LOCK_TIMEOUT is reset with each
            // new Send request. So when WAKE_LOCK_TIMEOUT occurs
            // all requests in mRequestList already waited at
            // least DEFAULT_WAKE_LOCK_TIMEOUT but no response.
            //
            // Note: Keep mRequestList so that delayed response
            // can still be handled when response finally comes.

            // synchronized (mRequestList)
            {
                AutoLock lock(mHost->mRequestList);
                if (mHost->ClearWakeLock()) {
                    if (RILJ_LOGD) {
                        Int32 count = 0;
                        mHost->mRequestList->GetSize(&count);
                        // Rlog.d(RILJ_LOG_TAG, "WAKE_LOCK_TIMEOUT " +
                        //         " mRequestList=" + count);
                        for (Int32 i = 0; i < count; i++) {
                            AutoPtr<IInterface> _rr;
                            mHost->mRequestList->ValueAt(i, (IInterface**)&_rr);
                            rr = (RILRequest*)IObject::Probe(_rr);
                            // Rlog.d(RILJ_LOG_TAG, i + ": [" + rr.mSerial + "] "
                            //         + requestToString(rr.mRequest));
                        }
                    }
                }
            }
        }
        break;
    }
    return NOERROR;
}

//==============================================================
//  RIL::RILReceiver::
//==============================================================
CAR_INTERFACE_IMPL(RIL::RILReceiver, Object, IRunnable)

RIL::RILReceiver::RILReceiver(
    /* [in] */ RIL* host)
    : mHost(host)
{
    mBuffer = ArrayOf<Byte>::Alloc(RIL_MAX_COMMAND_BYTES);
}

ECode RIL::RILReceiver::Run()
{
    Int32 retryCount = 0;
    String rilSocket("rild");

    // try {
    for (;;) {
        AutoPtr<ILocalSocket> s;
        AutoPtr<ILocalSocketAddress> l;

        if (mHost->mInstanceId == NULL) {
            rilSocket = (*SOCKET_NAME_RIL)[0];
        }
        else {
            Int32 id = 0;
            mHost->mInstanceId->GetValue(&id);
            rilSocket = (*SOCKET_NAME_RIL)[id];
        }

        CLocalSocket::New((ILocalSocket**)&s);
        CLocalSocketAddress::New(rilSocket,
                LocalSocketAddressNamespace_RESERVED, (ILocalSocketAddress**)&l);
        if (FAILED(s->Connect(l))) {
            if (s != NULL) {
                ICloseable::Probe(s)->Close();
            }

            // don't print an error message after the the first time
            // or after the 8th time
            if (retryCount == 8) {
                Logger::E(RILJ_LOG_TAG,
                    "Couldn't find '%s' socket after %d times, continuing to retry silently"
                    , rilSocket.string(), retryCount);
            }
            else if (retryCount > 0 && retryCount < 8) {
                Logger::I(RILJ_LOG_TAG, "Couldn't find '%s' socket; retrying after timeout", rilSocket.string());
            }

            Thread::Sleep(SOCKET_OPEN_RETRY_MILLIS);

            retryCount++;
            continue;
        }

        retryCount = 0;

        mHost->mSocket = s;
        // Rlog.i(RILJ_LOG_TAG, "Connected to '" + rilSocket + "' socket");

        /* Compatibility with qcom's DSDS (Dual SIM) stack */
        Boolean bNeeds = FALSE;
        mHost->NeedsOldRilFeature(String("qcomdsds"), &bNeeds);
        if (bNeeds) {
            String str("SUB1");
            AutoPtr<ArrayOf<Byte> > data = str.GetBytes();
            // try {
                AutoPtr<IOutputStream> os;
                mHost->mSocket->GetOutputStream((IOutputStream**)&os);
                os->Write(data);
                // Rlog.i(RILJ_LOG_TAG, "Data sent!!");
            // } catch (IOException ex) {
            //         Rlog.e(RILJ_LOG_TAG, "IOException", ex);
            // } catch (RuntimeException exc) {
            //     Rlog.e(RILJ_LOG_TAG, "Uncaught exception ", exc);
            // }
        }

        Int32 length = 0;
        // try {
            AutoPtr<IInputStream> is;
            mHost->mSocket->GetInputStream((IInputStream**)&is);

            for (;;) {
                AutoPtr<RILParcel> p;

                length = ReadRilMessage(is, mBuffer);

                if (length < 0) {
                    // End-of-stream reached
                    break;
                }

                // p = Parcel::Obtain();
                //CParcel::New((RILParcel**)&p);
                p = new RILParcel();
                p->Unmarshall(mBuffer, 0, length);
                p->SetDataPosition(0);

                //Rlog.v(RILJ_LOG_TAG, "Read packet: " + length + " bytes");
                //Logger::V("leliang", "Read packet: %d bytes", length);

                mHost->ProcessResponse(p);
                // p->Recycle();
            }
        // } catch (java.io.IOException ex) {
        //     Rlog.i(RILJ_LOG_TAG, "'" + rilSocket + "' socket closed",
        //           ex);
        // } catch (Throwable tr) {
        //     Rlog.e(RILJ_LOG_TAG, "Uncaught exception read length=" + length +
        //         "Exception:" + tr.toString());
        // }

        // Rlog.i(RILJ_LOG_TAG, "Disconnected from '" + rilSocket
        //       + "' socket");

        mHost->SetRadioState(RADIO_UNAVAILABLE);

        // try {
            ICloseable::Probe(mHost->mSocket)->Close();
        // } catch (IOException ex) {
        // }

        mHost->mSocket = NULL;
        RILRequest::ResetSerial();

        // Clear request list on close
        mHost->ClearRequestList(IRILConstants::RADIO_NOT_AVAILABLE, FALSE);
    }
    // } catch (Throwable tr) {
    //     Rlog.e(RILJ_LOG_TAG,"Uncaught exception", tr);
    // }

    /* We're disconnected so we don't know the ril version */
    mHost->NotifyRegistrantsRilConnectionChanged(-1);
    return NOERROR;
}

//==============================================================
//  RIL::UnsolOemHookBuffer::
//==============================================================

RIL::UnsolOemHookBuffer::UnsolOemHookBuffer(
    /* [in] */ Int32 rilInstance,
    /* [in] */ ArrayOf<Byte>* data)
    : mRilInstance(rilInstance)
    , mData(data)
{
}

ECode RIL::UnsolOemHookBuffer::GetRilInstance(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRilInstance;
    return NOERROR;
}

ECode RIL::UnsolOemHookBuffer::GetUnsolOemHookBuffer(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mData;
    return NOERROR;
}

//==============================================================
//  RIL::DisplayListener::
//==============================================================
CAR_INTERFACE_IMPL(RIL::DisplayListener, Object, IDisplayListener)

RIL::DisplayListener::DisplayListener(
    /* [in] */ RIL* host)
    : mHost(host)
{
}

ECode RIL::DisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

ECode RIL::DisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    return NOERROR;
}

ECode RIL::DisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    if (displayId == IDisplay::DEFAULT_DISPLAY) {
        mHost->UpdateScreenState();
    }
    return NOERROR;
}

//==============================================================
//  RIL::
//==============================================================
String RIL::RILJ_LOG_TAG("RILJ");
Boolean RIL::RILJ_LOGD = TRUE;
Boolean RIL::RILJ_LOGV = FALSE; // STOPSHIP if TRUE

Int32 RIL::DEFAULT_WAKE_LOCK_TIMEOUT = 60000;
Int32 RIL::BYTE_SIZE = 1;

Int32 RIL::OEMHOOK_BASE = 0x80000;

Int32 RIL::OEMHOOK_EVT_HOOK_SET_LOCAL_CALL_HOLD = OEMHOOK_BASE + 13;

Int32 RIL::OEMHOOK_EVT_HOOK_GET_MODEM_CAPABILITY = OEMHOOK_BASE + 35;

Int32 RIL::OEMHOOK_EVT_HOOK_UPDATE_SUB_BINDING = OEMHOOK_BASE + 36;

Int32 RIL::INT_SIZE = 4;
String RIL::OEM_IDENTIFIER("QOEMHOOK");

Int32 RIL::RIL_MAX_COMMAND_BYTES = (8 * 1024);
Int32 RIL::RESPONSE_SOLICITED = 0;
Int32 RIL::RESPONSE_UNSOLICITED = 1;

static AutoPtr<ArrayOf<String> > InitSOCKET_NAME_RIL()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = String("rild");
    (*arr)[1] = String("rild2");
    (*arr)[2] = String("rild3");
    return arr;
}

AutoPtr<ArrayOf<String> > RIL::SOCKET_NAME_RIL = InitSOCKET_NAME_RIL();

Int32 RIL::SOCKET_OPEN_RETRY_MILLIS = 4 * 1000;

Int32 RIL::CDMA_BSI_NO_OF_INTS_STRUCT = 3;

Int32 RIL::CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES = 31;

Int32 RIL::ReadRilMessage(
    /* [in] */ IInputStream* is,
    /* [in] */ ArrayOf<Byte>* buffer)
{
    Int32 countRead = 0;
    Int32 offset = 0;
    Int32 remaining = 0;
    Int32 messageLength = 0;

    // First, read in the length of the message
    offset = 0;
    remaining = 4;
    do {
        is->Read(buffer, offset, remaining, &countRead);

        if (countRead < 0 ) {
            // Rlog.e(RILJ_LOG_TAG, "Hit EOS reading message length");
            return -1;
        }

        offset += countRead;
        remaining -= countRead;
    } while (remaining > 0);

    messageLength = (((*buffer)[0] & 0xff) << 24)
            | (((*buffer)[1] & 0xff) << 16)
            | (((*buffer)[2] & 0xff) << 8)
            | ((*buffer)[3] & 0xff);

    // Then, re-use the buffer and read in the message itself
    offset = 0;
    remaining = messageLength;
    do {
        is->Read(buffer, offset, remaining, &countRead);

        if (countRead < 0 ) {
            // Rlog.e(RILJ_LOG_TAG, "Hit EOS reading message.  messageLength=" + messageLength
            //         + " remaining=" + remaining);
            return -1;
        }

        offset += countRead;
        remaining -= countRead;
    } while (remaining > 0);

    return messageLength;
}

CAR_INTERFACE_IMPL(RIL, BaseCommands, IRIL);

RIL::RIL()
    : OEMHOOK_UNSOL_SIM_REFRESH(OEMHOOK_BASE + 1016)
    , OEMHOOK_UNSOL_WWAN_IWLAN_COEXIST(OEMHOOK_BASE + 1018)
{
}

ECode RIL::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 preferredNetworkType,
    /* [in] */ Int32 cdmaSubscription)
{
    BaseCommands::constructor(context);
    Init(context, preferredNetworkType, cdmaSubscription, NULL);
    return NOERROR;
}

ECode RIL::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 preferredNetworkType,
    /* [in] */ Int32 cdmaSubscription,
    /* [in] */ IInteger32* instanceId)
{
    BaseCommands::constructor(context);
    Init(context, preferredNetworkType, cdmaSubscription, instanceId);
    return NOERROR;
}

ECode RIL::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int32 preferredNetworkType,
    /* [in] */ Int32 cdmaSubscription,
    /* [in] */ IInteger32* instanceId)
{
    mHeaderSize = OEM_IDENTIFIER.GetLength() + 2 * INT_SIZE;
    QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY = OEMHOOK_BASE + 1020;

    mDefaultDisplayState = IDisplay::STATE_UNKNOWN;
    mWakeLockTimeout = 0;

    mWakeLockCount = 0;

    CSparseArray::New((ISparseArray**)&mRequestList);

    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mTestingEmergencyCall);

    mQANElements = 4;
    mDisplayListener = new DisplayListener(this);

    if (RILJ_LOGD) {
        // String str("RIL(context, preferredNetworkType=");
        // str += preferredNetworkType;
        // str += " cdmaSubscription=";
        // str += cdmaSubscription;
        // str += ")";
        // RiljLog(str);
    }

    mContext = context;
    mCdmaSubscription  = cdmaSubscription;
    mPreferredNetworkType = preferredNetworkType;
    mPhoneType = IRILConstants::NO_PHONE;
    mInstanceId = instanceId;

    AutoPtr<IInterface> p;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&p);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(p);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, RILJ_LOG_TAG, (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(FALSE);
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->GetInt32(ITelephonyProperties::PROPERTY_WAKE_LOCK_TIMEOUT,
            DEFAULT_WAKE_LOCK_TIMEOUT, &mWakeLockTimeout);
    mWakeLockCount = 0;

    CHandlerThread::New(String("RILSender"), (IHandlerThread**)&mSenderThread);
    IThread::Probe(mSenderThread)->Start();

    AutoPtr<ILooper> looper;
    mSenderThread->GetLooper((ILooper**)&looper);
    mSender = new RILSender(looper, this);

    AutoPtr<IInterface> cs;
    context->GetSystemService(
            IContext::CONNECTIVITY_SERVICE, (IInterface**)&cs);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(cs);
    Boolean bSupported = FALSE;
    cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &bSupported);
    if (bSupported == FALSE) {
        RiljLog(String("Not starting RILReceiver: wifi-only"));
    }
    else {
        RiljLog(String("Starting RILReceiver"));
        mReceiver = new RILReceiver(this);
        CThread::New(mReceiver, String("RILReceiver"), (IThread**)&mReceiverThread);
        IThread::Probe(mReceiverThread)->Start();

        AutoPtr<IInterface> ds;
        context->GetSystemService(
                IContext::DISPLAY_SERVICE, (IInterface**)&ds);
        AutoPtr<IDisplayManager> dm = IDisplayManager::Probe(ds);
        dm->GetDisplay(IDisplay::DEFAULT_DISPLAY, (IDisplay**)&mDefaultDisplay);
        dm->RegisterDisplayListener(mDisplayListener, NULL);
    }

    AutoPtr<ITelephonyDevControllerHelper> tdcHelper;// = TelephonyDevController::GetInstance();
    CTelephonyDevControllerHelper::AcquireSingleton((ITelephonyDevControllerHelper**)&tdcHelper);
    tdcHelper->RegisterRIL(this);
    return NOERROR;
}

ECode RIL::GetVoiceRadioTechnology(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_VOICE_RADIO_TECH, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}


ECode RIL::GetImsRegistrationState(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_IMS_REGISTRATION_STATE, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }
    Send(rr);
    return NOERROR;
}

ECode RIL::SetOnNITZTime(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    BaseCommands::SetOnNITZTime(h, what, obj);

    // Send the last NITZ time if we have it
    if (mLastNITZTimeInfo != NULL) {
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, mLastNITZTimeInfo, NULL);
        mNITZTimeRegistrant->NotifyRegistrant(ar);
        mLastNITZTimeInfo = NULL;
    }
    return NOERROR;
}

ECode RIL::GetIccCardStatus(
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_SIM_STATUS, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetUiccSubscription(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 appIndex,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 subStatus,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request is also valid for SIM and RUIM (ICC card)
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_UICC_SUBSCRIPTION, result);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " slot: "; str += slotId;
        // str += " appIndex: "; str += appIndex;
        // str += " subId: "; str += subId;
        // str += " subStatus: "; str += subStatus;
        // RiljLog(str);
    }

    rr->mParcel->WriteInt32(slotId);
    rr->mParcel->WriteInt32(appIndex);
    rr->mParcel->WriteInt32(subId);
    rr->mParcel->WriteInt32(subStatus);

    Send(rr);
    return NOERROR;
}

ECode RIL::SetDataAllowed(
    /* [in] */ Boolean allowed,
    /* [in] */ IMessage* result)
{
    if(mRilVersion < 10 && mInstanceId == NULL) {
        if (result != NULL) {
            //AutoPtr<ICommandException> ex;
            // CCommandException::New(
            //     ICommandException::Error::REQUEST_NOT_SUPPORTED, (ICommandException**)&ex);
            Logger::E("RILJ", "TODO RIL::SetDataAllowed");
            AutoPtr<IThrowable> ex;
            CThrowable::New(String("REQUEST_NOT_SUPPORTED"), (IThrowable**)&ex);
            AsyncResult::ForMessage(result, NULL, IThrowable::Probe(ex));
            result->SendToTarget();
        }
        return NOERROR;
    }

    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ALLOW_DATA, result);
    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += allowed;
        // RiljLog(str);
    }

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(allowed ? 1 : 0);
    Send(rr);
    return NOERROR;
}

ECode RIL::GetDataCallProfile(
    /* [in] */ Int32 appType,
    /* [in] */ IMessage* result)
{
    if (mRilVersion < 10) {
        if (result != NULL) {
            //AutoPtr<ICommandException> ex;
            Logger::E("RILJ", "TODO RIL::GetDataCallProfile");
            AutoPtr<IThrowable> ex;
            CThrowable::New(String("REQUEST_NOT_SUPPORTED"), (IThrowable**)&ex);
            // CCommandException::New(
            //     ICommandException::Error::REQUEST_NOT_SUPPORTED, (ICommandException**)&ex);
            AsyncResult::ForMessage(result, NULL, IThrowable::Probe(ex));
            result->SendToTarget();
        }
        return NOERROR;
    }

    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_GET_DATA_CALL_PROFILE,
                                result);

    // count of ints
    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(appType);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " : ";
        // str += appType;
        // RiljLog(str);
    }
    Send(rr);
    return NOERROR;
}

ECode RIL::SupplyIccPin(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* result)
{
    return SupplyIccPinForApp(pin, String(NULL), result);
}

ECode RIL::SupplyIccPinForApp(
    /* [in] */ const String& pin,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ENTER_SIM_PIN, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    rr->mParcel->WriteInt32(oldRil ? 1 : 2);
    rr->mParcel->WriteString(pin);

    if (!oldRil) {
        rr->mParcel->WriteString(aid);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SupplyIccPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* result)
{
    return SupplyIccPukForApp(puk, newPin, String(NULL), result);
}

ECode RIL::SupplyIccPukForApp(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ENTER_SIM_PUK, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    rr->mParcel->WriteInt32(oldRil ? 2 : 3);
    rr->mParcel->WriteString(puk);
    rr->mParcel->WriteString(newPin);

    if (!oldRil) {
        rr->mParcel->WriteString(aid);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SupplyIccPin2(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* result)
{
    return SupplyIccPin2ForApp(pin, String(NULL), result);
}

ECode RIL::SupplyIccPin2ForApp(
    /* [in] */ const String& pin,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ENTER_SIM_PIN2, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    rr->mParcel->WriteInt32(oldRil ? 1 : 2);
    rr->mParcel->WriteString(pin);

    if (!oldRil) {
        rr->mParcel->WriteString(aid);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SupplyIccPuk2(
    /* [in] */ const String& puk2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* result)
{
    return SupplyIccPuk2ForApp(puk2, newPin2, String(NULL), result);
}

ECode RIL::SupplyIccPuk2ForApp(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin2,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ENTER_SIM_PUK2, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    rr->mParcel->WriteInt32(oldRil ? 2 : 3);
    rr->mParcel->WriteString(puk);
    rr->mParcel->WriteString(newPin2);

    if (!oldRil) {
        rr->mParcel->WriteString(aid);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ChangeIccPin(
    /* [in] */ const String& oldPin,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* result)
{
    return ChangeIccPinForApp(oldPin, newPin, String(NULL), result);
}

ECode RIL::ChangeIccPinForApp(
    /* [in] */ const String& oldPin,
    /* [in] */ const String& newPin,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    rr->mParcel->WriteInt32(oldRil ? 2 : 3);
    rr->mParcel->WriteString(oldPin);
    rr->mParcel->WriteString(newPin);

    if (!oldRil) {
        rr->mParcel->WriteString(aid);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ChangeIccPin2(
    /* [in] */ const String& oldPin2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* result)
{
    return ChangeIccPin2ForApp(oldPin2, newPin2, String(NULL), result);
}

ECode RIL::ChangeIccPin2ForApp(
    /* [in] */ const String& oldPin2,
    /* [in] */ const String& newPin2,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN2, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    rr->mParcel->WriteInt32(oldRil ? 2 : 3);
    rr->mParcel->WriteString(oldPin2);
    rr->mParcel->WriteString(newPin2);

    if (!oldRil) {
        rr->mParcel->WriteString(aid);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ChangeBarringPassword(
    /* [in] */ const String& facility,
    /* [in] */ const String& oldPwd,
    /* [in] */ const String& newPwd,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_CHANGE_BARRING_PASSWORD,
                                result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    rr->mParcel->WriteInt32(3);
    rr->mParcel->WriteString(facility);
    rr->mParcel->WriteString(oldPwd);
    rr->mParcel->WriteString(newPwd);

    Send(rr);
    return NOERROR;
}

ECode RIL::SupplyDepersonalization(
    /* [in] */ const String& netpin,
    /* [in] */ const String& type,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE,
                                result);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " Type:";
        // str += type;
        // RiljLog(str);
    }

    rr->mParcel->WriteInt32(2);
    rr->mParcel->WriteString(type);
    rr->mParcel->WriteString(netpin);

    Send(rr);
    return NOERROR;
}

ECode RIL::GetCurrentCalls(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_GET_CURRENT_CALLS,
                                result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetPDPContextList(
    /* [in] */ IMessage* result)
{
    return GetDataCallList(result);
}

ECode RIL::GetDataCallList(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_DATA_CALL_LIST,
                                result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::Dial(
    /* [in] */ const String& address,
    /* [in] */ Int32 clirMode,
    /* [in] */ IMessage* result)
{
    return Dial(address, clirMode, NULL, result);
}

ECode RIL::Dial(
    /* [in] */ const String& address,
    /* [in] */ Int32 clirMode,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ IMessage* result)
{
    //Logger::E("leliang", "line:%d, func:%s, address:%s, uusInfo:%p\n", __LINE__, __func__, address.string(), uusInfo);
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_DIAL,
                                result);

    rr->mParcel->WriteString(address);
    rr->mParcel->WriteInt32(clirMode);

    if (uusInfo == NULL) {
        rr->mParcel->WriteInt32(0); // UUS information is absent
    }
    else {
        rr->mParcel->WriteInt32(1); // UUS information is present
        Int32 type = 0;
        uusInfo->GetType(&type);
        rr->mParcel->WriteInt32(type);
        Int32 dcs = 0;
        uusInfo->GetDcs(&dcs);
        rr->mParcel->WriteInt32(dcs);
        AutoPtr<ArrayOf<Byte> > usdata;
        uusInfo->GetUserData((ArrayOf<Byte>**)&usdata);
        rr->mParcel->WriteByteArray(usdata.Get());
    }

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetIMSI(
    /* [in] */ IMessage* result)
{
    return GetIMSIForApp(String(NULL), result);
}

ECode RIL::GetIMSIForApp(
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_GET_IMSI,
                                result);

    Boolean skipNullAid = FALSE;
    NeedsOldRilFeature(String("skipnullaid"), &skipNullAid);
    Boolean writeAidOnly = FALSE;
    NeedsOldRilFeature(String("writeaidonly"), &writeAidOnly);

    if (!writeAidOnly && (aid != NULL || !skipNullAid)) {
        rr->mParcel->WriteInt32(1);
        rr->mParcel->WriteString(aid);
    }

    if (writeAidOnly) {
        rr->mParcel->WriteString(aid);
    }

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> getIMSI: ";
        str += RequestToString(rr->mRequest);
        str += " aid: ";
        str += aid;
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetIMEI(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_IMEI, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetIMEISV(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_IMEISV, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::HangupConnection(
    /* [in] */ Int32 gsmIndex,
    /* [in] */ IMessage* result)
{
    if (RILJ_LOGD) {
        // String str("hangupConnection: gsmIndex=");
        // str += gsmIndex;
        // RiljLog(str);
    }

    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_HANGUP, result);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += gsmIndex;
        // RiljLog(str);
    }

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(gsmIndex);

    Send(rr);
    return NOERROR;
}

ECode RIL::HangupWaitingOrBackground(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
                                    result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::HangupForegroundResumeBackground(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                    IRILConstants::RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
                                    result);
    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SwitchWaitingOrHoldingAndActive(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                    IRILConstants::RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
                                    result);
    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::Conference(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CONFERENCE, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetPreferredVoicePrivacy(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
            result);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(enable ? 1:0);

    Send(rr);
    return NOERROR;
}

ECode RIL::GetPreferredVoicePrivacy(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
            result);
    Send(rr);
    return NOERROR;
}

ECode RIL::SeparateConnection(
    /* [in] */ Int32 gsmIndex,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SEPARATE_CONNECTION, result);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += gsmIndex;
        // RiljLog(str);
    }

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(gsmIndex);

    Send(rr);
    return NOERROR;
}

ECode RIL::AcceptCall(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ANSWER, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::RejectCall(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_UDUB, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ExplicitCallTransfer(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_EXPLICIT_CALL_TRANSFER, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetLastCallFailCause(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_LAST_CALL_FAIL_CAUSE, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetLastPdpFailCause(
    /* [in] */ IMessage* result)
{
    return GetLastDataCallFailCause(result);
}

ECode RIL::GetLastDataCallFailCause(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetMute(
    /* [in] */ Boolean enableMute,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_MUTE, response);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += enableMute;
        // RiljLog(str);
    }

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(enableMute ? 1 : 0);

    Send(rr);
    return NOERROR;
}

ECode RIL::GetMute(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_MUTE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetSignalStrength(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SIGNAL_STRENGTH, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetVoiceRegistrationState(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_VOICE_REGISTRATION_STATE, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetDataRegistrationState(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_DATA_REGISTRATION_STATE, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetOperator(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_OPERATOR, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetHardwareConfig(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
        IRILConstants::RIL_REQUEST_GET_HARDWARE_CONFIG, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendDtmf(
    /* [in] */ Char32 c,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_DTMF, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    char carray[] = {(char)c};
    String str(carray);
    rr->mParcel->WriteString(str);

    Send(rr);
    return NOERROR;
}

ECode RIL::StartDtmf(
    /* [in] */ Char32 c,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_DTMF_START, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    char carray[] = {(char)c};
    String str(carray);
    rr->mParcel->WriteString(str);

    Send(rr);
    return NOERROR;
}

ECode RIL::StopDtmf(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_DTMF_STOP, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_CDMA_BURST_DTMF, result);

    rr->mParcel->WriteInt32(3);
    rr->mParcel->WriteString(dtmfString);
    rr->mParcel->WriteInt32(on);
    rr->mParcel->WriteInt32(off);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += " : ";
        str += dtmfString;
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

void RIL::ConstructGsmSendSmsRilRequest(
    /* [in] */ RILRequest* rr,
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu)
{
    rr->mParcel->WriteInt32(2);
    rr->mParcel->WriteString(smscPDU);
    rr->mParcel->WriteString(pdu);
}

ECode RIL::SendSMS(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SEND_SMS, result);

    ConstructGsmSendSmsRilRequest(rr, smscPDU, pdu);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendSMSExpectMore(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SEND_SMS_EXPECT_MORE, result);

    ConstructGsmSendSmsRilRequest(rr, smscPDU, pdu);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

void RIL::ConstructCdmaSendSmsRilRequest(
    /* [in] */ RILRequest* rr,
    /* [in] */ ArrayOf<Byte>* pdu)
{
    Int32 address_nbr_of_digits = 0;
    Int32 subaddr_nbr_of_digits = 0;
    Int32 bearerDataLength = 0;
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(pdu, (IByteArrayInputStream**)&bais);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New(IInputStream::Probe(bais), (IDataInputStream**)&dis);

    // try {
        Int32 teleServiceId = 0;
        IInputStream::Probe(dis)->Read(&teleServiceId);
        rr->mParcel->WriteInt32(teleServiceId); //teleServiceId

        Int32 servicePresent = 0;
        IInputStream::Probe(dis)->Read(&servicePresent);
        rr->mParcel->WriteByte((Byte) servicePresent); //servicePresent

        Int32 serviceCategory = 0;
        IInputStream::Probe(dis)->Read(&serviceCategory);
        rr->mParcel->WriteInt32(serviceCategory); //serviceCategory

        Int32 address_digit_mode = 0;
        IInputStream::Probe(dis)->Read(&address_digit_mode);
        rr->mParcel->WriteInt32(address_digit_mode); //address_digit_mode

        Int32 address_nbr_mode = 0;
        IInputStream::Probe(dis)->Read(&address_nbr_mode);
        rr->mParcel->WriteInt32(address_nbr_mode); //address_nbr_mode

        Int32 address_ton = 0;
        IInputStream::Probe(dis)->Read(&address_ton);
        rr->mParcel->WriteInt32(address_ton); //address_ton

        Int32 address_nbr_plan = 0;
        IInputStream::Probe(dis)->Read(&address_nbr_plan);
        rr->mParcel->WriteInt32(address_nbr_plan); //address_nbr_plan

        IInputStream::Probe(dis)->Read(&address_nbr_of_digits);
        rr->mParcel->WriteByte((Byte) address_nbr_of_digits);
        for(Int32 i = 0; i < address_nbr_of_digits; i++) {
            Byte address_orig_bytes = 0;
            IDataInput::Probe(dis)->ReadByte(&address_orig_bytes);
            rr->mParcel->WriteByte(address_orig_bytes); // address_orig_bytes[i]
        }
        Int32 subaddressType = 0;
        IInputStream::Probe(dis)->Read(&subaddressType);
        rr->mParcel->WriteInt32(subaddressType); //subaddressType
        Int32 subaddr_odd = 0;
        IInputStream::Probe(dis)->Read(&subaddr_odd);
        rr->mParcel->WriteByte((Byte) subaddr_odd); //subaddr_odd
        IInputStream::Probe(dis)->Read(&subaddr_nbr_of_digits);
        rr->mParcel->WriteByte((Byte) subaddr_nbr_of_digits);
        for(Int32 i = 0; i < subaddr_nbr_of_digits; i++) {
            Byte subaddr_orig_bytes = 0;
            IDataInput::Probe(dis)->ReadByte(&subaddr_orig_bytes);
            rr->mParcel->WriteByte(subaddr_orig_bytes); //subaddr_orig_bytes[i]
        }

        IInputStream::Probe(dis)->Read(&bearerDataLength);
        rr->mParcel->WriteInt32(bearerDataLength);
        for(Int32 i = 0; i < bearerDataLength; i++) {
            Byte bearerData = 0;
            IDataInput::Probe(dis)->ReadByte(&bearerData);
            rr->mParcel->WriteByte(bearerData); //bearerData[i]
        }
    // }catch (IOException ex){
    //     if (RILJ_LOGD) {
    //         RiljLog("sendSmsCdma: conversion from input stream to object failed: "
    //             + ex);
    //     }
    // }
}

ECode RIL::SendCdmaSms(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_SEND_SMS, result);

    ConstructCdmaSendSmsRilRequest(rr, pdu);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendImsGsmSms(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ Int32 retry,
    /* [in] */ Int32 messageRef,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_IMS_SEND_SMS, result);

    rr->mParcel->WriteInt32(IRILConstants::GSM_PHONE);
    rr->mParcel->WriteByte((Byte)retry);
    rr->mParcel->WriteInt32(messageRef);

    ConstructGsmSendSmsRilRequest(rr, smscPDU, pdu);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendImsCdmaSms(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 retry,
    /* [in] */ Int32 messageRef,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_IMS_SEND_SMS, result);

    rr->mParcel->WriteInt32(IRILConstants::CDMA_PHONE);
    rr->mParcel->WriteByte((Byte)retry);
    rr->mParcel->WriteInt32(messageRef);

    ConstructCdmaSendSmsRilRequest(rr, pdu);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::DeleteSmsOnSim(
    /* [in] */ Int32 index,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_DELETE_SMS_ON_SIM,
            response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(index);

    if (RILJ_LOGV) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += index;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::DeleteSmsOnRuim(
    /* [in] */ Int32 index,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM,
            response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(index);

    if (RILJ_LOGV) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " "; str += index;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::WriteSmsToSim(
    /* [in] */ Int32 status,
    /* [in] */ const String& smsc,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* response)
{
    status = TranslateStatus(status);

    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_WRITE_SMS_TO_SIM,
            response);

    rr->mParcel->WriteInt32(status);
    rr->mParcel->WriteString(pdu);
    rr->mParcel->WriteString(smsc);

    if (RILJ_LOGV) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += status;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::WriteSmsToRuim(
    /* [in] */ Int32 status,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* response)
{
    status = TranslateStatus(status);

    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM,
                                response);

    rr->mParcel->WriteInt32(status);
    ConstructCdmaWriteSmsRilRequest(rr, IccUtils::HexStringToBytes(pdu));

    if (RILJ_LOGV) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += status;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

void RIL::ConstructCdmaWriteSmsRilRequest(
    /* [in] */ RILRequest* rr,
    /* [in] */ ArrayOf<Byte>* pdu)
{
    Byte address_nbr_of_digits = 0;
    Byte subaddr_nbr_of_digits = 0;
    Byte bearerDataLength = 0;
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(pdu, (IByteArrayInputStream**)&bais);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New(IInputStream::Probe(bais), (IDataInputStream**)&dis);

    // try {
        Int32 teleServiceId = 0;
        Byte servicePresent = 0;
        Int32 serviceCategory = 0;

        Byte address_digit_mode = 0;
        Byte address_nbr_mode = 0;
        Byte address_ton = 0;
        Byte address_nbr_plan = 0;

        Byte subaddressType = 0;
        Byte subaddr_odd = 0;

        IDataInput::Probe(dis)->ReadInt32(&teleServiceId);
        rr->mParcel->WriteInt32(teleServiceId);
        Int32 iP = 0;
        IDataInput::Probe(dis)->ReadInt32(&iP);
        servicePresent = (Byte) iP;
        rr->mParcel->WriteByte(servicePresent);
        IDataInput::Probe(dis)->ReadInt32(&serviceCategory);
        rr->mParcel->WriteInt32(serviceCategory);

        IDataInput::Probe(dis)->ReadByte(&address_digit_mode);
        rr->mParcel->WriteInt32((Int32) address_digit_mode);
        IDataInput::Probe(dis)->ReadByte(&address_nbr_mode);
        rr->mParcel->WriteInt32(address_nbr_mode);
        IDataInput::Probe(dis)->ReadByte(&address_ton);
        rr->mParcel->WriteInt32(address_ton);
        IDataInput::Probe(dis)->ReadByte(&address_nbr_plan);
        rr->mParcel->WriteInt32(address_nbr_plan);

        IDataInput::Probe(dis)->ReadByte(&address_nbr_of_digits);
        rr->mParcel->WriteByte(address_nbr_of_digits);
        for (Int32 i = 0; i < address_nbr_of_digits; i++) {
            Byte address_orig_bytes_node = 0;
            IDataInput::Probe(dis)->ReadByte(&address_orig_bytes_node);
            rr->mParcel->WriteByte(address_orig_bytes_node); // address_orig_bytes[i]
        }

        // Int32
        IDataInput::Probe(dis)->ReadByte(&subaddressType);
        rr->mParcel->WriteInt32(subaddressType); // subaddressType
        IDataInput::Probe(dis)->ReadByte(&subaddr_odd);
        rr->mParcel->WriteByte(subaddr_odd); // subaddr_odd
        IDataInput::Probe(dis)->ReadByte(&subaddr_nbr_of_digits);
        rr->mParcel->WriteByte((Byte) subaddr_nbr_of_digits);
        for (Int32 i = 0; i < subaddr_nbr_of_digits; i++) {
            Byte subaddr_orig_bytes = 0;
            IDataInput::Probe(dis)->ReadByte(&subaddr_orig_bytes);
            rr->mParcel->WriteByte(subaddr_orig_bytes); // subaddr_orig_bytes[i]
        }

        IDataInput::Probe(dis)->ReadByte(&bearerDataLength);
        bearerDataLength = bearerDataLength & 0xff;
        rr->mParcel->WriteInt32(bearerDataLength);
        for (Int32 i = 0; i < bearerDataLength; i++) {
            Byte bearerData = 0;
            IDataInput::Probe(dis)->ReadByte(&bearerData);
            rr->mParcel->WriteByte(bearerData); // bearerData[i]
        }

        // String logMsg(" teleServiceId=");
        // logMsg += teleServiceId; logMsg += " servicePresent=";
        // logMsg += servicePresent; logMsg += " serviceCategory=";
        // logMsg += serviceCategory; logMsg += " address_digit_mode=";
        // logMsg += address_digit_mode; logMsg += " address_nbr_mode=";
        // logMsg += address_nbr_mode; logMsg += " address_ton=";
        // logMsg += address_ton; logMsg += " address_nbr_plan=";
        // logMsg += address_nbr_plan; logMsg += " address_nbr_of_digits=";
        // logMsg += address_nbr_of_digits; logMsg += " subaddressType=";
        // logMsg += subaddressType; logMsg += " subaddr_odd= ";
        // logMsg += subaddr_odd; logMsg += " subaddr_nbr_of_digits=";
        // logMsg += subaddr_nbr_of_digits; logMsg += " bearerDataLength=";
        // logMsg += bearerDataLength;

        // RiljLog(logMsg);
    // } catch (IOException ex) {
    //     if (RILJ_LOGD) {
    //         RiljLog("sendSmsCdma: conversion from input stream to object failed: "
    //             + ex);
    //     }
    // } finally {
        // try {
            if (NULL != bais) {
                ICloseable::Probe(bais)->Close();
            }

            if (NULL != dis) {
                ICloseable::Probe(dis)->Close();
            }
        // } catch (IOException e) {
        //     if (RILJ_LOGD) {
        //         RiljLog("sendSmsCdma: close input stream exception" + e);
        //     }
        // }
    // }
}

Int32 RIL::TranslateStatus(
    /* [in] */ Int32 status)
{
    switch(status & 0x7) {
        case ISmsManager::STATUS_ON_ICC_READ:
            return 1;
        case ISmsManager::STATUS_ON_ICC_UNREAD:
            return 0;
        case ISmsManager::STATUS_ON_ICC_SENT:
            return 3;
        case ISmsManager::STATUS_ON_ICC_UNSENT:
            return 2;
    }

    // Default to READ.
    return 1;
}

ECode RIL::SetupDataCall(
    /* [in] */ const String& radioTechnology,
    /* [in] */ const String& profile,
    /* [in] */ const String& apn,
    /* [in] */ const String& user,
    /* [in] */ const String& password,
    /* [in] */ const String& authType,
    /* [in] */ const String& protocol,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SETUP_DATA_CALL, result);

    rr->mParcel->WriteInt32(7);

    rr->mParcel->WriteString(radioTechnology);
    rr->mParcel->WriteString(profile);
    rr->mParcel->WriteString(apn);
    rr->mParcel->WriteString(user);
    rr->mParcel->WriteString(password);
    rr->mParcel->WriteString(authType);
    rr->mParcel->WriteString(protocol);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " "; str += radioTechnology;
        // str += " "; str += profile;
        // str += " "; str += apn;
        // str += " "; str += user;
        // str += " "; str += password;
        // str += " "; str += authType;
        // str += " "; str += protocol;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::DeactivateDataCall(
    /* [in] */ Int32 cid,
    /* [in] */ Int32 reason,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_DEACTIVATE_DATA_CALL,
                result);

    rr->mParcel->WriteInt32(2);
    rr->mParcel->WriteInt32(cid);
    rr->mParcel->WriteInt32(reason);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += cid;
        // str += " ";
        // str += reason;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetRadioPower(
    /* [in] */ Boolean on,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_RADIO_POWER,
                                result);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(on ? 1 : 0);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += (on ? " on" : " off");
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::RequestShutdown(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_SHUTDOWN,
                                result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetSuppServiceNotifications(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION,
                result);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(enable ? 1 : 0);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::AcknowledgeLastIncomingGsmSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 cause,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SMS_ACKNOWLEDGE,
                result);

    rr->mParcel->WriteInt32(2);
    rr->mParcel->WriteInt32(success ? 1 : 0);
    rr->mParcel->WriteInt32(cause);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += success;
        // str += " ";
        // str += cause;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::AcknowledgeLastIncomingCdmaSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 cause,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                    IRILConstants::RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE,
                    result);

    rr->mParcel->WriteInt32(success ? 0 : 1); //RIL_CDMA_SMS_ErrorClass
    // cause code according to X.S004-550E
    rr->mParcel->WriteInt32(cause);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += success;
        // str += " ";
        // str += cause;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::AcknowledgeIncomingGsmSmsWithPdu(
    /* [in] */ Boolean success,
    /* [in] */ const String& ackPdu,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU,
                result);

    rr->mParcel->WriteInt32(2);
    rr->mParcel->WriteString(success ? String("1") : String("0"));
    rr->mParcel->WriteString(ackPdu);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ' ';
        // str += success;
        // str += " [";
        // str += ackPdu;
        // str += ']';
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::IccIO(
    /* [in] */ Int32 command,
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* result)
{
    return IccIOForApp(command, fileid, path, p1, p2, p3, data, pin2, String(NULL), result);
}

ECode RIL::IccIOForApp(
    /* [in] */ Int32 command,
    /* [in] */ Int32 fileid,
    /* [in] */ const String& path,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ const String& pin2,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* result)
{
    //Note: This RIL request has not been renamed to ICC,
    //       but this request is also valid for SIM and RUIM
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SIM_IO, result);

    rr->mParcel->WriteInt32(command);
    rr->mParcel->WriteInt32(fileid);
    rr->mParcel->WriteString(path);
    rr->mParcel->WriteInt32(p1);
    rr->mParcel->WriteInt32(p2);
    rr->mParcel->WriteInt32(p3);
    rr->mParcel->WriteString(data);
    rr->mParcel->WriteString(pin2);
    rr->mParcel->WriteString(aid);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> iccIO: ";
        // str += RequestToString(rr->mRequest);
        // str += " 0x";
        // str += StringUtils::ToHexString(command);
        // str += " 0x";
        // str += StringUtils::ToHexString(fileid);
        // str += " ";
        // str += " path: "; str += path; str += ",";
        // str += p1; str += ","; str += p2;
        // str += ","; str += p3;
        // str += " aid: "; str += aid;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetCLIR(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_CLIR, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCLIR(
    /* [in] */ Int32 clirMode,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_CLIR, result);

    // count ints
    rr->mParcel->WriteInt32(1);

    rr->mParcel->WriteInt32(clirMode);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += clirMode;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::QueryCallWaiting(
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_QUERY_CALL_WAITING,
                response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(serviceClass);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += serviceClass;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_CALL_WAITING,
                response);

    rr->mParcel->WriteInt32(2);
    rr->mParcel->WriteInt32(enable ? 1 : 0);
    rr->mParcel->WriteInt32(serviceClass);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += enable;
        // str += ", ";
        // str += serviceClass;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,
                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetNetworkSelectionModeManual(
    /* [in] */ const String& operatorNumeric,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,
                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += " ";
        str += operatorNumeric;
        RiljLog(str);
    }

    rr->mParcel->WriteString(operatorNumeric);

    Send(rr);
    return NOERROR;
}

ECode RIL::GetNetworkSelectionMode(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,
                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_QUERY_AVAILABLE_NETWORKS,
                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCallForward(
    /* [in] */ Int32 action,
    /* [in] */ Int32 cfReason,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& number,
    /* [in] */ Int32 timeSeconds,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_CALL_FORWARD, response);

    rr->mParcel->WriteInt32(action);
    rr->mParcel->WriteInt32(cfReason);
    rr->mParcel->WriteInt32(serviceClass);
    AutoPtr<IPhoneNumberUtils> pu;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
    Int32 iNum = 0;
    pu->ToaFromString(number, &iNum);
    rr->mParcel->WriteInt32(iNum);
    rr->mParcel->WriteString(number);
    rr->mParcel->WriteInt32 (timeSeconds);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " "; str += action;
        // str += " "; str += cfReason;
        // str += " "; str += serviceClass;
        // str += timeSeconds;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::QueryCallForwardStatus(
    /* [in] */ Int32 cfReason,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& number,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
        = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,
            response);

    rr->mParcel->WriteInt32(2); // 2 is for query action, not in used anyway
    rr->mParcel->WriteInt32(cfReason);
    rr->mParcel->WriteInt32(serviceClass);
    AutoPtr<IPhoneNumberUtils> pu;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pu);
    Int32 iNum = 0;
    pu->ToaFromString(number, &iNum);
    rr->mParcel->WriteInt32(iNum);
    rr->mParcel->WriteString(number);
    rr->mParcel->WriteInt32 (0);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += cfReason;
        // str += " ";
        // str += serviceClass;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::QueryCLIP(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
        = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_QUERY_CLIP, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetBasebandVersion(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_BASEBAND_VERSION, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::QueryFacilityLock(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    return QueryFacilityLockForApp(facility, password, serviceClass, String(NULL), response);
}

ECode RIL::QueryFacilityLockForApp(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& appId,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
                                IRILConstants::RIL_REQUEST_QUERY_FACILITY_LOCK,
                                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    // count strings
    rr->mParcel->WriteInt32(oldRil ? 3 : 4);

    rr->mParcel->WriteString(facility);
    rr->mParcel->WriteString(password);

    rr->mParcel->WriteString(StringUtils::ToString(serviceClass));

    if (!oldRil) {
        rr->mParcel->WriteString(appId);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetFacilityLock(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ IMessage* response)
{
    return SetFacilityLockForApp(facility, lockState, password, serviceClass, String(NULL), response);
}

ECode RIL::SetFacilityLockForApp(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ Int32 serviceClass,
    /* [in] */ const String& appId,
    /* [in] */ IMessage* response)
{
    String lockString;
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_FACILITY_LOCK,
                response);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ["; str += facility;
        // str += " "; str += lockState;
        // str += " "; str += serviceClass;
        // str += " "; str += appId;
        // str += "]";
        // RiljLog(str);
    }

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("facilitylock"), &oldRil);

    // count strings
    rr->mParcel->WriteInt32(oldRil ? 4 : 5);

    rr->mParcel->WriteString(facility);
    lockString = (lockState) ? "1" : "0";
    rr->mParcel->WriteString(lockString);
    rr->mParcel->WriteString(password);
    rr->mParcel->WriteString(StringUtils::ToString(serviceClass));

    if (!oldRil) {
        rr->mParcel->WriteString(appId);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendUSSD(
    /* [in] */ const String& ussdString,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SEND_USSD, response);

    if (RILJ_LOGD) {
        String logUssdString("*******");
        if (RILJ_LOGV) {
            logUssdString = ussdString;
        }
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += " ";
        str += logUssdString;
        RiljLog(str);
    }

    rr->mParcel->WriteString(ussdString);

    Send(rr);
    return NOERROR;
}

ECode RIL::CancelPendingUssd(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_CANCEL_USSD,
                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ResetRadio(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_RESET_RADIO, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetLocalCallHold(
    /* [in] */ Int32 lchStatus)
{
    AutoPtr<ArrayOf<Byte> > payload = ArrayOf<Byte>::Alloc(1);
    (*payload)[0] = (Byte)(lchStatus & 0x7F);
    // Rlog.d(RILJ_LOG_TAG, "setLocalCallHold: lchStatus is " + lchStatus);

    SendOemRilRequestRaw(OEMHOOK_EVT_HOOK_SET_LOCAL_CALL_HOLD, 1, payload, NULL);
    return NOERROR;
}

ECode RIL::GetModemCapability(
    /* [in] */ IMessage* response)
{
    // Rlog.d(RILJ_LOG_TAG, "GetModemCapability");
    SendOemRilRequestRaw(OEMHOOK_EVT_HOOK_GET_MODEM_CAPABILITY, 0, NULL, response);
    return NOERROR;
}

ECode RIL::UpdateStackBinding(
    /* [in] */ Int32 stack,
    /* [in] */ Int32 enable,
    /* [in] */ IMessage* response)
{
    AutoPtr<ArrayOf<Byte> > payload = ArrayOf<Byte>::Alloc(2);
    (*payload)[0] = (Byte)stack;
    (*payload)[1] = (Byte)enable;
    // Rlog.d(RILJ_LOG_TAG, "UpdateStackBinding: on Stack: " + stack +
    //         ", enable/disable: " + enable);

    SendOemRilRequestRaw(OEMHOOK_EVT_HOOK_UPDATE_SUB_BINDING, 2, payload, response);
    return NOERROR;
}

void RIL::SendOemRilRequestRaw(
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 numPayload,
    /* [in] */ ArrayOf<Byte>* payload,
    /* [in] */ IMessage* response)
{
    AutoPtr<ArrayOf<Byte> > request = ArrayOf<Byte>::Alloc(1);
    (*request)[0] = mHeaderSize + numPayload * BYTE_SIZE;

    AutoPtr<IByteBufferHelper> bbf;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbf);
    AutoPtr<IByteBuffer> buf;
    bbf->Wrap(request, (IByteBuffer**)&buf);
    AutoPtr<IByteOrderHelper> bod;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&bod);
    ByteOrder bo;
    bod->GetNativeOrder(&bo);
    buf->SetOrder(bo);

    // Add OEM identifier String
    buf->Put(OEM_IDENTIFIER.GetBytes());
    // Add Request ID
    buf->PutInt32(requestId);
    if (numPayload > 0 && payload != NULL) {
        // Add Request payload length
        buf->PutInt32(numPayload * BYTE_SIZE);
        for (Int32 i = 0; i < payload->GetLength(); i++) {
            Byte b = (*payload)[i];
            buf->Put(b);
        }
    }

    InvokeOemRilRequestRaw(request, response);
}

ECode RIL::InvokeOemRilRequestRaw(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_OEM_HOOK_RAW, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += "[";
        str += StringUtils::ToHexString(*data);
        str += "]";
        RiljLog(str);
    }

    rr->mParcel->WriteByteArray(data);

    Send(rr);
    return NOERROR;
}

ECode RIL::InvokeOemRilRequestStrings(
    /* [in] */ ArrayOf<String>* strings,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_OEM_HOOK_STRINGS, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    rr->mParcel->WriteStringArray(strings);

    Send(rr);
    return NOERROR;
}

ECode RIL::SetBandMode(
    /* [in] */ Int32 bandMode,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(
                IRILConstants::RIL_REQUEST_SET_BAND_MODE, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(bandMode);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += bandMode;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::QueryAvailableBandMode(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr
            = RILRequest::Obtain(IRILConstants::RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE,
                                response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendTerminalResponse(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    rr->mParcel->WriteString(contents);
    Send(rr);
    return NOERROR;
}

ECode RIL::SendEnvelope(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    rr->mParcel->WriteString(contents);
    Send(rr);
    return NOERROR;
}

ECode RIL::SendEnvelopeWithStatus(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS, response);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += '[';
        // str += contents;
        // str += ']';
        // RiljLog(str);
    }

    rr->mParcel->WriteString(contents);
    Send(rr);
    return NOERROR;
}

ECode RIL::HandleCallSetupRequestFromSim(
    /* [in] */ Boolean accept,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
        IRILConstants::RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM,
        response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(1);
    (*param)[0] = accept ? 1 : 0;
    rr->mParcel->WriteInt32Array(param.Get());
    Send(rr);
    return NOERROR;
}

ECode RIL::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(networkType);

    mPreferredNetworkType = networkType;

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " : ";
        // str += networkType;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetPreferredNetworkType(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_GET_NEIGHBORING_CELL_IDS, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetLocationUpdates(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_LOCATION_UPDATES, response);
    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(enable ? 1 : 0);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ": ";
        // str += enable;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetSmscAddress(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_SMSC_ADDRESS, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetSmscAddress(
    /* [in] */ const String& address,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_SMSC_ADDRESS, result);

    rr->mParcel->WriteString(address);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += " : ";
        str += address;
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ReportSmsMemoryStatus(
    /* [in] */ Boolean available,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, result);
    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(available ? 1 : 0);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ": ";
        // str += available;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ReportStkServiceIsRunning(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetGsmBroadcastConfig(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GSM_GET_BROADCAST_CONFIG, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetGsmBroadcastConfig(
    /* [in] */ ArrayOf<ISmsBroadcastConfigInfo*>* config,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GSM_SET_BROADCAST_CONFIG, response);

    Int32 numOfConfig = config->GetLength();
    rr->mParcel->WriteInt32(numOfConfig);

    for(Int32 i = 0; i < numOfConfig; i++) {
        Int32 formServiceId = 0;
        (*config)[i]->GetFromServiceId(&formServiceId);
        rr->mParcel->WriteInt32(formServiceId);

        Int32 toServiceId = 0;
        (*config)[i]->GetToServiceId(&toServiceId);
        rr->mParcel->WriteInt32(toServiceId);

        Int32 fromCodeScheme = 0;
        (*config)[i]->GetFromCodeScheme(&fromCodeScheme);
        rr->mParcel->WriteInt32(fromCodeScheme);

        Int32 toCodeScheme = 0;
        (*config)[i]->GetToCodeScheme(&toCodeScheme);
        rr->mParcel->WriteInt32(toCodeScheme);

        Boolean bSelected = 0;
        (*config)[i]->IsSelected(&bSelected);
        rr->mParcel->WriteInt32(bSelected ? 1 : 0);
    }

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " with ";
        // str += numOfConfig;
        // str += " configs : ";
        // RiljLog(str);
        for (Int32 i = 0; i < numOfConfig; i++) {
            Logger::E(RILJ_LOG_TAG, "TODO ");
            // RiljLog((*config)[i]->ToString());
        }
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetGsmBroadcastActivation(
    /* [in] */ Boolean activate,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GSM_BROADCAST_ACTIVATION, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(activate ? 0 : 1);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

void RIL::UpdateScreenState()
{
    Int32 oldState = mDefaultDisplayState;
    mDefaultDisplay->GetState(&mDefaultDisplayState);
    if (mDefaultDisplayState != oldState) {
        if (oldState != IDisplay::STATE_ON
                && mDefaultDisplayState == IDisplay::STATE_ON) {
            SendScreenState(TRUE);
        }
        else if ((oldState == IDisplay::STATE_ON || oldState == IDisplay::STATE_UNKNOWN)
                    && mDefaultDisplayState != IDisplay::STATE_ON) {
            SendScreenState(FALSE);
        }
    }
}

void RIL::SendScreenState(
    /* [in] */ Boolean on)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SCREEN_STATE, NULL);
    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(on ? 1 : 0);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ": ";
        // str += on;
        // RiljLog(str);
    }

    Send(rr);
}

ECode RIL::OnRadioAvailable()
{
    // In case screen state was lost (due to process crash),
    // this ensures that the RIL knows the correct screen state.
    UpdateScreenState();
    return NOERROR;
}

ECode RIL::GetRadioStateFromInt(
    /* [in] */ Int32 stateInt,
    /* [out] */ ICommandsInterfaceRadioState* state)
{
    VALIDATE_NOT_NULL(state);

    /* RIL_RadioState ril.h */
    switch(stateInt) {
        case 0: *state = RADIO_OFF; break;
        case 1: *state = RADIO_UNAVAILABLE; break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10: *state = RADIO_ON; break;

        default:
            // throw new RuntimeException(
            //             "Unrecognized RIL_RadioState: " + stateInt);
            return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

void RIL::SwitchToRadioState(
    /* [in] */ ICommandsInterfaceRadioState newState)
{
    SetRadioState(newState);
}

void RIL::AcquireWakeLock()
{
    AutoLock lock(mWakeLock);
    mWakeLock->AcquireLock();
    mWakeLockCount++;

    mSender->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
    AutoPtr<IMessage> msg;
    mSender->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT, (IMessage**)&msg);
    Boolean bSnd = FALSE;
    mSender->SendMessageDelayed(msg, mWakeLockTimeout, &bSnd);
}

void RIL::DecrementWakeLock()
{
    AutoLock lock(mWakeLock);
    if (mWakeLockCount > 1) {
        mWakeLockCount--;
    }
    else {
        mWakeLockCount = 0;
        mWakeLock->ReleaseLock();
        mSender->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
    }
}

Boolean RIL::ClearWakeLock()
{
    AutoLock lock(mWakeLock);
    Boolean bHeld = FALSE;
    mWakeLock->IsHeld(&bHeld);
    if (mWakeLockCount == 0 && bHeld == FALSE) return FALSE;
    // Rlog.d(RILJ_LOG_TAG, "NOTE: mWakeLockCount is " + mWakeLockCount + "at time of clearing");
    mWakeLockCount = 0;
    mWakeLock->ReleaseLock();
    mSender->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
    return TRUE;
}

void RIL::Send(
    /* [in] */ RILRequest* rr)
{
    AutoPtr<IMessage> msg;

    if (mSocket == NULL) {
        Logger::E(RILJ_LOG_TAG, "RIL::Send mSocket is NULL, error is RADIO_NOT_AVAILABLE");
        rr->OnError(IRILConstants::RADIO_NOT_AVAILABLE, NULL);
        rr->ReleaseRequest();
        return;
    }
    //Logger::E("leliang", "line:%d, func:%s\n", __LINE__, __func__);

    mSender->ObtainMessage(EVENT_SEND, TO_IINTERFACE(rr), (IMessage**)&msg);

    AcquireWakeLock();

    msg->SendToTarget();
}

void RIL::ProcessResponse(
    /* [in] */ RILParcel* p)
{
    Int32 type = 0;

    p->ReadInt32(&type);

    if (type == RESPONSE_UNSOLICITED) {
        ProcessUnsolicited(p);
    }
    else if (type == RESPONSE_SOLICITED) {
        AutoPtr<RILRequest> rr = ProcessSolicited(p);
        if (rr != NULL) {
            rr->ReleaseRequest();
            DecrementWakeLock();
        }
    }
}

void RIL::ClearRequestList(
    /* [in] */ Int32 error,
    /* [in] */ Boolean loggable)
{
    AutoPtr<RILRequest> rr;
    {
        AutoLock lock(mRequestList);
        Int32 count = 0;
        mRequestList->GetSize(&count);
        if (RILJ_LOGD && loggable) {
            // Rlog.d(RILJ_LOG_TAG, "clearRequestList " +
            //         " mWakeLockCount=" + mWakeLockCount +
            //         " mRequestList=" + count);
        }

        for (Int32 i = 0; i < count ; i++) {
            AutoPtr<IInterface> val;
            mRequestList->ValueAt(i, (IInterface**)&val);
            rr = (RILRequest*)IObject::Probe(val);
            if (RILJ_LOGD && loggable) {
                // Rlog.d(RILJ_LOG_TAG, i + ": [" + rr.mSerial + "] " +
                //         requestToString(rr.mRequest));
            }
            rr->OnError(error, NULL);
            rr->ReleaseRequest();
            DecrementWakeLock();
        }
        mRequestList->Clear();
    }
}

AutoPtr<RILRequest> RIL::FindAndRemoveRequestFromList(
    /* [in] */ Int32 serial)
{
    AutoPtr<RILRequest> rr = NULL;
    {
        AutoLock lock(mRequestList);
        AutoPtr<IInterface> p;
        mRequestList->Get(serial, (IInterface**)&p);
        rr = (RILRequest*)IObject::Probe(p);
        if (rr != NULL) {
            mRequestList->Remove(serial);
        }
    }

    return rr;
}

AutoPtr<RILRequest> RIL::ProcessSolicited(
    /* [in] */ RILParcel* p)
{
    Int32 serial = 0, error = 0;
    // Boolean found = FALSE;

    p->ReadInt32(&serial);
    p->ReadInt32(&error);

    AutoPtr<RILRequest> rr;

    rr = FindAndRemoveRequestFromList(serial);

    if (rr == NULL) {
        Logger::W(RILJ_LOG_TAG, "Unexpected solicited response! sn: %d, error: %d", serial, error);
        // Rlog.w(RILJ_LOG_TAG, "Unexpected solicited response! sn: "
        //                 + serial + " error: " + error);
        return NULL;
    }

    AutoPtr<IInterface> ret;

    Int32 dataAvail = 0;
    p->DataAvail(&dataAvail);
    //Logger::E("leliang", "RIL::ProcessSolicited, request:%d,serial:%d,dataAvail:%d, error:%d", rr->mRequest, serial, dataAvail, error);
    if (error == 0 || dataAvail > 0) {
        // either command succeeds or command fails but with data payload
        // try {
        switch (rr->mRequest) {
            /*
            cat libs/telephony/ril_commands.h \
            | egrep "^ *{RIL_" \
            | sed -re 's/\{([^,]+),[^,]+,([^}]+).+/case \1: ret = \2(p); break;/'
             */
            case IRILConstants::RIL_REQUEST_GET_SIM_STATUS: ret = ResponseIccCardStatus(p); break;
            case IRILConstants::RIL_REQUEST_ENTER_SIM_PIN: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_ENTER_SIM_PUK: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_ENTER_SIM_PIN2: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_ENTER_SIM_PUK2: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN2: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_GET_CURRENT_CALLS: ret =  ResponseCallList(p); break;
            case IRILConstants::RIL_REQUEST_DIAL: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GET_IMSI: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_HANGUP: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: {
                Boolean bGetAndSet = FALSE;
                mTestingEmergencyCall->GetAndSet(FALSE, &bGetAndSet);
                if (bGetAndSet) {
                    if (mEmergencyCallbackModeRegistrant != NULL) {
                        RiljLog(String("testing emergency call, notify ECM Registrants"));
                        mEmergencyCallbackModeRegistrant->NotifyRegistrant();
                    }
                }
                ret = ResponseVoid(p);
                break;
            }
            case IRILConstants::RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CONFERENCE: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_UDUB: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_LAST_CALL_FAIL_CAUSE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SIGNAL_STRENGTH: ret =  ResponseSignalStrength(p); break;
            case IRILConstants::RIL_REQUEST_VOICE_REGISTRATION_STATE: ret =  ResponseStrings(p); break;
            case IRILConstants::RIL_REQUEST_DATA_REGISTRATION_STATE: ret =  ResponseStrings(p); break;
            case IRILConstants::RIL_REQUEST_OPERATOR: ret =  ResponseStrings(p); break;
            case IRILConstants::RIL_REQUEST_RADIO_POWER: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_DTMF: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SEND_SMS: ret =  ResponseSMS(p); break;
            case IRILConstants::RIL_REQUEST_SEND_SMS_EXPECT_MORE: ret =  ResponseSMS(p); break;
            case IRILConstants::RIL_REQUEST_SETUP_DATA_CALL: ret =  ResponseSetupDataCall(p); break;
            case IRILConstants::RIL_REQUEST_SIM_IO: ret =  ResponseICC_IO(p); break;
            case IRILConstants::RIL_REQUEST_SEND_USSD: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CANCEL_USSD: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GET_CLIR: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SET_CLIR: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: ret =  ResponseCallForward(p); break;
            case IRILConstants::RIL_REQUEST_SET_CALL_FORWARD: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_CALL_WAITING: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SET_CALL_WAITING: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SMS_ACKNOWLEDGE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GET_IMEI: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_GET_IMEISV: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_ANSWER: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_DEACTIVATE_DATA_CALL: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_FACILITY_LOCK: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SET_FACILITY_LOCK: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_CHANGE_BARRING_PASSWORD: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : ret =  ResponseOperatorInfos(p); break;
            case IRILConstants::RIL_REQUEST_DTMF_START: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_DTMF_STOP: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_BASEBAND_VERSION: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_SEPARATE_CONNECTION: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_MUTE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GET_MUTE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_CLIP: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_DATA_CALL_LIST: ret =  ResponseDataCallList(p); break;
            case IRILConstants::RIL_REQUEST_RESET_RADIO: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_OEM_HOOK_RAW: ret =  ResponseRaw(p); break;
            case IRILConstants::RIL_REQUEST_OEM_HOOK_STRINGS: ret =  ResponseStrings(p); break;
            case IRILConstants::RIL_REQUEST_SCREEN_STATE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_WRITE_SMS_TO_SIM: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_DELETE_SMS_ON_SIM: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_BAND_MODE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_STK_GET_PROFILE: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_STK_SET_PROFILE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_EXPLICIT_CALL_TRANSFER: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: ret =  ResponseGetPreferredNetworkType(p); break;
            case IRILConstants::RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: ret = ResponseCellList(p); break;
            case IRILConstants::RIL_REQUEST_SET_LOCATION_UPDATES: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SET_TTY_MODE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_QUERY_TTY_MODE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_FLASH: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_BURST_DTMF: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SEND_SMS: ret =  ResponseSMS(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GSM_GET_BROADCAST_CONFIG: ret =  ResponseGmsBroadcastConfig(p); break;
            case IRILConstants::RIL_REQUEST_GSM_SET_BROADCAST_CONFIG: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GSM_BROADCAST_ACTIVATION: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG: ret =  ResponseCdmaBroadcastConfig(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_BROADCAST_ACTIVATION: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_SUBSCRIPTION: ret =  ResponseStrings(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: ret =  ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_DEVICE_IDENTITY: ret =  ResponseStrings(p); break;
            case IRILConstants::RIL_REQUEST_GET_SMSC_ADDRESS: ret = ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_SET_SMSC_ADDRESS: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: ret =  ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_GET_DATA_CALL_PROFILE: ret =  ResponseGetDataCallProfile(p); break;
            case IRILConstants::RIL_REQUEST_ISIM_AUTHENTICATION: ret =  ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: ret = ResponseICC_IO(p); break;
            case IRILConstants::RIL_REQUEST_VOICE_RADIO_TECH: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_GET_CELL_INFO_LIST: ret = ResponseCellInfoList(p); break;
            case IRILConstants::RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_INITIAL_ATTACH_APN: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_DATA_PROFILE: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_IMS_REGISTRATION_STATE: ret = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_IMS_SEND_SMS: ret =  ResponseSMS(p); break;
            case IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: ret =  ResponseICC_IO(p); break;
            case IRILConstants::RIL_REQUEST_SIM_OPEN_CHANNEL: ret  = ResponseInts(p); break;
            case IRILConstants::RIL_REQUEST_SIM_CLOSE_CHANNEL: ret  = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: ret = ResponseICC_IO(p); break;
            case IRILConstants::RIL_REQUEST_SIM_GET_ATR: ret = ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_NV_READ_ITEM: ret = ResponseString(p); break;
            case IRILConstants::RIL_REQUEST_NV_WRITE_ITEM: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_NV_WRITE_CDMA_PRL: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_NV_RESET_CONFIG: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_SET_UICC_SUBSCRIPTION: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_ALLOW_DATA: ret = ResponseVoid(p); break;
            case IRILConstants::RIL_REQUEST_GET_HARDWARE_CONFIG: ret = ResponseHardwareConfig(p); break;
            case IRILConstants::RIL_REQUEST_SIM_AUTHENTICATION: ret =  ResponseICC_IOBase64(p); break;
            case IRILConstants::RIL_REQUEST_SHUTDOWN: ret = ResponseVoid(p); break;
            default: {
                // throw new RuntimeException("Unrecognized solicited response: " + rr.mRequest);
                assert(0 && "Unrecognized solicited response");
                break;
            }
        }
        // } catch (Throwable tr) {
        //     // Exceptions here usually mean invalid RIL responses

        //     Rlog.w(RILJ_LOG_TAG, rr.serialString() + "< "
        //             + requestToString(rr.mRequest)
        //             + " exception, possible invalid RIL response", tr);

        //     if (rr.mResult != NULL) {
        //         AsyncResult.forMessage(rr.mResult, NULL, tr);
        //         rr.mResult.sendToTarget();
        //     }
        //     return rr;
        // }
    }

    if (rr->mRequest == IRILConstants::RIL_REQUEST_SHUTDOWN) {
        // Set RADIO_STATE to RADIO_UNAVAILABLE to continue shutdown process
        // regardless of error code to continue shutdown procedure.
        // String str("Response to RIL_REQUEST_SHUTDOWN received. Error is ");
        // str += error;
        // str += " Setting Radio State to Unavailable regardless of error.";
        // RiljLog(str);
        SetRadioState(RADIO_UNAVAILABLE);
    }

    // Here and below fake RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, see b/7255789.
    // This is needed otherwise we don't automatically transition to the main lock
    // screen when the pin or puk is entered incorrectly.
    switch (rr->mRequest) {
        case IRILConstants::RIL_REQUEST_ENTER_SIM_PUK:
        case IRILConstants::RIL_REQUEST_ENTER_SIM_PUK2:
            if (mIccStatusChangedRegistrants != NULL) {
                if (RILJ_LOGD) {
                    // String str("ON enter sim puk fakeSimStatusChanged: reg count=");
                    // str += mIccStatusChangedRegistrants->GetSize();
                    // RiljLog(str);
                }
                mIccStatusChangedRegistrants->NotifyRegistrants();
            }
            break;
    }

    if (error != 0) {
        switch (rr->mRequest) {
            case IRILConstants::RIL_REQUEST_ENTER_SIM_PIN:
            case IRILConstants::RIL_REQUEST_ENTER_SIM_PIN2:
            case IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN:
            case IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN2:
            case IRILConstants::RIL_REQUEST_SET_FACILITY_LOCK:
                if (mIccStatusChangedRegistrants != NULL) {
                    if (RILJ_LOGD) {
                        // String str("ON some errors fakeSimStatusChanged: reg count=");
                        // str += mIccStatusChangedRegistrants->GetSize();
                        // RiljLog(str);
                    }
                    mIccStatusChangedRegistrants->NotifyRegistrants();
                }
                break;
        }

        rr->OnError(error, ret);
    }
    else {

        if (RILJ_LOGD) {
            String str = rr->SerialString();
            str += "< ";
            str += RequestToString(rr->mRequest);
            str += " ";
            str += RetToString(rr->mRequest, ret);
            RiljLog(str);
        }

        if (rr->mResult != NULL) {
            AsyncResult::ForMessage(rr->mResult, ret, NULL);
            rr->mResult->SendToTarget();
        }
    }
    return rr;
}

String RIL::RetToString(
    /* [in] */ Int32 req,
    /* [in] */ IInterface* ret)
{
    if (ret == NULL) return String("");
    switch (req) {
        // Don't log these return values, for privacy's sake.
        case IRILConstants::RIL_REQUEST_GET_IMSI:
        case IRILConstants::RIL_REQUEST_GET_IMEI:
        case IRILConstants::RIL_REQUEST_GET_IMEISV:
        case IRILConstants::RIL_REQUEST_SIM_OPEN_CHANNEL:
        case IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:

            if (!RILJ_LOGV) {
                // If not versbose logging just return and don't display IMSI and IMEI, IMEISV
                return String("");
            }
    }

    StringBuilder sb;
    String s;
    Int32 length = 0;
    Logger::E(RILJ_LOG_TAG, "TODO RIL::RetToString, line:%d, length:%d", __LINE__, length);
    //assert(0 && "TODO");
    // if (ret instanceof Int32[]){
    //     AutoPtr<ArrayOf<Int32> > intArray = (Int32[]) ret;
    //     length = intArray->GetLength();
    //     sb = new StringBuilder("{");
    //     if (length > 0) {
    //         Int32 i = 0;
    //         sb.Append(intArray[i++]);
    //         while ( i < length) {
    //             sb.Append(", ");
    //             sb.Append((*intArray)[i++]);
    //         }
    //     }
    //     sb.Append("}");
    //     s = sb.ToString();
    // }
    // else if (ret instanceof String[]) {
    //     AutoPtr<ArrayOf<String> > strings = (String[]) ret;
    //     length = strings->GetLength();
    //     sb = new StringBuilder("{");
    //     if (length > 0) {
    //         Int32 i = 0;
    //         sb.append(strings[i++]);
    //         while ( i < length) {
    //             sb.Append(", ");
    //             sb.Append(strings[i++]);
    //         }
    //     }
    //     sb.Append("}");
    //     s = sb.toString();
    // }
    // else if (req == IRILConstants::RIL_REQUEST_GET_CURRENT_CALLS) {
    //     AutoPtr<IArrayList> calls = IArrayList::Probe(ret);
    //     sb = new StringBuilder(" ");
    //     for (DriverCall dc : calls) {
    //         sb.Append("[");
    //         sb.Append(dc);
    //         sb.Append("] ");
    //     }
    //     s = sb.toString();
    // }
    // else if (req == RIL_REQUEST_GET_NEIGHBORING_CELL_IDS) {
    //     AutoPtr<IArrayList> cells = IArrayList::Probe(ret); // ArrayList<NeighboringCellInfo>
    //     sb = new StringBuilder(" ");
    //     for (NeighboringCellInfo cell : cells) {
    //         sb.Append(cell);
    //         sb.Append(" ");
    //     }
    //     s = sb.ToString();
    // }
    // else if (req == IRILConstants::RIL_REQUEST_GET_HARDWARE_CONFIG) {
    //     AutoPtr<IArrayList> hwcfgs = IArrayList::Probe(ret); // ArrayList<HardwareConfig>
    //     sb = new StringBuilder(" ");
    //     for (HardwareConfig hwcfg : hwcfgs) {
    //         sb.Append("[");
    //         sb.Append(hwcfg);
    //         sb.Append("] ");
    //     }
    //     s = sb.ToString();
    // }
    // else {
    //     s = ret.ToString();
    // }
    return s;
}

void RIL::ProcessUnsolicited(
    /* [in] */ RILParcel* p)
{
    Int32 response = 0;
    AutoPtr<IInterface> ret;

    p->ReadInt32(&response);

    //Logger::E("leliang", "line:%d, func:%s, response:%d\n", __LINE__, __func__, response);
    // try
    switch(response) {
        /*
        cat libs/telephony/ril_unsol_commands.h \
        | egrep "^ *{RIL_" \
        | sed -re 's/\{([^,]+),[^,]+,([^}]+).+/case \1: \2(rr, p); break;/'
        */

        case IRILConstants::RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS: ret =  ResponseString(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: ret =  ResponseString(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: ret =  ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_ON_USSD: ret =  ResponseStrings(p); break;
        case IRILConstants::RIL_UNSOL_NITZ_TIME_RECEIVED: ret =  ResponseString(p); break;
        case IRILConstants::RIL_UNSOL_SIGNAL_STRENGTH: ret = ResponseSignalStrength(p); break;
        case IRILConstants::RIL_UNSOL_DATA_CALL_LIST_CHANGED: ret = ResponseDataCallList(p);break;
        case IRILConstants::RIL_UNSOL_SUPP_SVC_NOTIFICATION: ret = ResponseSuppServiceNotification(p); break;
        case IRILConstants::RIL_UNSOL_STK_SESSION_END: ret = ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_STK_PROACTIVE_COMMAND: ret = ResponseString(p); break;
        case IRILConstants::RIL_UNSOL_STK_EVENT_NOTIFY: ret = ResponseString(p); break;
        case IRILConstants::RIL_UNSOL_STK_CALL_SETUP: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_SIM_SMS_STORAGE_FULL: ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_SIM_REFRESH: ret =  ResponseSimRefresh(p); break;
        case IRILConstants::RIL_UNSOL_CALL_RING: ret =  ResponseCallRing(p); break;
        case IRILConstants::RIL_UNSOL_RESTRICTED_STATE_CHANGED: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED:  ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_CDMA_NEW_SMS:  ret =  ResponseCdmaSms(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS:  ret =  ResponseRaw(p); break;
        case IRILConstants::RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL:  ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: ret = ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_CDMA_CALL_WAITING: ret = ResponseCdmaCallWaiting(p); break;
        case IRILConstants::RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_CDMA_INFO_REC: ret = ResponseCdmaInformationRecord(p); break;
        case IRILConstants::RIL_UNSOL_OEM_HOOK_RAW: ret = ResponseRaw(p); break;
        case IRILConstants::RIL_UNSOL_RINGBACK_TONE: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_RESEND_INCALL_MUTE: ret = ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOl_CDMA_PRL_CHANGED: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: ret = ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_RIL_CONNECTED: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: ret =  ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_CELL_INFO_LIST: ret = ResponseCellInfoList(p); break;
        case IRILConstants::RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: ret =  ResponseVoid(p); break;
        case IRILConstants::RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: ret =  ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_SRVCC_STATE_NOTIFY: ret = ResponseInts(p); break;
        case IRILConstants::RIL_UNSOL_HARDWARE_CONFIG_CHANGED: ret = ResponseHardwareConfig(p); break;
        case IRILConstants::RIL_UNSOL_ON_SS: ret =  ResponseSsData(p); break;
        case IRILConstants::RIL_UNSOL_STK_CC_ALPHA_NOTIFY: ret =  ResponseString(p); break;
        case IRILConstants::RIL_UNSOL_STK_SEND_SMS_RESULT: ret = ResponseInts(p); break; // Samsung STK

        default:
            // throw new RuntimeException("Unrecognized unsol response: " + response);
            Logger::E("RILJ", "Unrecognized unsol response:%d", response);
            assert(0);
        break; // (implied)
    }
    // } catch (Throwable tr) {
    //     Rlog.e(RILJ_LOG_TAG, "Exception processing unsol response: " + response +
    //         "Exception:" + tr.toString());
    //     return;
    // }

    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
    switch(response) {
        case IRILConstants::RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: {
            /* has bonus radio state Int32 */
            Int32 st = 0;
            p->ReadInt32(&st);
            ICommandsInterfaceRadioState newState;
            GetRadioStateFromInt(st, &newState);
            if (RILJ_LOGD) UnsljLogMore(response, StringUtils::ToString(newState));

            SwitchToRadioState(newState);
        break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: {
            if (RILJ_LOGD) {
                UnsljLog(response);
            }

            mImsNetworkStateChangedRegistrants->NotifyRegistrants(ar);
        break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: {
            if (RILJ_LOGD) UnsljLog(response);

            mCallStateRegistrants->NotifyRegistrants(ar);
        break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: {
            if (RILJ_LOGD) UnsljLog(response);

            mVoiceNetworkStateRegistrants->NotifyRegistrants(ar);
        break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS: {
            if (RILJ_LOGD) UnsljLog(response);

            // FIXME this should move up a layer
            AutoPtr<ArrayOf<String> > a = ArrayOf<String>::Alloc(2);

            AutoPtr<ICharSequence> pRet = ICharSequence::Probe(ret);
            String value;
            pRet->ToString(&value);
            a->Set(1, value);
            AutoPtr<ISmsMessage> sms;
            AutoPtr<ISmsMessageHelper> hlp;

            CSmsMessageHelper::AcquireSingleton((ISmsMessageHelper**)&hlp);
            hlp->NewFromCMT(a, (ISmsMessage**)&sms);

            //Logger::E("leliang", "received sms: %s", TO_CSTR(sms));
            if (mGsmSmsRegistrant != NULL) {
                //Logger::E("leliang", "line:%d, func:%s, sms:%p\n", __LINE__, __func__, sms.Get());
                AutoPtr<AsyncResult> arSms = new AsyncResult(NULL, sms, NULL);
                mGsmSmsRegistrant->NotifyRegistrant(arSms);
            }
        break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mSmsStatusRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mSmsStatusRegistrant->NotifyRegistrant(arRet);
            }
        break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: {
            if (RILJ_LOGD) {
                UnsljLogRet(response, ret);
            }

            assert(0 && "TODO");
            AutoPtr<ArrayOf<Int32> > smsIndex;// = (Int32[])ret;

            if(smsIndex->GetLength() == 1) {
                if (mSmsOnSimRegistrant != NULL) {
                    assert(0 && "TODO");
                    AutoPtr<AsyncResult> arSmsIndex;// = new AsyncResult(NULL, smsIndex, NULL);
                    mSmsOnSimRegistrant->NotifyRegistrant(arSmsIndex);
                }
            }
            else {
                if (RILJ_LOGD) {
                    // String str(" NEW_SMS_ON_SIM ERROR with wrong length ");
                    // str += smsIndex->GetLength();
                    // RiljLog(str);
                }
            }
        break;
        }
        case IRILConstants::RIL_UNSOL_ON_USSD: {
            assert(0 && "TODO");
            AutoPtr<ArrayOf<String> > resp; // = (String[])ret;

            if (resp->GetLength() < 2) {
                resp = ArrayOf<String>::Alloc(2);
                assert(0 && "TODO");
                // (*resp)[0] = ((String[])ret)[0];
                (*resp)[1] = NULL;
            }
            if (RILJ_LOGD) UnsljLogMore(response, (*resp)[0]);
            if (mUSSDRegistrant != NULL) {
                assert(0 && "TODO");
                AutoPtr<AsyncResult> ar;// = new AsyncResult(NULL, resp, NULL);
                mUSSDRegistrant->NotifyRegistrant(ar);
            }
        break;
        }
        case IRILConstants::RIL_UNSOL_NITZ_TIME_RECEIVED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            // has bonus long containing milliseconds since boot that the NITZ
            // time was received
            Int64 nitzReceiveTime = 0;
            p->ReadInt64(&nitzReceiveTime);

            AutoPtr<IArrayOf> result;
            CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&result);

            result->Set(0, ret);
            AutoPtr<IInteger64> pTime;
            CInteger64::New(nitzReceiveTime, (IInteger64**)&pTime);
            result->Set(1, pTime);

            AutoPtr<ISystemProperties> sp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
            Boolean ignoreNitz = FALSE;
            sp->GetBoolean(
                    ITelephonyProperties::PROPERTY_IGNORE_NITZ, FALSE, &ignoreNitz);

            if (ignoreNitz) {
                if (RILJ_LOGD) RiljLog(String("ignoring UNSOL_NITZ_TIME_RECEIVED"));
            }
            else {
                if (mNITZTimeRegistrant != NULL) {
                    AutoPtr<AsyncResult> arResult = new AsyncResult(NULL, result, NULL);
                    mNITZTimeRegistrant->NotifyRegistrant(arResult);
                }
                else {
                    // in case NITZ time registrant isnt registered yet
                    mLastNITZTimeInfo = result;
                }
            }
        break;
        }

        case IRILConstants::RIL_UNSOL_SIGNAL_STRENGTH: {
            // Note this is set to "verbose" because it happens
            // frequently
            if (RILJ_LOGV) UnsljLogvRet(response, ret);

            if (mSignalStrengthRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mSignalStrengthRegistrant->NotifyRegistrant(arRet);
            }
        break;
        }
        case IRILConstants::RIL_UNSOL_DATA_CALL_LIST_CHANGED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            Boolean oldRil = FALSE;
            NeedsOldRilFeature(String("skipbrokendatacall"), &oldRil);
            AutoPtr<IArrayList> retList = IArrayList::Probe(ret);
            AutoPtr<IInterface> pNode;
            retList->Get(0, (IInterface**)&pNode);
            AutoPtr<IDataCallResponse> _pNode = IDataCallResponse::Probe(pNode);
            assert(0 && "TODO");
            // if (oldRil && _pNode->mType.Equals("IP"))
            //     break;

            AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
            mDataNetworkStateRegistrants->NotifyRegistrants(arRet);
        break;
        }
        case IRILConstants::RIL_UNSOL_SUPP_SVC_NOTIFICATION: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mSsnRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mSsnRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_STK_SESSION_END: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mCatSessionEndRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCatSessionEndRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_STK_PROACTIVE_COMMAND: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mCatProCmdRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCatProCmdRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_STK_EVENT_NOTIFY: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mCatEventRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCatEventRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_STK_CALL_SETUP: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mCatCallSetUpRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCatCallSetUpRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_SIM_SMS_STORAGE_FULL: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mIccSmsFullRegistrant != NULL) {
                mIccSmsFullRegistrant->NotifyRegistrant();
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_SIM_REFRESH: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mIccRefreshRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mIccRefreshRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_CALL_RING: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mRingRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mRingRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RESTRICTED_STATE_CHANGED: {
            if (RILJ_LOGD) UnsljLogvRet(response, ret);
            if (mRestrictedStateRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mRestrictedStateRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mIccStatusChangedRegistrants != NULL) {
                mIccStatusChangedRegistrants->NotifyRegistrants();
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: {
            if (RILJ_LOGD) UnsljLog(response);

            AutoPtr<ISmsMessage> sms = ISmsMessage::Probe(ret);

            if (mCdmaSmsRegistrant != NULL) {
                AutoPtr<AsyncResult> arSms = new AsyncResult(NULL, sms, NULL);
                mCdmaSmsRegistrant->NotifyRegistrant(arSms);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mGsmBroadcastSmsRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mGsmBroadcastSmsRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mIccSmsFullRegistrant != NULL) {
                mIccSmsFullRegistrant->NotifyRegistrant();
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: {
            if (RILJ_LOGD) UnsljLog(response);

            if (mEmergencyCallbackModeRegistrant != NULL) {
                mEmergencyCallbackModeRegistrant->NotifyRegistrant();
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_CDMA_CALL_WAITING: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mCallWaitingInfoRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCallWaitingInfoRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mOtaProvisionRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mOtaProvisionRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_CDMA_INFO_REC: {
            AutoPtr<IArrayList> listInfoRecs;

            // try {
                listInfoRecs = IArrayList::Probe(ret);
            // } catch (ClassCastException e) {
            //     Rlog.e(RILJ_LOG_TAG, "Unexpected exception casting to listInfoRecs", e);
            //     break;
            // }
            AutoPtr<IIterator> it;
            listInfoRecs->GetIterator((IIterator**)&it);
            Boolean bHasNext = FALSE;
            while ((it->HasNext(&bHasNext), bHasNext)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<ICdmaInformationRecords> rec = ICdmaInformationRecords::Probe(p);
                if (RILJ_LOGD) UnsljLogRet(response, rec);
                NotifyRegistrantsCdmaInfoRec(rec);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_OEM_HOOK_RAW: {
            // if (RILJ_LOGD) UnsljLogvRet(response, StringUtils::ToHexString((Byte[]) ret));
            AutoPtr<IByteBufferHelper> bbf;
            CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbf);
            AutoPtr<IByteBuffer> oemHookResponse;
            AutoPtr<IArrayOf> byteArray = IArrayOf::Probe(ret);
            Int32 size;
            byteArray->GetLength(&size);
            AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(size);
            for(Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> ele;
                byteArray->Get(i, (IInterface**)&ele);
                Byte b;
                IByte::Probe(ele)->GetValue(&b);
                array->Set(i, b);
            }
            bbf->Wrap(array, (IByteBuffer**)&oemHookResponse);
            AutoPtr<IByteOrderHelper> bod;
            CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&bod);
            ByteOrder bo;
            bod->GetNativeOrder(&bo);
            oemHookResponse->SetOrder(bo);
            if (IsQcUnsolOemHookResp(oemHookResponse)) {
                //assert(0 && "TODO");
                // Rlog.d(RILJ_LOG_TAG, "OEM ID check Passed");
                Logger::D(RILJ_LOG_TAG, "OEM ID check Passed");
                ProcessUnsolOemhookResponse(oemHookResponse);
            }
            else if (mUnsolOemHookRawRegistrant != NULL) {
                //assert(0 && "TODO");
                // Rlog.d(RILJ_LOG_TAG, "External OEM message, to be notified");
                Logger::D("leliang", "External OEM message, to be notified");
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mUnsolOemHookRawRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RINGBACK_TONE: {
            if (RILJ_LOGD) UnsljLogvRet(response, ret);
            if (mRingbackToneRegistrants != NULL) {
                assert(0 && "TODO");
                Boolean playtone;// = (((Int32[])ret)[0] == 1);
                AutoPtr<IBoolean> _playtone;
                CBoolean::New(playtone, (IBoolean**)&_playtone);
                AutoPtr<AsyncResult> arPlaytone = new AsyncResult(NULL, _playtone, NULL);
                mRingbackToneRegistrants->NotifyRegistrants(arPlaytone);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RESEND_INCALL_MUTE: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mResendIncallMuteRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mResendIncallMuteRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mVoiceRadioTechChangedRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mVoiceRadioTechChangedRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mCdmaSubscriptionChangedRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCdmaSubscriptionChangedRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOl_CDMA_PRL_CHANGED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mCdmaPrlChangedRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCdmaPrlChangedRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mExitEmergencyCallbackModeRegistrants != NULL) {
                mExitEmergencyCallbackModeRegistrants->NotifyRegistrants(ar);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_RIL_CONNECTED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            // Initial conditions
            SetRadioPower(FALSE, NULL);
            SetPreferredNetworkType(mPreferredNetworkType, NULL);
            SetCdmaSubscriptionSource(mCdmaSubscription, NULL);
            SetCellInfoListRate(Elastos::Core::Math::INT32_MAX_VALUE, NULL);
            // NotifyRegistrantsRilConnectionChanged(((Int32[])ret)[0]);
            break;
        }
        case IRILConstants::RIL_UNSOL_CELL_INFO_LIST: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mRilCellInfoListRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mRilCellInfoListRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mSubscriptionStatusRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mSubscriptionStatusRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_SRVCC_STATE_NOTIFY: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mSrvccStateRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mSrvccStateRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_HARDWARE_CONFIG_CHANGED: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mHardwareConfigChangeRegistrants != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mHardwareConfigChangeRegistrants->NotifyRegistrants(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_ON_SS: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mSsRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mSsRegistrant->NotifyRegistrant(arRet);
            }
            break;
        }
        case IRILConstants::RIL_UNSOL_STK_CC_ALPHA_NOTIFY: {
            if (RILJ_LOGD) UnsljLogRet(response, ret);

            if (mCatCcAlphaRegistrant != NULL) {
                AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                mCatCcAlphaRegistrant->NotifyRegistrant(arRet);
            }
            break; // Samsung STK
        }
        case IRILConstants::RIL_UNSOL_STK_SEND_SMS_RESULT: {
            AutoPtr<IResourcesHelper> hlp;
            CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
            AutoPtr<IResources> res;
            hlp->GetSystem((IResources**)&res);
            Boolean b = FALSE;
            res->GetBoolean(R::bool_::config_samsung_stk, &b);
            if (b) {
                if (RILJ_LOGD) UnsljLogRet(response, ret);

                if (mCatSendSmsResultRegistrant != NULL) {
                    AutoPtr<AsyncResult> arRet = new AsyncResult(NULL, ret, NULL);
                    mCatSendSmsResultRegistrant->NotifyRegistrant(arRet);
                }
            }
            break;
        }
    }
}

void RIL::NotifyRegistrantsRilConnectionChanged(
    /* [in] */ Int32 rilVer)
{
    mRilVersion = rilVer;
    if (mRilConnectedRegistrants != NULL) {
        AutoPtr<IInteger32> p;
        CInteger32::New(rilVer, (IInteger32**)&p);
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, p, NULL);
        mRilConnectedRegistrants->NotifyRegistrants(ar);
    }
}

Boolean RIL::IsQcUnsolOemHookResp(
    /* [in] */ IByteBuffer* oemHookResponse)
{
    /* Check OEM ID in UnsolOemHook response */
    Int32 cap = 0;
    IBuffer::Probe(oemHookResponse)->GetCapacity(&cap);
    if (cap < mHeaderSize) {
        /*
         * size of UnsolOemHook message is less than expected, considered as
         * External OEM's message
         */
        // Rlog.d(RILJ_LOG_TAG,
        //         "RIL_UNSOL_OEM_HOOK_RAW data size is " + oemHookResponse.capacity());
        return FALSE;
    }
    else {
        AutoPtr<ArrayOf<Byte> > oemIdBytes = ArrayOf<Byte>::Alloc(OEM_IDENTIFIER.GetLength());
        oemHookResponse->Get(oemIdBytes);
        String oemIdString;// = new String(oemIdBytes);
        // Rlog.d(RILJ_LOG_TAG, "Oem ID in RIL_UNSOL_OEM_HOOK_RAW is " + oemIdString);
        if (!oemIdString.Equals(OEM_IDENTIFIER)) {
            /* OEM ID not matched, considered as External OEM's message */
            return FALSE;
        }
    }
    return TRUE;
}

void RIL::ProcessUnsolOemhookResponse(
    /* [in] */ IByteBuffer* oemHookResponse)
{
    Int32 responseId = 0, responseSize = 0; //responseVoiceId = 0;

    oemHookResponse->GetInt32(&responseId);
    // Rlog.d(RILJ_LOG_TAG, "Response ID in RIL_UNSOL_OEM_HOOK_RAW is " + responseId);

    oemHookResponse->GetInt32(&responseSize);
    if (responseSize < 0) {
        // Rlog.e(RILJ_LOG_TAG, "Response Size is Invalid " + responseSize);
        return;
    }

    AutoPtr<ArrayOf<Byte> > responseData = ArrayOf<Byte>::Alloc(responseSize);
    Int32 remain = 0;
    IBuffer::Probe(oemHookResponse)->GetRemaining(&remain);
    if (remain == responseSize) {
        oemHookResponse->Get(responseData, 0, responseSize);
    }
    else {
        // Rlog.e(RILJ_LOG_TAG, "Response Size(" + responseSize
        //         + ") doesnot match remaining bytes(" +
        //         oemHookResponse.remaining() + ") in the buffer. So, don't process further");
        return;
    }

    switch (responseId) {
        case 0x80000 + 1018:    // OEMHOOK_UNSOL_WWAN_IWLAN_COEXIST
            NotifyWwanIwlanCoexist(responseData);
            break;

        case 0x80000 + 1016:    // OEMHOOK_UNSOL_SIM_REFRESH
            NotifySimRefresh(responseData);
            break;

        case 0x80000 + 1020:    // QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY
            // Rlog.d(RILJ_LOG_TAG, "QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY = mInstanceId"
            //         + mInstanceId);
            NotifyModemCap(responseData, mInstanceId);
            break;

        default:
            // Rlog.d(RILJ_LOG_TAG, "Response ID " + responseId
            //         + " is not served in this process.");
            break;
    }
}

void RIL::NotifyWwanIwlanCoexist(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, CoreUtils::ConvertByteArray(data), NULL);
    mWwanIwlanCoexistenceRegistrants->NotifyRegistrants(ar);
    Logger::D(RILJ_LOG_TAG, "WWAN, IWLAN coexistence notified to registrants");
}

void RIL::NotifySimRefresh(
    /* [in] */ ArrayOf<Byte>* data)
{
    Int32 len = data->GetLength();
    AutoPtr<ArrayOf<Byte> > userdata = ArrayOf<Byte>::Alloc(1 + len);
    data->Copy(userdata, 0, len);
    //Add slot id in SIM_REFRESH event to notify framework: IccRecords.
    if (mInstanceId == NULL) {
        (*userdata)[len] = 0;
    }
    else {
        Int32 id = 0;
        mInstanceId->GetValue(&id);
        (*userdata)[len] = (Byte)(id & 0xFF);
    }

    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, CoreUtils::ConvertByteArray(userdata), NULL);
    mSimRefreshRegistrants->NotifyRegistrants(ar);
    Logger::D(RILJ_LOG_TAG, "SIM_REFRESH notified to registrants");
}

void RIL::NotifyModemCap(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IInteger32* phoneId)
{
    Int32 id = 0;
    phoneId->GetValue(&id);
    AutoPtr<UnsolOemHookBuffer> buffer = new UnsolOemHookBuffer(id, data);

    //Had notifyRegistrants not discarded userObj, we could have easily
    //passed the subId as ar.userObj.
    AutoPtr<AsyncResult> ar = new AsyncResult(NULL, TO_IINTERFACE(buffer), NULL);

    mModemCapRegistrants->NotifyRegistrants(ar);
    // Rlog.d(RILJ_LOG_TAG, "MODEM_CAPABILITY on phone=" + phoneId + " notified to registrants");
}

AutoPtr<IInterface> RIL::ResponseInts(
    /* [in] */ RILParcel* p)
{
    Int32 numInts = 0;

    p->ReadInt32(&numInts);

    AutoPtr<ArrayOf<Int32> > response = ArrayOf<Int32>::Alloc(numInts);

    for (Int32 i = 0 ; i < numInts ; i++) {
        Int32 value;
        p->ReadInt32(&value);
        //Logger::E("leliang", "line:%d, func:%s, index:%d, value:%d\n", __LINE__, __func__, i, value);
        response->Set(i, value);
    }

    return CoreUtils::Convert(response);
}

AutoPtr<IInterface> RIL::ResponseVoid(
    /* [in] */ RILParcel* p)
{
    return NULL;
}

AutoPtr<IInterface> RIL::ResponseCallForward(
    /* [in] */ RILParcel* p)
{
    Int32 numInfos = 0;

    p->ReadInt32(&numInfos);

    AutoPtr<ArrayOf<ICallForwardInfo*> > infos = ArrayOf<ICallForwardInfo*>::Alloc(numInfos);

    for (Int32 i = 0 ; i < numInfos ; i++) {
        infos->Set(i, new CallForwardInfo());
        AutoPtr<CallForwardInfo> pInfo = (CallForwardInfo*)((*infos)[i]);

        Int32 status = 0;
        p->ReadInt32(&status);
        pInfo->mStatus = status;

        Int32 reason = 0;
        p->ReadInt32(&reason);
        pInfo->mReason = reason;

        Int32 serviceClass = 0;
        p->ReadInt32(&serviceClass);
        pInfo->mServiceClass = serviceClass;

        Int32 toa = 0;
        p->ReadInt32(&toa);
        pInfo->mToa = toa;

        String number;
        p->ReadString(&number);
        pInfo->mNumber = number;

        Int32 timeSeconds = 0;
        p->ReadInt32(&timeSeconds);
        pInfo->mTimeSeconds = timeSeconds;
    }

    AutoPtr<IArrayOf> array = CoreUtils::Convert(infos, EIID_ICallForwardInfo);
    return array;
}

AutoPtr<IInterface> RIL::ResponseSuppServiceNotification(
    /* [in] */ RILParcel* p)
{
    AutoPtr<CSuppServiceNotification> notification;
    CSuppServiceNotification::NewByFriend((CSuppServiceNotification**)&notification);

    Int32 notificationType = 0;
    p->ReadInt32(&notificationType);
    notification->mNotificationType = notificationType;

    Int32 code = 0;
    p->ReadInt32(&code);
    notification->mCode = code;

    Int32 index = 0;
    p->ReadInt32(&index);
    notification->mIndex = index;

    Int32 type = 0;
    p->ReadInt32(&type);
    notification->mType = type;

    String number;
    p->ReadString(&number);
    notification->mNumber = number;

    return notification->Probe(EIID_IInterface);
}

AutoPtr<IInterface> RIL::ResponseCdmaSms(
    /* [in] */ RILParcel* p)
{
    AutoPtr<ISmsMessage> sms;
    assert(0 && "TODO");
    // sms = SmsMessage::NewFromParcel(p);

    return sms;
}

AutoPtr<IInterface> RIL::ResponseString(
    /* [in] */ RILParcel* p)
{
    String response;

    p->ReadString(&response);

    //Logger::E("leliang", "line:%d, func:%s, response:%s\n", __LINE__, __func__, response.string());
    AutoPtr<ICharSequence> pResult;
    CString::New(response, (ICharSequence**)&pResult);
    return pResult;
}

AutoPtr<IInterface> RIL::ResponseStrings(
    /* [in] */ RILParcel* p)
{
    //Int32 num;
    AutoPtr<ArrayOf<String> > response;

    //now read string error
    p->ReadStringArray((ArrayOf<String>**)&response);
    for(Int32 i = 0; i < response->GetLength(); ++i) {
        if ((*response)[i] == NULL)
            (*response)[i] = String("");
        //Logger::E("leliang", "RIL::ResponseStrings, %d is %s", i, ((*response)[i]).string());
    }
    //fake code
    //AutoPtr<ArrayOf<String> > test;
    //test = ArrayOf<String>::Alloc(2);
    //(*test)[0] = String("Hello");
    //(*test)[1] = String("World");

    return CoreUtils::Convert(response);
}

AutoPtr<IInterface> RIL::ResponseRaw(
    /* [in] */ RILParcel* p)
{
    //Int32 num;
    AutoPtr<ArrayOf<Byte> > response;

    p->CreateByteArray((ArrayOf<Byte>**)&response);

    return CoreUtils::ConvertByteArray(response);
}

AutoPtr<IInterface> RIL::ResponseSMS(
    /* [in] */ RILParcel* p)
{
    Int32 messageRef = 0, errorCode = 0;
    String ackPDU;

    p->ReadInt32(&messageRef);
    p->ReadString(&ackPDU);
    p->ReadInt32(&errorCode);

    AutoPtr<ISmsResponse> response;
    CSmsResponse::New(messageRef, ackPDU, errorCode, (ISmsResponse**)&response);

    return response;
}

AutoPtr<IInterface> RIL::ResponseICC_IO(
    /* [in] */ RILParcel* p)
{
    Int32 sw1 = 0, sw2 = 0;
    AutoPtr<IMessage> ret;

    p->ReadInt32(&sw1);
    p->ReadInt32(&sw2);

    String s;
    p->ReadString(&s);

    if (RILJ_LOGV) {
        String str("< iccIO: ");
        str += " 0x";
        str += StringUtils::ToHexString(sw1);
        str += " 0x";
        str += StringUtils::ToHexString(sw2);
        str += " ";
        str += s;
        RiljLog(str);
    }

    AutoPtr<IIccIoResult> res;
    CIccIoResult::New(sw1, sw2, s, (IIccIoResult**)&res);
    return res;
}

AutoPtr<IInterface> RIL::ResponseICC_IOBase64(
    /* [in] */ RILParcel* p)
{
    Int32 sw1 = 0, sw2 = 0;
    AutoPtr<IMessage> ret;

    p->ReadInt32(&sw1);
    p->ReadInt32(&sw2);

    String s;
    p->ReadString(&s);

    if (RILJ_LOGV) {
        String str("< iccIO: ");
        str += " 0x";
        str += StringUtils::ToHexString(sw1);
        str += " 0x";
        str += StringUtils::ToHexString(sw2);
        str += " ";
        str += s;
        RiljLog(str);
    }

    AutoPtr<IBase64> base64;
    CBase64::AcquireSingleton((IBase64**)&base64);
    AutoPtr<ArrayOf<Byte> > vs;
    base64->Decode(s, IBase64::DEFAULT, (ArrayOf<Byte>**)&vs);
    AutoPtr<IIccIoResult> res;
    CIccIoResult::New(sw1, sw2, vs, (IIccIoResult**)&res);
    return res;
}

ECode RIL::NeedsOldRilFeature(
    /* [in] */ const String& feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String res;
    sp->Get(String("ro.telephony.ril.config"), String(""), &res);
    AutoPtr<ArrayOf<String> > features;
    StringUtils::Split(res, String(","), (ArrayOf<String>**)&features);
    for (Int32 i = 0; i < features->GetLength(); i++) {
        String found = (*features)[i];
        if (found.Equals(feature)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<IInterface> RIL::ResponseIccCardStatus(
    /* [in] */ RILParcel* p)
{
    //Logger::E("leliang", "line:%d, func:%s\n", __LINE__, __func__);
    AutoPtr<IccCardApplicationStatus> appStatus;

    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("icccardstatus"), &oldRil);

    AutoPtr<CIccCardStatus> cardStatus;
    CIccCardStatus::NewByFriend((CIccCardStatus**)&cardStatus);
    Int32 cardState = 0;
    p->ReadInt32(&cardState);
    cardStatus->SetCardState(cardState);
    Int32 pinState = 0;
    p->ReadInt32(&pinState);
    cardStatus->SetUniversalPinState(pinState);
    p->ReadInt32(&(cardStatus->mGsmUmtsSubscriptionAppIndex));
    p->ReadInt32(&(cardStatus->mCdmaSubscriptionAppIndex));

    if (!oldRil) {
        p->ReadInt32(&(cardStatus->mImsSubscriptionAppIndex));
    }

    Int32 numApplications = 0;
    p->ReadInt32(&numApplications);

    // limit to maximum allowed applications
    if (numApplications > IIccCardStatus::CARD_MAX_APPS) {
        numApplications = IIccCardStatus::CARD_MAX_APPS;
    }
    cardStatus->mApplications = ArrayOf<IIccCardApplicationStatus*>::Alloc(numApplications);

    for (Int32 i = 0 ; i < numApplications ; i++) {
        appStatus = new IccCardApplicationStatus();
        Int32 typeFromRILInt = 0;
        p->ReadInt32(&typeFromRILInt);
        appStatus->AppTypeFromRILInt(typeFromRILInt, &appStatus->mApp_type);

        Int32 stateFromRILInt = 0;
        p->ReadInt32(&stateFromRILInt);
        appStatus->AppStateFromRILInt(stateFromRILInt, &appStatus->mApp_state);

        Int32 perso_substate = 0;
        p->ReadInt32(&perso_substate);
        appStatus->PersoSubstateFromRILInt(perso_substate, &appStatus->mPerso_substate);

        p->ReadString(&(appStatus->mAid));
        p->ReadString(&(appStatus->mApp_label));
        p->ReadInt32(&(appStatus->mPin1_replaced));
        Int32 pin1 = 0;
        p->ReadInt32(&pin1);
        appStatus->PinStateFromRILInt(pin1, &appStatus->mPin1);
        Int32 pin2 = 0;
        p->ReadInt32(&pin2);
        appStatus->PinStateFromRILInt(pin2, &appStatus->mPin2);
        cardStatus->mApplications->Set(i, (IIccCardApplicationStatus*)appStatus);
    }
    return cardStatus->Probe(EIID_IInterface);
}

AutoPtr<IInterface> RIL::ResponseSimRefresh(
    /* [in] */ RILParcel* p)
{
    AutoPtr<IccRefreshResponse> response = new IccRefreshResponse();

    p->ReadInt32(&(response->mRefreshResult));
    p->ReadInt32(&(response->mEfId));
    p->ReadString(&(response->mAid));
    return TO_IINTERFACE(response);
}

AutoPtr<IInterface> RIL::ResponseCallList(
    /* [in] */ RILParcel* p)
{
    Int32 num = 0;
    Int32 voiceSettings = 0;
    AutoPtr<IArrayList> response;
    AutoPtr<DriverCall> dc;

    p->ReadInt32(&num);
    CArrayList::New(num, (IArrayList**)&response);

    if (RILJ_LOGV) {
        // String str("responseCallList: num=");
        // str += num;
        // str += " mEmergencyCallbackModeRegistrant=";
        // assert(0 && "TODO");
        // // str += mEmergencyCallbackModeRegistrant.Get();
        // str += " mTestingEmergencyCall=";
        // // str += mTestingEmergencyCall.Get();
        // RiljLog(str);
    }
    //Logger::E("leliang", "line:%d, func:%s, num:%d", __LINE__, __func__, num);
    for (Int32 i = 0; i < num; i++) {
        dc = new DriverCall();

        Int32 state = 0;
        p->ReadInt32(&state);
        DriverCall::StateFromCLCC(state, &(dc->mState));
        p->ReadInt32(&(dc->mIndex));
        p->ReadInt32(&(dc->mTOA));
        Int32 iMpty = 0;
        p->ReadInt32(&iMpty);
        dc->mIsMpty = (0 != iMpty);
        Int32 iMT = 0;
        p->ReadInt32(&iMT);
        dc->mIsMT = (0 != iMT);
        p->ReadInt32(&(dc->mAls));
        p->ReadInt32(&voiceSettings);
        dc->mIsVoice = (0 == voiceSettings) ? FALSE : TRUE;
        Int32 isVoicePrivacy = 0;
        p->ReadInt32(&isVoicePrivacy);
        dc->mIsVoicePrivacy = (0 != isVoicePrivacy);
        p->ReadString(&(dc->mNumber));
        Int32 np = 0;
        p->ReadInt32(&np);
        DriverCall::PresentationFromCLIP(np, &(dc->mNumberPresentation));
        p->ReadString(&(dc->mName));
        // according to ril.h, namePresentation should be handled as numberPresentation;
        Int32 namePresentation = 0;
        p->ReadInt32(&namePresentation);
        DriverCall::PresentationFromCLIP(namePresentation, &(dc->mNamePresentation));
        Int32 uusInfoPresent = 0;
        p->ReadInt32(&uusInfoPresent);
        if (uusInfoPresent == 1) {
            dc->mUusInfo = new UUSInfo();
            Int32 type = 0;
            p->ReadInt32(&type);
            dc->mUusInfo->SetType(type);
            Int32 dcs = 0;
            p->ReadInt32(&dcs);
            dc->mUusInfo->SetDcs(dcs);
            AutoPtr<ArrayOf<Byte> > userData;
            p->CreateByteArray((ArrayOf<Byte>**)&userData);
            dc->mUusInfo->SetUserData(userData);
            String str1("Incoming UUS : type=");
            // str1 += dc->mUusInfo->GetType();
            str1 += ", dcs=";
            // str1 += dc->mUusInfo->GetDcs();
            str1 += ", length=";
            // str1 += dc->mUusInfo->GetUserData()->GetLength();
            RiljLogv(str1);

            String str2("Incoming UUS : data (string)=");
            // str2 += new String(dc->mUusInfo->GetUserData());
            RiljLogv(str2);

            String str3("Incoming UUS : data (hex): ");
            // str3 += StringUtils::ToHexString(dc->mUusInfo->GetUserData());
            RiljLogv(str3);
        }
        else {
            RiljLogv(String("Incoming UUS : NOT present!"));
        }

        // Make sure there's a leading + on addresses with a TOA of 145
        // dc->mNumber = PhoneNumberUtils::StringFromStringAndTOA(dc->mNumber, dc->mTOA);

        response->Add(TO_IINTERFACE(dc));
        //Logger::E("leliang", "line:%d, func:%s, dc_obj:%p\n", __LINE__, __func__, TO_IINTERFACE(dc));

        if (dc->mIsVoicePrivacy) {
            mVoicePrivacyOnRegistrants->NotifyRegistrants();
            RiljLog(String("InCall VoicePrivacy is enabled"));
            Logger::D(RILJ_LOG_TAG, "InCall VoicePrivacy is enabled");
        }
        else {
            mVoicePrivacyOffRegistrants->NotifyRegistrants();
            RiljLog(String("InCall VoicePrivacy is disabled"));
            Logger::D(RILJ_LOG_TAG, "InCall VoicePrivacy is disabled");
        }
    }

    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->Sort(IList::Probe(response));

    Boolean bGetAndSet = FALSE;
    mTestingEmergencyCall->GetAndSet(FALSE, &bGetAndSet);
    if ((num == 0) && bGetAndSet) {
        if (mEmergencyCallbackModeRegistrant != NULL) {
            String str("responseCallList: call ended, testing emergency call,");
            str += " notify ECM Registrants";
            RiljLog(str);
            mEmergencyCallbackModeRegistrant->NotifyRegistrant();
        }
    }

    return response;
}

AutoPtr<IDataCallResponse> RIL::GetDataCallResponse(
    /* [in] */ RILParcel* p,
    /* [in] */ Int32 version)
{
    assert(0 && "TODO");
    AutoPtr<IDataCallResponse> dataCall;// = new DataCallResponse();

    // dataCall->mVersion = version;
    if (version < 5) {
        // p->ReadInt32(&(dataCall->mCid));
        // p->ReadInt32(&(dataCall->mActive));
        // p->ReadString(&(dataCall->mType));
        Boolean oldRil = FALSE;
        NeedsOldRilFeature(String("datacallapn"), &oldRil);
        if (version < 4 || oldRil) {
            String APN;
            p->ReadString(&APN); // APN - not used
        }
        String addresses;
        p->ReadString(&addresses);
        if (!TextUtils::IsEmpty(addresses)) {
            // StringUtils::Split(addresses, String(" "), &(dataCall->mAddresses));
        }
        // DataCallState needs an ifname. Since we don't have one use the name from the ThrottleService resource (default=rmnet0).
        AutoPtr<IResourcesHelper> hlp;
        CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
        AutoPtr<IResources> res;
        hlp->GetSystem((IResources**)&res);
        // res->GetString(R::string::config_datause_iface, &(dataCall->mIfname));
    }
    else {
        // p->ReadInt32(&(dataCall->mStatus));
        Boolean oldRil = FALSE;
        NeedsOldRilFeature(String("usehcradio"), &oldRil);
        if (oldRil) {
            // dataCall->mSuggestedRetryTime = -1;
        }
        else {
            // p->ReadInt32(&(dataCall->mSuggestedRetryTime));
        }
        // p->ReadInt32(&(dataCall->mCid));
        // p->ReadInt32(&(dataCall->mActive));
        // p->ReadString(&(dataCall->mType));
        // p->ReadString(&(dataCall->mIfname));
        // if ((dataCall.status == DcFailCause.NONE.getErrorCode()) &&
                // TextUtils::IsEmpty(dataCall->mIfname)) {
          // throw new RuntimeException("getDataCallResponse, no ifname");
        // }
        String addresses;
        p->ReadString(&addresses);
        if (!TextUtils::IsEmpty(addresses)) {
            // StringUtils::Split(addresses, String(" "), &(dataCall->mAddresses));
        }
        String dnses;
        p->ReadString(&dnses);
        if (!TextUtils::IsEmpty(dnses)) {
            // StringUtils::Split(dnses, String(" "), &(dataCall->mDnses));
        }
        String gateways;
        p->ReadString(&gateways);
        if (!TextUtils::IsEmpty(gateways)) {
            // StringUtils::Split(gateways, String(" "), &(dataCall->mGateways));
        }
        if (version >= 10) {
            String pcscf;
            p->ReadString(&pcscf);
            if (!TextUtils::IsEmpty(pcscf)) {
                // StringUtils::Split(pcscf, String(" "), &(dataCall->mPcscf));
            }
            if (version >= 11) {
                // p->ReadInt32(&(dataCall->mMtu));
            }
        }
    }
    return dataCall;
}

AutoPtr<IInterface> RIL::ResponseDataCallList(
    /* [in] */ RILParcel* p)
{
    AutoPtr<IArrayList> response;
    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("datacall"), &oldRil);
    Int32 ver = 0;
    if (oldRil) {
        ver = 3;
    }
    else {
        p->ReadInt32(&ver);
    }
    Int32 num = 0;
    p->ReadInt32(&num);
    // String str("responseDataCallList ver=");
    // str += ver;
    // str += " num=";
    // str += num;
    // RiljLog(str);

    CArrayList::New(num, (IArrayList**)&response);
    for (Int32 i = 0; i < num; i++) {
        response->Add(GetDataCallResponse(p, ver));
    }

    return response;
}

AutoPtr<IInterface> RIL::ResponseSetupDataCall(
    /* [in] */ RILParcel* p)
{
    Boolean oldRil = FALSE;
    NeedsOldRilFeature(String("datacall"), &oldRil);
    Int32 ver = 0;
    if (oldRil) {
        ver = 3;
    }
    else {
        p->ReadInt32(&ver);
    }
    Int32 num = 0;
    p->ReadInt32(&num);
    if (RILJ_LOGV) {
        // String str("responseSetupDataCall ver=");
        // str += ver;
        // str += " num=";
        // str += num;
        // RiljLog(str);
    }

    AutoPtr<IDataCallResponse> dataCall;

    if (ver < 5) {
        assert(0 && "TODO");
        // CDataCallResponse::New((IDataCallResponse**)&dataCall);
        // dataCall->mVersion = ver;
        String cid;
        p->ReadString(&cid);
        // dataCall->mCid = StringUtils::ParseInt32(cid);
        // p->ReadString(&(dataCall->mIfname));
        // if (TextUtils::IsEmpty(dataCall->mIfname)) {
            // throw new RuntimeException(
            //         "RIL_REQUEST_SETUP_DATA_CALL response, no ifname");
            return NULL;
        // }
        String addresses;
        p->ReadString(&addresses);
        if (!TextUtils::IsEmpty(addresses)) {
            // StringUtils::Split(addresses, String(" "), &(dataCall->mAddresses))
        }
        if (num >= 4) {
            String dnses;
            p->ReadString(&dnses);
            if (RILJ_LOGD) {
                String str("responseSetupDataCall got dnses=");
                str += dnses;
                RiljLog(str);
            }
            if (!TextUtils::IsEmpty(dnses)) {
                // StringUtils::Split(dnses, String(" "), &(dataCall->mDnses));
            }
        }
        if (num >= 5) {
            String gateways;
            p->ReadString(&gateways);
            if (RILJ_LOGD) {
                String str("responseSetupDataCall got gateways=");
                str += gateways;
                RiljLog(str);
            }
            if (!TextUtils::IsEmpty(gateways)) {
                // StringUtils::Split(gateways, String(" "), &(dataCall->mGateways));
            }
        }
        if (num >= 6) {
            String pcscf;
            p->ReadString(&pcscf);
            if (RILJ_LOGD) {
                String str("responseSetupDataCall got pcscf=");
                str += pcscf;
                RiljLog(str);
            }
            if (!TextUtils::IsEmpty(pcscf)) {
                // StringUtils::Split(pcscf, String(" "), &(dataCall->mPcscf));
            }
        }
        if (num >= 7) {
            String mtu;
            p->ReadString(&mtu);
            // dataCall->mMtu = StringUtils::ParseInt32(mtu);
            if (RILJ_LOGD) {
                String str("responseSetupDataCall got mtu=");
                // str += dataCall->mMtu;
                RiljLog(str);
            }
        }
    }
    else {
        if (num != 1) {
            // throw new RuntimeException(
            //         "RIL_REQUEST_SETUP_DATA_CALL response expecting 1 RIL_Data_Call_response_v5"
            //         + " got " + num);
            return NULL;
        }
        dataCall = GetDataCallResponse(p, ver);
    }

    return dataCall;
}

AutoPtr<IInterface> RIL::ResponseOperatorInfos(
    /* [in] */ RILParcel* p)
{
    assert(0 && "TODO");
    AutoPtr<ArrayOf<String> > strings;// = (String [])ResponseStrings(p);
    AutoPtr<IArrayList> ret;

    if (strings->GetLength() % mQANElements != 0) {
        // throw new RuntimeException(
        //     "RIL_REQUEST_QUERY_AVAILABLE_NETWORKS: invalid response. Got "
        //     + strings.length + " strings, expected multiple of " + mQANElements);
        return NULL;
    }

    CArrayList::New(strings->GetLength() / mQANElements, (IArrayList**)&ret);

    for (Int32 i = 0; i < strings->GetLength(); i += mQANElements) {
        AutoPtr<IOperatorInfo> p;
        // COperatorInfo::New(
        //                 (*strings)[i+0],
        //                 (*strings)[i+1],
        //                 (*strings)[i+2],
        //                 (*strings)[i+3],
        //                 (IOperatorInfo**)&p);
        ret->Add(p);
    }

    return ret;
}

AutoPtr<IInterface> RIL::ResponseCellList(
    /* [in] */ RILParcel* p)
{
    Int32 num = 0, rssi = 0;
    String location;
    AutoPtr<IArrayList> response;
    AutoPtr<INeighboringCellInfo> cell;

    p->ReadInt32(&num);
    CArrayList::New((IArrayList**)&response);

    // Determine the radio access type
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String radioString;
    sp->Get(
            ITelephonyProperties::PROPERTY_DATA_NETWORK_TYPE, String("unknown"), &radioString);
    Int32 radioType = 0;
    if (radioString.Equals("GPRS")) {
        radioType = ITelephonyManager::NETWORK_TYPE_GPRS;
    }
    else if (radioString.Equals("EDGE")) {
        radioType = ITelephonyManager::NETWORK_TYPE_EDGE;
    }
    else if (radioString.Equals("UMTS")) {
        radioType = ITelephonyManager::NETWORK_TYPE_UMTS;
    }
    else if (radioString.Equals("HSDPA")) {
        radioType = ITelephonyManager::NETWORK_TYPE_HSDPA;
    }
    else if (radioString.Equals("HSUPA")) {
        radioType = ITelephonyManager::NETWORK_TYPE_HSUPA;
    }
    else if (radioString.Equals("HSPA")) {
        radioType = ITelephonyManager::NETWORK_TYPE_HSPA;
    }
    else {
        radioType = ITelephonyManager::NETWORK_TYPE_UNKNOWN;
    }

    // Interpret the location based on radio access type
    if (radioType != ITelephonyManager::NETWORK_TYPE_UNKNOWN) {
        for (Int32 i = 0 ; i < num ; i++) {
            p->ReadInt32(&rssi);
            p->ReadString(&location);
            assert(0 && "TODO");
            // cell = new NeighboringCellInfo(rssi, location, radioType);
            response->Add(cell);
        }
    }
    return response;
}

AutoPtr<IInterface> RIL::ResponseGetPreferredNetworkType(
    /* [in] */ RILParcel* p)
{
    assert(0 && "TODO");
    AutoPtr<ArrayOf<Int32> > response;// = (Int32[]) ResponseInts(p);

    if (response->GetLength() >= 1) {
        // Since this is the response for getPreferredNetworkType
        // we'll assume that it should be the value we want the
        // vendor ril to take if we reestablish a connection to it.
        mPreferredNetworkType = (*response)[0];
    }
    // return response;
    return NULL;
}

AutoPtr<IInterface> RIL::ResponseGmsBroadcastConfig(
    /* [in] */ RILParcel* p)
{
    Int32 num = 0;
    AutoPtr<IArrayList> response;
    AutoPtr<ISmsBroadcastConfigInfo> info;

    p->ReadInt32(&num);
    CArrayList::New(num, (IArrayList**)&response);

    for (Int32 i = 0; i < num; i++) {
        Int32 fromId = 0;
        p->ReadInt32(&fromId);
        Int32 toId = 0;
        p->ReadInt32(&toId);
        Int32 fromScheme = 0;
        p->ReadInt32(&fromScheme);
        Int32 toScheme = 0;
        p->ReadInt32(&toScheme);
        Int32 iSel = 0;
        p->ReadInt32(&iSel);
        Boolean selected = (iSel == 1);

        assert(0 && "TODO" && selected);
        // info = new SmsBroadcastConfigInfo(fromId, toId, fromScheme,
        //         toScheme, selected);
        response->Add(info);
    }
    return response;
}

AutoPtr<IInterface> RIL::ResponseCdmaBroadcastConfig(
    /* [in] */ RILParcel* p)
{
    Int32 numServiceCategories;
    AutoPtr<ArrayOf<Int32> > response;

    p->ReadInt32(&numServiceCategories);

    if (numServiceCategories == 0) {
        // TODO: The logic of providing default values should
        // not be done by this transport layer. And needs to
        // be done by the vendor ril or application logic.
        Int32 numInts = CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES * CDMA_BSI_NO_OF_INTS_STRUCT + 1;
        response = ArrayOf<Int32>::Alloc(numInts);

        // Faking a default record for all possible records.
        (*response)[0] = CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES;

        // Loop over CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES set 'english' as
        // default language and selection status to FALSE for all.
        for (Int32 i = 1; i < numInts; i += CDMA_BSI_NO_OF_INTS_STRUCT ) {
            (*response)[i + 0] = i / CDMA_BSI_NO_OF_INTS_STRUCT;
            (*response)[i + 1] = 1;
            (*response)[i + 2] = 0;
        }
    }
    else {
        Int32 numInts = (numServiceCategories * CDMA_BSI_NO_OF_INTS_STRUCT) + 1;
        response = ArrayOf<Int32>::Alloc(numInts);

        (*response)[0] = numServiceCategories;
        for (Int32 i = 1 ; i < numInts; i++) {
            p->ReadInt32(&((*response)[i]));
        }
    }

    assert(0 && "TODO");
    // return response;
    return NULL;
}

AutoPtr<IInterface> RIL::ResponseSignalStrength(
    /* [in] */ RILParcel* p)
{
    // Assume this is gsm, but doesn't matter as ServiceStateTracker
    // sets the proper value.
    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    AutoPtr<ArrayOf<Byte> > data;
    p->Marshall((ArrayOf<Byte>**)&data);
    parcel->Unmarshall(data, 0, data->GetLength());
    AutoPtr<ISignalStrength> signalStrength = CSignalStrength::MakeSignalStrengthFromRilParcel(parcel);
    return signalStrength;
}

AutoPtr<IArrayList> RIL::ResponseCdmaInformationRecord(
    /* [in] */ RILParcel* p)
{
    Int32 numberOfInfoRecs = 0;
    AutoPtr<IArrayList> response;

    /**
     * Loop through all of the information records unmarshalling them
     * and converting them to Java Objects.
     */
    p->ReadInt32(&numberOfInfoRecs);
    CArrayList::New(numberOfInfoRecs, (IArrayList**)&response);

    for (Int32 i = 0; i < numberOfInfoRecs; i++) {
        AutoPtr<ICdmaInformationRecords> InfoRec;
        assert(0 && "TODO");
        // CCdmaInformationRecords::New(p, (ICdmaInformationRecords**)&InfoRec);
        response->Add(InfoRec);
    }

    return response;
}

AutoPtr<IInterface> RIL::ResponseCdmaCallWaiting(
    /* [in] */ RILParcel* p)
{
    AutoPtr<ICdmaCallWaitingNotification> notification;
    assert(0 && "TODO");
    // CCdmaCallWaitingNotification::New((ICdmaCallWaitingNotification**)&notification);

    // p->ReadString(&(notification->mNumber));
    Int32 numberPresentation = 0;
    p->ReadInt32(&numberPresentation);
    // notification->mNumberPresentation;// =
            // CCdmaCallWaitingNotification::PresentationFromCLIP(numberPresentation);
    // p->ReadString(&(notification->mName));
    // notification->mNamePresentation = notification->mNumberPresentation;
    // p->ReadInt32(&(notification->mIsPresent));
    // p->ReadInt32(&(notification->mSignalType));
    // p->ReadInt32(&(notification->mAlertPitch));
    // p->ReadInt32(&(notification->mSignal));
    // p->ReadInt32(&(notification->mNumberType));
    // p->ReadInt32(&(notification->mNumberPlan));

    return notification;
}

AutoPtr<IInterface> RIL::ResponseCallRing(
    /* [in] */ RILParcel* p)
{
    AutoPtr<ArrayOf<Char32> > response = ArrayOf<Char32>::Alloc(4);

    Int32 isPresent = 0;
    p->ReadInt32(&isPresent);
    (*response)[0] = (Char32) isPresent;

    Int32 signalType = 0;
    p->ReadInt32(&signalType);
    (*response)[1] = (Char32) signalType;

    Int32 alertPitch = 0;
    p->ReadInt32(&alertPitch);
    (*response)[2] = (Char32) alertPitch;

    Int32 isignal = 0;
    p->ReadInt32(&isignal);
    (*response)[3] = (Char32) isignal;

    return CoreUtils::ConvertChar32Array(response);
}

AutoPtr<IArrayList> RIL::ResponseGetDataCallProfile(
    /* [in] */ RILParcel* p)
{
    Int32 nProfiles = 0;
    p->ReadInt32(&nProfiles);
    if (RILJ_LOGD) {
        // String str("# data call profiles:");
        // str += nProfiles;
        // RiljLog(str);
    }

    AutoPtr<IArrayList> response;
    CArrayList::New(nProfiles, (IArrayList**)&response);

    Int32 profileId = 0;
    Int32 priority = 0;
    for (Int32 i = 0; i < nProfiles; i++) {
        p->ReadInt32(&profileId);
        p->ReadInt32(&priority);
        AutoPtr<IApnProfileOmh> profile;
        assert(0 && "TODO");
        // CApnProfileOmh::New(profileId, priority, (IApnProfileOmh**)&profiles);
        if (RILJ_LOGD) {
            // String str("responseGetDataCallProfile()");
            // Int32 profileIdget = 0, priorityget = 0;
            // IApnSetting::Probe(profile)->GetProfileId(&profileIdget);
            // profile->GetPriority(&priorityget);
            // str += profileIdget;
            // str += ":";
            // str += priorityget;
            // RiljLog(str);
        }
        response->Add(profile);
    }

    return response;
}

void RIL::NotifyRegistrantsCdmaInfoRec(
    /* [in] */ ICdmaInformationRecords* infoRec)
{
    Int32 response = IRILConstants::RIL_UNSOL_CDMA_INFO_REC;
    assert(0 && "TODO" && response);
    // if (ICdmaInformationRecordsCdmaDisplayInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mDisplayInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mDisplayInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
    // else if (ICdmaInformationRecordsCdmaSignalInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mSignalInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mSignalInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
    // else if (ICdmaInformationRecordsCdmaNumberInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mNumberInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mNumberInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
    // else if (ICdmaInformationRecordsCdmaRedirectingNumberInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mRedirNumInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mRedirNumInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
    // else if (ICdmaInformationRecordsCdmaLineControlInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mLineControlInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mLineControlInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
    // else if (ICdmaInformationRecordsCdmaT53ClirInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mT53ClirInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mT53ClirInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
    // else if (ICdmaInformationRecordsCdmaT53AudioControlInfoRec::Probe(infoRec->mRecord) != NULL) {
    //     if (mT53AudCntrlInfoRegistrants != NULL) {
    //         if (RILJ_LOGD) UnsljLogRet(response, infoRec->mRecord);
    //         AutoPtr<AsyncResult> p = new AsyncResult(NULL, infoRec->mRecord, NULL);
    //         mT53AudCntrlInfoRegistrants->NotifyRegistrants(p);
    //     }
    // }
}

AutoPtr<IArrayList> RIL::ResponseCellInfoList(
    /* [in] */ RILParcel* p)
{
    Int32 numberOfInfoRecs = 0;
    AutoPtr<IArrayList> response;

    /**
     * Loop through all of the information records unmarshalling them
     * and converting them to Java Objects.
     */
    p->ReadInt32(&numberOfInfoRecs);
    CArrayList::New(numberOfInfoRecs, (IArrayList**)&response);

    for (Int32 i = 0; i < numberOfInfoRecs; i++) {
        assert(0 && "TODO");
        AutoPtr<ICellInfo> InfoRec;// = CellInfo.CREATOR.createFromParcel(p);
        response->Add(InfoRec);
    }

    return response;
}

AutoPtr<IInterface> RIL::ResponseHardwareConfig(
    /* [in] */ RILParcel* p)
{
    Int32 num = 0;
    AutoPtr<IArrayList> response;
    AutoPtr<IHardwareConfig> hw;

    p->ReadInt32(&num);
    CArrayList::New(num, (IArrayList**)&response);

    if (RILJ_LOGV) {
        // String str("responseHardwareConfig: num=");
        // str += num;
        // RiljLog(str);
    }
    for (Int32 i = 0 ; i < num ; i++) {
        Int32 type = 0;
        p->ReadInt32(&type);
        switch(type) {
            case IHardwareConfig::DEV_HARDWARE_TYPE_MODEM: {
                assert(0 && "TODO");
                // hw = new HardwareConfig(type);
                String id;
                p->ReadString(&id);
                Int32 state = 0;
                p->ReadInt32(&state);
                Int32 model = 0;
                p->ReadInt32(&model);
                Int32 ratBits = 0;
                p->ReadInt32(&ratBits);
                Int32 maxV = 0;
                p->ReadInt32(&maxV);
                Int32 maxD = 0;
                p->ReadInt32(&maxD);
                Int32 maxS = 0;
                p->ReadInt32(&maxS);
                hw->AssignModem(id, state, model, ratBits, maxV, maxD, maxS);
            break;
            }
            case IHardwareConfig::DEV_HARDWARE_TYPE_SIM: {
                assert(0 && "TODO");
                // hw = new HardwareConfig(type);
                String id;
                p->ReadString(&id);
                Int32 state = 0;
                p->ReadInt32(&state);
                String link;
                p->ReadString(&link);
                hw->AssignSim(id, state, link);
                break;
            }
            default: {
                // throw new RuntimeException(
                //    "RIL_REQUEST_GET_HARDWARE_CONFIG invalid hardward type:" + type);
                break;
            }
        }

        response->Add(hw);
    }

  return response;
}

String RIL::RequestToString(
    /* [in] */ Int32 request)
{
    /*
    cat libs/telephony/ril_commands.h \
    | egrep "^ *{RIL_" \
    | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'
    */
    switch(request) {
        case IRILConstants::RIL_REQUEST_GET_SIM_STATUS: return String("GET_SIM_STATUS");
        case IRILConstants::RIL_REQUEST_ENTER_SIM_PIN: return String("ENTER_SIM_PIN");
        case IRILConstants::RIL_REQUEST_ENTER_SIM_PUK: return String("ENTER_SIM_PUK");
        case IRILConstants::RIL_REQUEST_ENTER_SIM_PIN2: return String("ENTER_SIM_PIN2");
        case IRILConstants::RIL_REQUEST_ENTER_SIM_PUK2: return String("ENTER_SIM_PUK2");
        case IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN: return String("CHANGE_SIM_PIN");
        case IRILConstants::RIL_REQUEST_CHANGE_SIM_PIN2: return String("CHANGE_SIM_PIN2");
        case IRILConstants::RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE: return String("ENTER_DEPERSONALIZATION_CODE");
        case IRILConstants::RIL_REQUEST_GET_CURRENT_CALLS: return String("GET_CURRENT_CALLS");
        case IRILConstants::RIL_REQUEST_DIAL: return String("DIAL");
        case IRILConstants::RIL_REQUEST_GET_IMSI: return String("GET_IMSI");
        case IRILConstants::RIL_REQUEST_HANGUP: return String("HANGUP");
        case IRILConstants::RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: return String("HANGUP_WAITING_OR_BACKGROUND");
        case IRILConstants::RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: return String("HANGUP_FOREGROUND_RESUME_BACKGROUND");
        case IRILConstants::RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: return String("REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE");
        case IRILConstants::RIL_REQUEST_CONFERENCE: return String("CONFERENCE");
        case IRILConstants::RIL_REQUEST_UDUB: return String("UDUB");
        case IRILConstants::RIL_REQUEST_LAST_CALL_FAIL_CAUSE: return String("LAST_CALL_FAIL_CAUSE");
        case IRILConstants::RIL_REQUEST_SIGNAL_STRENGTH: return String("SIGNAL_STRENGTH");
        case IRILConstants::RIL_REQUEST_VOICE_REGISTRATION_STATE: return String("VOICE_REGISTRATION_STATE");
        case IRILConstants::RIL_REQUEST_DATA_REGISTRATION_STATE: return String("DATA_REGISTRATION_STATE");
        case IRILConstants::RIL_REQUEST_OPERATOR: return String("OPERATOR");
        case IRILConstants::RIL_REQUEST_RADIO_POWER: return String("RADIO_POWER");
        case IRILConstants::RIL_REQUEST_DTMF: return String("DTMF");
        case IRILConstants::RIL_REQUEST_SEND_SMS: return String("SEND_SMS");
        case IRILConstants::RIL_REQUEST_SEND_SMS_EXPECT_MORE: return String("SEND_SMS_EXPECT_MORE");
        case IRILConstants::RIL_REQUEST_SETUP_DATA_CALL: return String("SETUP_DATA_CALL");
        case IRILConstants::RIL_REQUEST_SIM_IO: return String("SIM_IO");
        case IRILConstants::RIL_REQUEST_SEND_USSD: return String("SEND_USSD");
        case IRILConstants::RIL_REQUEST_CANCEL_USSD: return String("CANCEL_USSD");
        case IRILConstants::RIL_REQUEST_GET_CLIR: return String("GET_CLIR");
        case IRILConstants::RIL_REQUEST_SET_CLIR: return String("SET_CLIR");
        case IRILConstants::RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: return String("QUERY_CALL_FORWARD_STATUS");
        case IRILConstants::RIL_REQUEST_SET_CALL_FORWARD: return String("SET_CALL_FORWARD");
        case IRILConstants::RIL_REQUEST_QUERY_CALL_WAITING: return String("QUERY_CALL_WAITING");
        case IRILConstants::RIL_REQUEST_SET_CALL_WAITING: return String("SET_CALL_WAITING");
        case IRILConstants::RIL_REQUEST_SMS_ACKNOWLEDGE: return String("SMS_ACKNOWLEDGE");
        case IRILConstants::RIL_REQUEST_GET_IMEI: return String("GET_IMEI");
        case IRILConstants::RIL_REQUEST_GET_IMEISV: return String("GET_IMEISV");
        case IRILConstants::RIL_REQUEST_ANSWER: return String("ANSWER");
        case IRILConstants::RIL_REQUEST_DEACTIVATE_DATA_CALL: return String("DEACTIVATE_DATA_CALL");
        case IRILConstants::RIL_REQUEST_QUERY_FACILITY_LOCK: return String("QUERY_FACILITY_LOCK");
        case IRILConstants::RIL_REQUEST_SET_FACILITY_LOCK: return String("SET_FACILITY_LOCK");
        case IRILConstants::RIL_REQUEST_CHANGE_BARRING_PASSWORD: return String("CHANGE_BARRING_PASSWORD");
        case IRILConstants::RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: return String("QUERY_NETWORK_SELECTION_MODE");
        case IRILConstants::RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: return String("SET_NETWORK_SELECTION_AUTOMATIC");
        case IRILConstants::RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: return String("SET_NETWORK_SELECTION_MANUAL");
        case IRILConstants::RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : return String("QUERY_AVAILABLE_NETWORKS ");
        case IRILConstants::RIL_REQUEST_DTMF_START: return String("DTMF_START");
        case IRILConstants::RIL_REQUEST_DTMF_STOP: return String("DTMF_STOP");
        case IRILConstants::RIL_REQUEST_BASEBAND_VERSION: return String("BASEBAND_VERSION");
        case IRILConstants::RIL_REQUEST_SEPARATE_CONNECTION: return String("SEPARATE_CONNECTION");
        case IRILConstants::RIL_REQUEST_SET_MUTE: return String("SET_MUTE");
        case IRILConstants::RIL_REQUEST_GET_MUTE: return String("GET_MUTE");
        case IRILConstants::RIL_REQUEST_QUERY_CLIP: return String("QUERY_CLIP");
        case IRILConstants::RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: return String("LAST_DATA_CALL_FAIL_CAUSE");
        case IRILConstants::RIL_REQUEST_DATA_CALL_LIST: return String("DATA_CALL_LIST");
        case IRILConstants::RIL_REQUEST_RESET_RADIO: return String("RESET_RADIO");
        case IRILConstants::RIL_REQUEST_OEM_HOOK_RAW: return String("OEM_HOOK_RAW");
        case IRILConstants::RIL_REQUEST_OEM_HOOK_STRINGS: return String("OEM_HOOK_STRINGS");
        case IRILConstants::RIL_REQUEST_SCREEN_STATE: return String("SCREEN_STATE");
        case IRILConstants::RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: return String("SET_SUPP_SVC_NOTIFICATION");
        case IRILConstants::RIL_REQUEST_WRITE_SMS_TO_SIM: return String("WRITE_SMS_TO_SIM");
        case IRILConstants::RIL_REQUEST_DELETE_SMS_ON_SIM: return String("DELETE_SMS_ON_SIM");
        case IRILConstants::RIL_REQUEST_SET_BAND_MODE: return String("SET_BAND_MODE");
        case IRILConstants::RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: return String("QUERY_AVAILABLE_BAND_MODE");
        case IRILConstants::RIL_REQUEST_STK_GET_PROFILE: return String("REQUEST_STK_GET_PROFILE");
        case IRILConstants::RIL_REQUEST_STK_SET_PROFILE: return String("REQUEST_STK_SET_PROFILE");
        case IRILConstants::RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: return String("REQUEST_STK_SEND_ENVELOPE_COMMAND");
        case IRILConstants::RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: return String("REQUEST_STK_SEND_TERMINAL_RESPONSE");
        case IRILConstants::RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: return String("REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM");
        case IRILConstants::RIL_REQUEST_EXPLICIT_CALL_TRANSFER: return String("REQUEST_EXPLICIT_CALL_TRANSFER");
        case IRILConstants::RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: return String("REQUEST_SET_PREFERRED_NETWORK_TYPE");
        case IRILConstants::RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: return String("REQUEST_GET_PREFERRED_NETWORK_TYPE");
        case IRILConstants::RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: return String("REQUEST_GET_NEIGHBORING_CELL_IDS");
        case IRILConstants::RIL_REQUEST_SET_LOCATION_UPDATES: return String("REQUEST_SET_LOCATION_UPDATES");
        case IRILConstants::RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: return String("RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE");
        case IRILConstants::RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: return String("RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE");
        case IRILConstants::RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: return String("RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE");
        case IRILConstants::RIL_REQUEST_SET_TTY_MODE: return String("RIL_REQUEST_SET_TTY_MODE");
        case IRILConstants::RIL_REQUEST_QUERY_TTY_MODE: return String("RIL_REQUEST_QUERY_TTY_MODE");
        case IRILConstants::RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: return String("RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE");
        case IRILConstants::RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: return String("RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE");
        case IRILConstants::RIL_REQUEST_CDMA_FLASH: return String("RIL_REQUEST_CDMA_FLASH");
        case IRILConstants::RIL_REQUEST_CDMA_BURST_DTMF: return String("RIL_REQUEST_CDMA_BURST_DTMF");
        case IRILConstants::RIL_REQUEST_CDMA_SEND_SMS: return String("RIL_REQUEST_CDMA_SEND_SMS");
        case IRILConstants::RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: return String("RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE");
        case IRILConstants::RIL_REQUEST_GSM_GET_BROADCAST_CONFIG: return String("RIL_REQUEST_GSM_GET_BROADCAST_CONFIG");
        case IRILConstants::RIL_REQUEST_GSM_SET_BROADCAST_CONFIG: return String("RIL_REQUEST_GSM_SET_BROADCAST_CONFIG");
        case IRILConstants::RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG: return String("RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG");
        case IRILConstants::RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG: return String("RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG");
        case IRILConstants::RIL_REQUEST_GSM_BROADCAST_ACTIVATION: return String("RIL_REQUEST_GSM_BROADCAST_ACTIVATION");
        case IRILConstants::RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: return String("RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY");
        case IRILConstants::RIL_REQUEST_CDMA_BROADCAST_ACTIVATION: return String("RIL_REQUEST_CDMA_BROADCAST_ACTIVATION");
        case IRILConstants::RIL_REQUEST_CDMA_SUBSCRIPTION: return String("RIL_REQUEST_CDMA_SUBSCRIPTION");
        case IRILConstants::RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: return String("RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM");
        case IRILConstants::RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: return String("RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM");
        case IRILConstants::RIL_REQUEST_DEVICE_IDENTITY: return String("RIL_REQUEST_DEVICE_IDENTITY");
        case IRILConstants::RIL_REQUEST_GET_SMSC_ADDRESS: return String("RIL_REQUEST_GET_SMSC_ADDRESS");
        case IRILConstants::RIL_REQUEST_SET_SMSC_ADDRESS: return String("RIL_REQUEST_SET_SMSC_ADDRESS");
        case IRILConstants::RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: return String("REQUEST_EXIT_EMERGENCY_CALLBACK_MODE");
        case IRILConstants::RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: return String("RIL_REQUEST_REPORT_SMS_MEMORY_STATUS");
        case IRILConstants::RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: return String("RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING");
        case IRILConstants::RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: return String("RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE");
        case IRILConstants::RIL_REQUEST_GET_DATA_CALL_PROFILE: return String("RIL_REQUEST_GET_DATA_CALL_PROFILE");
        case IRILConstants::RIL_REQUEST_ISIM_AUTHENTICATION: return String("RIL_REQUEST_ISIM_AUTHENTICATION");
        case IRILConstants::RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: return String("RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU");
        case IRILConstants::RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: return String("RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS");
        case IRILConstants::RIL_REQUEST_VOICE_RADIO_TECH: return String("RIL_REQUEST_VOICE_RADIO_TECH");
        case IRILConstants::RIL_REQUEST_GET_CELL_INFO_LIST: return String("RIL_REQUEST_GET_CELL_INFO_LIST");
        case IRILConstants::RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: return String("RIL_REQUEST_SET_CELL_INFO_LIST_RATE");
        case IRILConstants::RIL_REQUEST_SET_INITIAL_ATTACH_APN: return String("RIL_REQUEST_SET_INITIAL_ATTACH_APN");
        case IRILConstants::RIL_REQUEST_SET_DATA_PROFILE: return String("RIL_REQUEST_SET_DATA_PROFILE");
        case IRILConstants::RIL_REQUEST_IMS_REGISTRATION_STATE: return String("RIL_REQUEST_IMS_REGISTRATION_STATE");
        case IRILConstants::RIL_REQUEST_IMS_SEND_SMS: return String("RIL_REQUEST_IMS_SEND_SMS");
        case IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: return String("RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC");
        case IRILConstants::RIL_REQUEST_SIM_OPEN_CHANNEL: return String("RIL_REQUEST_SIM_OPEN_CHANNEL");
        case IRILConstants::RIL_REQUEST_SIM_CLOSE_CHANNEL: return String("RIL_REQUEST_SIM_CLOSE_CHANNEL");
        case IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: return String("RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL");
        case IRILConstants::RIL_REQUEST_NV_READ_ITEM: return String("RIL_REQUEST_NV_READ_ITEM");
        case IRILConstants::RIL_REQUEST_NV_WRITE_ITEM: return String("RIL_REQUEST_NV_WRITE_ITEM");
        case IRILConstants::RIL_REQUEST_NV_WRITE_CDMA_PRL: return String("RIL_REQUEST_NV_WRITE_CDMA_PRL");
        case IRILConstants::RIL_REQUEST_NV_RESET_CONFIG: return String("RIL_REQUEST_NV_RESET_CONFIG");
        case IRILConstants::RIL_REQUEST_SET_UICC_SUBSCRIPTION: return String("RIL_REQUEST_SET_UICC_SUBSCRIPTION");
        case IRILConstants::RIL_REQUEST_ALLOW_DATA: return String("RIL_REQUEST_ALLOW_DATA");
        case IRILConstants::RIL_REQUEST_GET_HARDWARE_CONFIG: return String("GET_HARDWARE_CONFIG");
        case IRILConstants::RIL_REQUEST_SIM_AUTHENTICATION: return String("RIL_REQUEST_SIM_AUTHENTICATION");
        case IRILConstants::RIL_REQUEST_SHUTDOWN: return String("RIL_REQUEST_SHUTDOWN");
        default: return String("<unknown request>");
    }
}

String RIL::ResponseToString(
    /* [in] */ Int32 request)
{
    /*
    cat libs/telephony/ril_unsol_commands.h \
    | egrep "^ *{RIL_" \
    | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'
    */
    switch(request) {
        case IRILConstants::RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: return String("UNSOL_RESPONSE_RADIO_STATE_CHANGED");
        case IRILConstants::RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: return String("UNSOL_RESPONSE_CALL_STATE_CHANGED");
        case IRILConstants::RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: return String("UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED");
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS: return String("UNSOL_RESPONSE_NEW_SMS");
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: return String("UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT");
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: return String("UNSOL_RESPONSE_NEW_SMS_ON_SIM");
        case IRILConstants::RIL_UNSOL_ON_USSD: return String("UNSOL_ON_USSD");
        case IRILConstants::RIL_UNSOL_ON_USSD_REQUEST: return String("UNSOL_ON_USSD_REQUEST");
        case IRILConstants::RIL_UNSOL_NITZ_TIME_RECEIVED: return String("UNSOL_NITZ_TIME_RECEIVED");
        case IRILConstants::RIL_UNSOL_SIGNAL_STRENGTH: return String("UNSOL_SIGNAL_STRENGTH");
        case IRILConstants::RIL_UNSOL_DATA_CALL_LIST_CHANGED: return String("UNSOL_DATA_CALL_LIST_CHANGED");
        case IRILConstants::RIL_UNSOL_SUPP_SVC_NOTIFICATION: return String("UNSOL_SUPP_SVC_NOTIFICATION");
        case IRILConstants::RIL_UNSOL_STK_SESSION_END: return String("UNSOL_STK_SESSION_END");
        case IRILConstants::RIL_UNSOL_STK_PROACTIVE_COMMAND: return String("UNSOL_STK_PROACTIVE_COMMAND");
        case IRILConstants::RIL_UNSOL_STK_EVENT_NOTIFY: return String("UNSOL_STK_EVENT_NOTIFY");
        case IRILConstants::RIL_UNSOL_STK_CALL_SETUP: return String("UNSOL_STK_CALL_SETUP");
        case IRILConstants::RIL_UNSOL_SIM_SMS_STORAGE_FULL: return String("UNSOL_SIM_SMS_STORAGE_FULL");
        case IRILConstants::RIL_UNSOL_SIM_REFRESH: return String("UNSOL_SIM_REFRESH");
        case IRILConstants::RIL_UNSOL_CALL_RING: return String("UNSOL_CALL_RING");
        case IRILConstants::RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED: return String("UNSOL_RESPONSE_SIM_STATUS_CHANGED");
        case IRILConstants::RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: return String("UNSOL_RESPONSE_CDMA_NEW_SMS");
        case IRILConstants::RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: return String("UNSOL_RESPONSE_NEW_BROADCAST_SMS");
        case IRILConstants::RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: return String("UNSOL_CDMA_RUIM_SMS_STORAGE_FULL");
        case IRILConstants::RIL_UNSOL_RESTRICTED_STATE_CHANGED: return String("UNSOL_RESTRICTED_STATE_CHANGED");
        case IRILConstants::RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: return String("UNSOL_ENTER_EMERGENCY_CALLBACK_MODE");
        case IRILConstants::RIL_UNSOL_CDMA_CALL_WAITING: return String("UNSOL_CDMA_CALL_WAITING");
        case IRILConstants::RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: return String("UNSOL_CDMA_OTA_PROVISION_STATUS");
        case IRILConstants::RIL_UNSOL_CDMA_INFO_REC: return String("UNSOL_CDMA_INFO_REC");
        case IRILConstants::RIL_UNSOL_OEM_HOOK_RAW: return String("UNSOL_OEM_HOOK_RAW");
        case IRILConstants::RIL_UNSOL_RINGBACK_TONE: return String("UNSOL_RINGBACK_TONE");
        case IRILConstants::RIL_UNSOL_RESEND_INCALL_MUTE: return String("UNSOL_RESEND_INCALL_MUTE");
        case IRILConstants::RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: return String("CDMA_SUBSCRIPTION_SOURCE_CHANGED");
        case IRILConstants::RIL_UNSOl_CDMA_PRL_CHANGED: return String("UNSOL_CDMA_PRL_CHANGED");
        case IRILConstants::RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: return String("UNSOL_EXIT_EMERGENCY_CALLBACK_MODE");
        case IRILConstants::RIL_UNSOL_RIL_CONNECTED: return String("UNSOL_RIL_CONNECTED");
        case IRILConstants::RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: return String("UNSOL_VOICE_RADIO_TECH_CHANGED");
        case IRILConstants::RIL_UNSOL_CELL_INFO_LIST: return String("UNSOL_CELL_INFO_LIST");
        case IRILConstants::RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED:
            return String("UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED");
        case IRILConstants::RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED:
                return String("RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED");
        case IRILConstants::RIL_UNSOL_SRVCC_STATE_NOTIFY:
                return String("UNSOL_SRVCC_STATE_NOTIFY");
        case IRILConstants::RIL_UNSOL_HARDWARE_CONFIG_CHANGED: return String("RIL_UNSOL_HARDWARE_CONFIG_CHANGED");
        case IRILConstants::RIL_UNSOL_ON_SS: return String("UNSOL_ON_SS");
        case IRILConstants::RIL_UNSOL_STK_CC_ALPHA_NOTIFY: return String("UNSOL_STK_CC_ALPHA_NOTIFY");
        case IRILConstants::RIL_UNSOL_STK_SEND_SMS_RESULT: return String("RIL_UNSOL_STK_SEND_SMS_RESULT");
        default: return String("<unknown response>");
    }
}

void RIL::RiljLog(
    /* [in] */ const String& msg)
{
    // Rlog.d(RILJ_LOG_TAG, msg
    //         + (mInstanceId != NULL ? (" [SUB" + mInstanceId + "]") : ""));
    Int32 value = 0;
    if (mInstanceId != NULL)
        mInstanceId->GetValue(&value);
    Logger::D(RILJ_LOG_TAG, "msg:%s, [SUB %d ]", msg.string(), value);
}

void RIL::RiljLogv(
    /* [in] */ const String& msg)
{
    // Rlog.v(RILJ_LOG_TAG, msg
    //         + (mInstanceId != NULL ? (" [SUB" + mInstanceId + "]") : ""));
    Int32 value = 0;
    if (mInstanceId != NULL)
        mInstanceId->GetValue(&value);
    Logger::V(RILJ_LOG_TAG, "msg:%s, [SUB %d ]", msg.string(), value);
}

void RIL::UnsljLog(
    /* [in] */ Int32 response)
{
    String str("[UNSL]< ");
    str += ResponseToString(response);
    RiljLog(str);
}

void RIL::UnsljLogMore(
    /* [in] */ Int32 response,
    /* [in] */ const String& more)
{
    String str("[UNSL]< ");
    str += ResponseToString(response);
    str += " ";
    str += more;
    RiljLog(str);
}

void RIL::UnsljLogRet(
    /* [in] */ Int32 response,
    /* [in] */ IInterface* ret)
{
    String str("[UNSL]< ");
    str += ResponseToString(response);
    str += " ";
    str += RetToString(response, ret);
    RiljLog(str);
}

void RIL::UnsljLogvRet(
    /* [in] */ Int32 response,
    /* [in] */ IInterface* ret)
{
    String str("[UNSL]< ");
    str += ResponseToString(response);
    str += " ";
    str += RetToString(response, ret);
    RiljLogv(str);
}

AutoPtr<IInterface> RIL::ResponseSsData(
    /* [in] */ RILParcel* p)
{
    Int32 num = 0;
    AutoPtr<ISsData> ssData;
    assert(0 && "TODO");
    // CSsData::New((ISsData**)&ssData);

    Int32 serviceType = 0;
    p->ReadInt32(&serviceType);
    // ssData->mServiceType = ssData->ServiceTypeFromRILInt(serviceType);
    Int32 requestType = 0;
    p->ReadInt32(&requestType);
    // ssData->mRequestType = ssData->RequestTypeFromRILInt(requestType);
    Int32 teleserviceType = 0;
    p->ReadInt32(&teleserviceType);
    // ssData->mTeleserviceType = ssData->TeleserviceTypeFromRILInt(teleserviceType);
    // p->ReadInt32(&(ssData->mServiceClass)); // This is service class sent in the SS request.
    // p->ReadInt32(&(ssData->mResult)); // This is the result of the SS request.
    p->ReadInt32(&num);

    // if (ssData->mServiceType->IsTypeCF() &&
    //     ssData->mRequestType->IsTypeInterrogation()) {
    //     ssData->mCfInfo = new CallForwardInfo[num];

    //     for (Int32 i = 0; i < num; i++) {
    //         AutoPtr<ICallForwardInfo> pInfo;
    //         CCallForwardInfo::New((ICallForwardInfo**)&pInfo);
    //         ssData->mCfInfo[i] = pInfo;

    //         Int32 status = 0;
    //         p->ReadInt32(&status);
    //         pInfo->mStatus = status;
    //         Int32 reason = 0;
    //         p->ReadInt32(&reason);
    //         pInfo->mReason = reason;
    //         Int32 serviceClass = 0;
    //         p->ReadInt32(&serviceClass);
    //         pInfo->mServiceClass = serviceClass;
    //         Int32 toa = 0;
    //         p->ReadInt32(&toa);
    //         pInfo->mToa = toa;
    //         String number;
    //         p->ReadString(&number);
    //         pInfo->mNumber = number;
    //         Int32 timeSeconds = 0;
    //         p->ReadInt32(&timeSeconds);
    //         pInfo->mTimeSeconds = timeSeconds;

    //         String str("[SS Data] CF Info ");
    //         str += i;
    //         str += " : ";
    //         str += pInfo;
    //         RiljLog(str);
    //     }
    // }
    // else {
    //     ssData->mSsInfo = ArrayOf<Int32>::Alloc(num);
    //     for (Int32 i = 0; i < num; i++) {
    //         Int32 iInfoNode = 0;
    //         p->ReadInt32(&iInfoNode);
    //         ssData->mSsInfo[i] = iInfoNode;
    //         String str("[SS Data] SS Info ");
    //         str += i;
    //         str += " : ";
    //         str += iInfoNode;
    //         RiljLog(str);
    //     }
    // }

    return ssData;
}

ECode RIL::GetDeviceIdentity(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_DEVICE_IDENTITY, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetCDMASubscription(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_SUBSCRIPTION, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetPhoneType(
    /* [in] */ Int32 phoneType)
{
    // Called by CDMAPhone and GSMPhone constructor
    if (RILJ_LOGD) {
        // String str("setPhoneType=");
        // str += phoneType;
        // str += " old value=";
        // str += mPhoneType;
        // RiljLog(str);
    }
    mPhoneType = phoneType;
    return NOERROR;
}

ECode RIL::QueryCdmaRoamingPreference(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCdmaRoamingPreference(
    /* [in] */ Int32 cdmaRoamingType,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(cdmaRoamingType);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " : ";
        // str += cdmaRoamingType;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCdmaSubscriptionSource(
    /* [in] */ Int32 cdmaSubscription,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(cdmaSubscription);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " : ";
        // str += cdmaSubscription;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetCdmaSubscriptionSource(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::QueryTTYMode(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_QUERY_TTY_MODE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetTTYMode(
    /* [in] */ Int32 ttyMode,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(
            IRILConstants::RIL_REQUEST_SET_TTY_MODE, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(ttyMode);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " : ";
        // str += ttyMode;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SendCDMAFeatureCode(
    /* [in] */ const String& FeatureCode,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_FLASH, response);

    rr->mParcel->WriteString(FeatureCode);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += " : ";
        str += FeatureCode;
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetCdmaBroadcastConfig(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG, response);

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCdmaBroadcastConfig(
    /* [in] */ ArrayOf<ICdmaSmsBroadcastConfigInfo*>* configs,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG, response);

    // Convert to 1 service category per config (the way RIL takes is)
    AutoPtr<IArrayList> processedConfigs;
    CArrayList::New((IArrayList**)&processedConfigs);
    for (Int32 k = 0; k < configs->GetLength(); k++) {
        AutoPtr<ICdmaSmsBroadcastConfigInfo> config = (*configs)[k];
        Int32 i = 0;
        config->GetFromServiceCategory(&i);
        Int32 iMax = 0;
        config->GetToServiceCategory(&iMax);
        for (; i <= iMax; i++) {
            Int32 language = 0;
            config->GetLanguage(&language);
            Boolean bSelected = FALSE;
            config->IsSelected(&bSelected);
            AutoPtr<ICdmaSmsBroadcastConfigInfo> pCI;
            assert(0 && "TODO");
            // CCdmaSmsBroadcastConfigInfo::New(i,
            //         i,
            //         language,
            //         bSelected,
            //         (ICdmaSmsBroadcastConfigInfo**)&pCI);
            processedConfigs->Add(pCI);
        }
    }

    AutoPtr<ArrayOf<ICdmaSmsBroadcastConfigInfo*> > rilConfigs;
    assert(0 && "TODO");
    // processedConfigs->ToArray(configs, (ArrayOf<IInterface*>**)&rilConfigs);
    rr->mParcel->WriteInt32(rilConfigs->GetLength());
    for(Int32 i = 0; i < rilConfigs->GetLength(); i++) {
        Int32 cate = 0;
        (*rilConfigs)[i]->GetFromServiceCategory(&cate);
        rr->mParcel->WriteInt32(cate);
        Int32 language = 0;
        (*rilConfigs)[i]->GetLanguage(&language);
        rr->mParcel->WriteInt32(language);
        Boolean bSelected = FALSE;
        (*rilConfigs)[i]->IsSelected(&bSelected);
        rr->mParcel->WriteInt32(bSelected ? 1 : 0);
    }

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += " with ";
        // str += rilConfigs->GetLength();
        // str += " configs : ";
        // RiljLog(str);
        for (Int32 i = 0; i < rilConfigs->GetLength(); i++) {
            assert(0 && "TODO");
            // RiljLog((*rilConfigs)[i]->ToString());
        }
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCdmaBroadcastActivation(
    /* [in] */ Boolean activate,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_CDMA_BROADCAST_ACTIVATION, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(activate ? 0 : 1);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::ExitEmergencyCallbackMode(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, response);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::RequestIsimAuthentication(
    /* [in] */ const String& nonce,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_ISIM_AUTHENTICATION, response);

    rr->mParcel->WriteString(nonce);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::RequestIccSimAuthentication(
    /* [in] */ Int32 authContext,
    /* [in] */ const String& data,
    /* [in] */ const String& aid,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SIM_AUTHENTICATION, response);

    rr->mParcel->WriteInt32(authContext);
    rr->mParcel->WriteString(data);
    rr->mParcel->WriteString(aid);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::GetCellInfoList(
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_GET_CELL_INFO_LIST, result);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetCellInfoListRate(
    /* [in] */ Int32 rateInMillis,
    /* [in] */ IMessage* response)
{
    if (RILJ_LOGD) {
        // String str("setCellInfoListRate: ");
        // str += rateInMillis;
        // RiljLog(str);
    }
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(rateInMillis);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetInitialAttachApn(
    /* [in] */ const String& apn,
    /* [in] */ const String& protocol,
    /* [in] */ Int32 authType,
    /* [in] */ const String& username,
    /* [in] */ const String& password,
    /* [in] */ IMessage* result)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_INITIAL_ATTACH_APN, result);

    if (RILJ_LOGD) RiljLog(String("Set RIL_REQUEST_SET_INITIAL_ATTACH_APN"));

    rr->mParcel->WriteString(apn);
    rr->mParcel->WriteString(protocol);
    rr->mParcel->WriteInt32(authType);
    rr->mParcel->WriteString(username);
    rr->mParcel->WriteString(password);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ", apn:"; str += apn;
        // str += ", protocol:"; str += protocol;
        // str += ", authType:"; str += authType;
        // str += ", username:"; str += username;
        // str += ", password:"; str += password;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::SetDataProfile(
    /* [in] */ ArrayOf<IDataProfile*>* dps,
    /* [in] */ IMessage* result)
{
    if (RILJ_LOGD) RiljLog(String("Set RIL_REQUEST_SET_DATA_PROFILE"));

    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SET_DATA_PROFILE, NULL);
    assert(0 && "TODO");
    // DataProfile::ToParcel(rr->mParcel, dps);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        str += " with ";
        // str += dps;
        str += " Data Profiles : ";
        RiljLog(str);
        for (Int32 i = 0; i < dps->GetLength(); i++) {
            assert(0 && "TODO");
            // RiljLog((*dps)[i].ToString());
        }
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::TestingEmergencyCall()
{
    if (RILJ_LOGD) RiljLog(String("testingEmergencyCall"));
    mTestingEmergencyCall->Set(TRUE);
    return NOERROR;
}

ECode RIL::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("RIL: "));
    assert(0 && "TODO");
    // pw->Println(this);
    pw->Println(String(" mSocket="));
    pw->Println(mSocket);
    pw->Println(String(" mSenderThread="));
    pw->Println(mSenderThread);
    pw->Println(String(" mSender="));
    // pw->Println(mSender);
    pw->Println(String(" mReceiverThread="));
    pw->Println(mReceiverThread);
    pw->Println(String(" mReceiver="));
    // pw->Println(mReceiver);
    pw->Println(String(" mWakeLock="));
    pw->Println(mWakeLock);
    pw->Println(String(" mWakeLockTimeout="));
    pw->Println(mWakeLockTimeout);
    {
        AutoLock lock(mRequestList);
        {
            AutoLock lock(mWakeLock);
            pw->Println(String(" mWakeLockCount="));
            pw->Println(mWakeLockCount);
        }
        Int32 count = 0;
        mRequestList->GetSize(&count);
        pw->Println(String(" mRequestList count="));
        pw->Println(count);
        for (Int32 i = 0; i < count; i++) {
            assert(0 && "TODO");
            // AutoPtr<RILRequest> rr = mRequestList.valueAt(i);
            // pw->Println("  [" + rr.mSerial + "] " + RequestToString(rr->mRequest));
        }
    }
    pw->Println(String(" mLastNITZTimeInfo="));
    pw->Println(mLastNITZTimeInfo);
    pw->Println(String(" mTestingEmergencyCall="));
    pw->Println(mTestingEmergencyCall.Get());
    return NOERROR;
}

ECode RIL::IccOpenLogicalChannel(
    /* [in] */ const String& AID,
    /* [in] */ IMessage* response)
{
    if(mRilVersion < 10) {
        if (response != NULL) {
            //AutoPtr<ICommandException> ex;
            Logger::E("RILJ", "TODO RIL::IccOpenLogicalChannel");
            AutoPtr<IThrowable> ex;
            CThrowable::New(String("RIL::OnError"), (IThrowable**)&ex);
            // CCommandException::New(
            //     ICommandException::Error::REQUEST_NOT_SUPPORTED, (ICommandException**)&ex);
            AsyncResult::ForMessage(response, NULL, IThrowable::Probe(ex));
            response->SendToTarget();
        }
        return NOERROR;
    }

    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SIM_OPEN_CHANNEL, response);
    rr->mParcel->WriteString(AID);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::IccCloseLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SIM_CLOSE_CHANNEL, response);
    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(channel);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::IccTransmitApduLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    if(mRilVersion < 10) {
        if (response != NULL) {
            AutoPtr<ICommandException> ex;
            assert(0 && "TODO");
            // CCommandException::New(
            //     ICommandException::Error::REQUEST_NOT_SUPPORTED, (ICommandException**)&ex);
            // AsyncResult::ForMessage(response, NULL, ex);
            response->SendToTarget();
        }
        return NOERROR;
    }

    if (channel <= 0) {
        // throw new RuntimeException(
        //     "Invalid channel in iccTransmitApduLogicalChannel: " + channel);
        return E_RUNTIME_EXCEPTION;
    }

    IccTransmitApduHelper(IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, channel, cla,
            instruction, p1, p2, p3, data, response);
    return NOERROR;
}

ECode RIL::IccTransmitApduBasicChannel(
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    IccTransmitApduHelper(IRILConstants::RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, 0, cla, instruction,
            p1, p2, p3, data, response);
    return NOERROR;
}

ECode RIL::GetAtr(
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_SIM_GET_ATR, response);
    Int32 slotId = 0;
    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(slotId);
    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> iccGetAtr: ";
        // str += RequestToString(rr->mRequest);
        // str += " ";
        // str += slotId;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

void RIL::IccTransmitApduHelper(
    /* [in] */ Int32 rilCommand,
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    if (mRilVersion < 10) {
        if (response != NULL) {
            AutoPtr<ICommandException> ex;
            assert(0 && "TODO");
            // CCommandException::New(
            //     ICommandException::Error::REQUEST_NOT_SUPPORTED, (ICommandException**)&ex);
            // AsyncResult::ForMessage(response, NULL, ex);
            response->SendToTarget();
        }
        return;
    }

    AutoPtr<RILRequest> rr = RILRequest::Obtain(rilCommand, response);
    rr->mParcel->WriteInt32(channel);
    rr->mParcel->WriteInt32(cla);
    rr->mParcel->WriteInt32(instruction);
    rr->mParcel->WriteInt32(p1);
    rr->mParcel->WriteInt32(p2);
    rr->mParcel->WriteInt32(p3);
    rr->mParcel->WriteString(data);

    if (RILJ_LOGD) {
        String str = rr->SerialString();
        str += "> ";
        str += RequestToString(rr->mRequest);
        RiljLog(str);
    }

    Send(rr);
    return;
}

ECode RIL::NvReadItem(
    /* [in] */ Int32 itemID,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_NV_READ_ITEM, response);

    rr->mParcel->WriteInt32(itemID);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ' ';
        // str += itemID;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::NvWriteItem(
    /* [in] */ Int32 itemID,
    /* [in] */ const String& itemValue,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_NV_WRITE_ITEM, response);

    rr->mParcel->WriteInt32(itemID);
    rr->mParcel->WriteString(itemValue);

    if (RILJ_LOGD) {
        // String str = rr->SerialString();
        // str += "> ";
        // str += RequestToString(rr->mRequest);
        // str += ' ';
        // str += itemID;
        // str += ": ";
        // str += itemValue;
        // RiljLog(str);
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::NvWriteCdmaPrl(
    /* [in] */ ArrayOf<Byte>* preferredRoamingList,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_NV_WRITE_CDMA_PRL, response);

    assert(0 && "TODO");
    // rr->mParcel->WriteByteArray(preferredRoamingList);

    if (RILJ_LOGD) {
        StringBuilder sb(rr->SerialString());
        sb += "> "; sb += RequestToString(rr->mRequest);
        sb += " ("; sb += preferredRoamingList->GetLength(); sb += " bytes)";
        RiljLog(sb.ToString());
    }

    Send(rr);
    return NOERROR;
}

ECode RIL::NvResetConfig(
    /* [in] */ Int32 resetType,
    /* [in] */ IMessage* response)
{
    AutoPtr<RILRequest> rr = RILRequest::Obtain(IRILConstants::RIL_REQUEST_NV_RESET_CONFIG, response);

    rr->mParcel->WriteInt32(1);
    rr->mParcel->WriteInt32(resetType);

    if (RILJ_LOGD) {
        StringBuilder sb(rr->SerialString());
        sb += "> "; sb += RequestToString(rr->mRequest); sb += " "; sb += resetType;
        RiljLog(sb.ToString());
    }

    Send(rr);
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
