/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using static com::Android::Internal::Telephony::RILConstants::I*;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_UNKNOWN;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_EDGE;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_GPRS;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_UMTS;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_HSDPA;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_HSUPA;
using static android::Telephony::TelephonyManager::INETWORK_TYPE_HSPA;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::PowerManager::IWakeLock;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::INeighboringCellInfo;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IDisplay;

using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Gsm::ISsData;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardApplicationStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccCardStatus;
using Elastos::Droid::Internal::Telephony::Uicc::IIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRefreshResponse;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaCallWaitingNotification;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInformationRecords;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Dataconnection::IApnProfileOmh;
using Elastos::Droid::Internal::Telephony::Dataconnection::IApnSetting;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcFailCause;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataCallResponse;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataProfile;
using Elastos::Droid::Internal::Telephony::ITelephonyDevController;
using Elastos::Droid::Internal::Telephony::IHardwareConfig;

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Nio::IByteBuffer;
using Elastos::Nio::IByteOrder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;
using Elastos::Utility::IRandom;

/**
 * {@hide}
 */
class RILRequest {
    static const String LOG_TAG = "RilRequest";

    //***** Class Variables
    static Random sRandom = new Random();
    static AtomicInteger sNextSerial = new AtomicInteger(0);
    private static Object sPoolSync = new Object();
    private static RILRequest sPool = NULL;
    private static Int32 sPoolSize = 0;
    private static const Int32 MAX_POOL_SIZE = 4;
    private Context mContext;

    //***** Instance Variables
    Int32 mSerial;
    Int32 mRequest;
    Message mResult;
    Parcel mParcel;
    RILRequest mNext;

    /**
     * Retrieves a new RILRequest instance from the pool.
     *
     * @param request RIL_REQUEST_*
     * @param result sent when operation completes
     * @return a RILRequest instance from the pool.
     */
    static RILRequest Obtain(Int32 request, Message result) {
        RILRequest rr = NULL;

        Synchronized(sPoolSync) {
            If (sPool != NULL) {
                rr = sPool;
                sPool = rr.mNext;
                rr.mNext = NULL;
                sPoolSize--;
            }
        }

        If (rr == NULL) {
            rr = new RILRequest();
        }

        rr.mSerial = sNextSerial->GetAndIncrement();

        rr.mRequest = request;
        rr.mResult = result;
        rr.mParcel = Parcel->Obtain();

        If (result != NULL && result->GetTarget() == NULL) {
            throw new NullPointerException("Message target must not be NULL");
        }

        // first elements in any RIL Parcel
        rr.mParcel->WriteInt(request);
        rr.mParcel->WriteInt(rr.mSerial);

        return rr;
    }

    /**
     * Returns a RILRequest instance to the pool.
     *
     * Note: This should only be called once per use.
     */
    void Release() {
        Synchronized (sPoolSync) {
            If (sPoolSize < MAX_POOL_SIZE) {
                mNext = sPool;
                sPool = this;
                sPoolSize++;
                mResult = NULL;
            }
        }
    }

    private RILRequest() {
    }

    static void
    ResetSerial() {
        // use a random so that on recovery we probably don't mix old requests
        // with new.
        sNextSerial->Set(sRandom->NextInt());
    }

    String
    SerialString() {
        //Cheesy way to do %04d
        StringBuilder sb = new StringBuilder(8);
        String sn;

        Int64 adjustedSerial = (((Int64)mSerial) - Integer.MIN_VALUE)%10000;

        sn = Long->ToString(adjustedSerial);

        //sb->Append("J[");
        sb->Append('[');
        For (Int32 i = 0, s = sn->Length() ; i < 4 - s; i++) {
            sb->Append('0');
        }

        sb->Append(sn);
        sb->Append(']');
        return sb->ToString();
    }

    void
    OnError(Int32 error, Object ret) {
        CommandException ex;

        ex = CommandException->FromRilErrno(error);

        If (RIL.RILJ_LOGD) Rlog->D(LOG_TAG, SerialString() + "< "
            + RIL->RequestToString(mRequest)
            + " error: " + ex + " ret=" + RIL->RetToString(mRequest, ret));

        If (mResult != NULL) {
            AsyncResult->ForMessage(mResult, ret, ex);
            mResult->SendToTarget();
        }

        If (mParcel != NULL) {
            mParcel->Recycle();
            mParcel = NULL;
        }
    }
}


/**
 * RIL implementation of the CommandsInterface.
 *
 * {@hide}
 */
public class RIL extends BaseCommands implements CommandsInterface {
    static const String RILJ_LOG_TAG = "RILJ";
    static const Boolean RILJ_LOGD = TRUE;
    static const Boolean RILJ_LOGV = FALSE; // STOPSHIP if TRUE

    /**
     * Wake lock timeout should be longer than the longest timeout in
     * the vendor ril.
     */
    private static const Int32 DEFAULT_WAKE_LOCK_TIMEOUT = 60000;
    private static const Int32 BYTE_SIZE = 1;

    /** Starting number for OEMHOOK request and response IDs */
    private static const Int32 OEMHOOK_BASE = 0x80000;
    final Int32 OEMHOOK_UNSOL_SIM_REFRESH = OEMHOOK_BASE + 1016;
    final Int32 OEMHOOK_UNSOL_WWAN_IWLAN_COEXIST = OEMHOOK_BASE + 1018;

    /** Set Local Call Hold subscription */
    private static const Int32 OEMHOOK_EVT_HOOK_SET_LOCAL_CALL_HOLD = OEMHOOK_BASE + 13;
    /** Get Modem Capabilities*/
    private static const Int32 OEMHOOK_EVT_HOOK_GET_MODEM_CAPABILITY = OEMHOOK_BASE + 35;
    /** Update Stack Binding*/
    private static const Int32 OEMHOOK_EVT_HOOK_UPDATE_SUB_BINDING = OEMHOOK_BASE + 36;


    private static const Int32 INT_SIZE = 4;
    private static const String OEM_IDENTIFIER = "QOEMHOOK";
    Int32 mHeaderSize = OEM_IDENTIFIER->Length() + 2 * INT_SIZE;
    final Int32 QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY = OEMHOOK_BASE + 1020;

    //***** Instance Variables

    LocalSocket mSocket;
    HandlerThread mSenderThread;
    RILSender mSender;
    Thread mReceiverThread;
    RILReceiver mReceiver;
    Display mDefaultDisplay;
    Int32 mDefaultDisplayState = Display.STATE_UNKNOWN;
    WakeLock mWakeLock;
    final Int32 mWakeLockTimeout;
    // The number of wakelock requests currently active.  Don't release the lock
    // until dec'd to 0
    Int32 mWakeLockCount;

    SparseArray<RILRequest> mRequestList = new SparseArray<RILRequest>();

    Object     mLastNITZTimeInfo;

    // When we are testing emergency calls
    AtomicBoolean mTestingEmergencyCall = new AtomicBoolean(FALSE);

    protected Integer mInstanceId;

    // Number of per-network elements expected in QUERY_AVAILABLE_NETWORKS's response.
    // 4 elements is default, but many RILs actually return 5, making it impossible to
    // divide the response array without prior knowledge of the number of elements.
    protected Int32 mQANElements = 4;

    //***** Events

    static const Int32 EVENT_SEND                 = 1;
    static const Int32 EVENT_WAKE_LOCK_TIMEOUT    = 2;

    //***** Constants

    // match with constant in ril.cpp
    static const Int32 RIL_MAX_COMMAND_BYTES = (8 * 1024);
    static const Int32 RESPONSE_SOLICITED = 0;
    static const Int32 RESPONSE_UNSOLICITED = 1;

    static const String[] SOCKET_NAME_RIL = {"rild", "rild2", "rild3"};

    static const Int32 SOCKET_OPEN_RETRY_MILLIS = 4 * 1000;

    // The number of the required config values for broadcast SMS stored in the C struct
    // RIL_CDMA_BroadcastServiceInfo
    private static const Int32 CDMA_BSI_NO_OF_INTS_STRUCT = 3;

    private static const Int32 CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES = 31;

    private final DisplayManager.DisplayListener mDisplayListener =
            new DisplayManager->DisplayListener() {
        //@Override
        CARAPI OnDisplayAdded(Int32 displayId) { }

        //@Override
        CARAPI OnDisplayRemoved(Int32 displayId) { }

        //@Override
        CARAPI OnDisplayChanged(Int32 displayId) {
            If (displayId == Display.DEFAULT_DISPLAY) {
                UpdateScreenState();
            }
        }
    };

    class RILSender extends Handler implements Runnable {
        public RILSender(Looper looper) {
            Super(looper);
        }

        // Only allocated once
        Byte[] dataLength = new Byte[4];

        //***** Runnable implementation
        //@Override
        CARAPI
        Run() {
            //setup if needed
        }


        //***** Handler implementation
        //@Override CARAPI
        HandleMessage(Message msg) {
            RILRequest rr = (RILRequest)(msg.obj);
            RILRequest req = NULL;

            Switch (msg.what) {
                case EVENT_SEND:
                    try {
                        LocalSocket s;

                        s = mSocket;

                        If (s == NULL) {
                            rr->OnError(RADIO_NOT_AVAILABLE, NULL);
                            rr->Release();
                            DecrementWakeLock();
                            return;
                        }

                        Synchronized (mRequestList) {
                            mRequestList->Append(rr.mSerial, rr);
                        }

                        Byte[] data;

                        data = rr.mParcel->Marshall();
                        rr.mParcel->Recycle();
                        rr.mParcel = NULL;

                        If (data.length > RIL_MAX_COMMAND_BYTES) {
                            throw new RuntimeException(
                                    "Parcel larger than max bytes allowed! "
                                                          + data.length);
                        }

                        // parcel length in big endian
                        dataLength[0] = dataLength[1] = 0;
                        dataLength[2] = (Byte)((data.length >> 8) & 0xff);
                        dataLength[3] = (Byte)((data.length) & 0xff);

                        //Rlog->V(RILJ_LOG_TAG, "writing packet: " + data.length + " bytes");

                        s->GetOutputStream()->Write(dataLength);
                        s->GetOutputStream()->Write(data);
                    } Catch (IOException ex) {
                        Rlog->E(RILJ_LOG_TAG, "IOException", ex);
                        req = FindAndRemoveRequestFromList(rr.mSerial);
                        // make sure this request has not already been handled,
                        // eg, if RILReceiver cleared the list.
                        If (req != NULL) {
                            rr->OnError(RADIO_NOT_AVAILABLE, NULL);
                            rr->Release();
                            DecrementWakeLock();
                        }
                    } Catch (RuntimeException exc) {
                        Rlog->E(RILJ_LOG_TAG, "Uncaught exception ", exc);
                        req = FindAndRemoveRequestFromList(rr.mSerial);
                        // make sure this request has not already been handled,
                        // eg, if RILReceiver cleared the list.
                        If (req != NULL) {
                            rr->OnError(GENERIC_FAILURE, NULL);
                            rr->Release();
                            DecrementWakeLock();
                        }
                    }

                    break;

                case EVENT_WAKE_LOCK_TIMEOUT:
                    // Haven't heard back from the last request.  Assume we're
                    // not getting a response and  release the wake lock.

                    // The timer of WAKE_LOCK_TIMEOUT is reset with each
                    // new send request. So when WAKE_LOCK_TIMEOUT occurs
                    // all requests in mRequestList already waited at
                    // least DEFAULT_WAKE_LOCK_TIMEOUT but no response.
                    //
                    // Note: Keep mRequestList so that delayed response
                    // can still be handled when response finally comes.

                    Synchronized (mRequestList) {
                        If (ClearWakeLock()) {
                            If (RILJ_LOGD) {
                                Int32 count = mRequestList->Size();
                                Rlog->D(RILJ_LOG_TAG, "WAKE_LOCK_TIMEOUT " +
                                        " mRequestList=" + count);
                                For (Int32 i = 0; i < count; i++) {
                                    rr = mRequestList->ValueAt(i);
                                    Rlog->D(RILJ_LOG_TAG, i + ": [" + rr.mSerial + "] "
                                            + RequestToString(rr.mRequest));
                                }
                            }
                        }
                    }
                    break;
            }
        }
    }

    /**
     * Reads in a single RIL message off the wire. A RIL message consists
     * of a 4-Byte little-endian length and a subsequent series of bytes.
     * The final Message (length header omitted) is read into
     * <code>buffer</code> and the length of the final Message (less header)
     * is returned. A return value of -1 indicates end-of-stream.
     *
     * @param is non-NULL; Stream to read from
     * @param buffer Buffer to fill in. Must be as large as maximum
     * message size, or an ArrayOutOfBounds exception will be thrown.
     * @return Length of message less header, or -1 on end of stream.
     * @throws IOException
     */
    private static Int32 ReadRilMessage(InputStream is, Byte[] buffer)
            throws IOException {
        Int32 countRead;
        Int32 offset;
        Int32 remaining;
        Int32 messageLength;

        // First, read in the length of the message
        offset = 0;
        remaining = 4;
        do {
            countRead = is->Read(buffer, offset, remaining);

            If (countRead < 0 ) {
                Rlog->E(RILJ_LOG_TAG, "Hit EOS reading message length");
                return -1;
            }

            offset += countRead;
            remaining -= countRead;
        } While (remaining > 0);

        messageLength = ((buffer[0] & 0xff) << 24)
                | ((buffer[1] & 0xff) << 16)
                | ((buffer[2] & 0xff) << 8)
                | (buffer[3] & 0xff);

        // Then, re-use the buffer and read in the message itself
        offset = 0;
        remaining = messageLength;
        do {
            countRead = is->Read(buffer, offset, remaining);

            If (countRead < 0 ) {
                Rlog->E(RILJ_LOG_TAG, "Hit EOS reading message.  messageLength=" + messageLength
                        + " remaining=" + remaining);
                return -1;
            }

            offset += countRead;
            remaining -= countRead;
        } While (remaining > 0);

        return messageLength;
    }

    class RILReceiver implements Runnable {
        Byte[] buffer;

        RILReceiver() {
            buffer = new Byte[RIL_MAX_COMMAND_BYTES];
        }

        //@Override
        CARAPI
        Run() {
            Int32 retryCount = 0;
            String rilSocket = "rild";

            try {For (;;) {
                LocalSocket s = NULL;
                LocalSocketAddress l;

                If (mInstanceId == NULL || mInstanceId == 0 ) {
                    rilSocket = SOCKET_NAME_RIL[0];
                } else {
                    rilSocket = SOCKET_NAME_RIL[mInstanceId];
                }

                try {
                    s = new LocalSocket();
                    l = new LocalSocketAddress(rilSocket,
                            LocalSocketAddress.Namespace.RESERVED);
                    s->Connect(l);
                } Catch (IOException ex){
                    try {
                        If (s != NULL) {
                            s->Close();
                        }
                    } Catch (IOException ex2) {
                        //ignore failure to close after failure to connect
                    }

                    // don't print an error message after the the first time
                    // or after the 8th time

                    If (retryCount == 8) {
                        Rlog.E (RILJ_LOG_TAG,
                            "Couldn't find '" + rilSocket
                            + "' socket after " + retryCount
                            + " times, continuing to retry silently");
                    } else If (retryCount > 0 && retryCount < 8) {
                        Rlog.I (RILJ_LOG_TAG,
                            "Couldn't find '" + rilSocket
                            + "' socket; retrying after timeout");
                    }

                    try {
                        Thread->Sleep(SOCKET_OPEN_RETRY_MILLIS);
                    } Catch (InterruptedException er) {
                    }

                    retryCount++;
                    continue;
                }

                retryCount = 0;

                mSocket = s;
                Rlog->I(RILJ_LOG_TAG, "Connected to '" + rilSocket + "' socket");

                /* Compatibility with qcom's DSDS (Dual SIM) stack */
                If (NeedsOldRilFeature("qcomdsds")) {
                    String str = "SUB1";
                    Byte[] data = str->GetBytes();
                    try {
                        mSocket->GetOutputStream()->Write(data);
                        Rlog->I(RILJ_LOG_TAG, "Data sent!!");
                    } Catch (IOException ex) {
                            Rlog->E(RILJ_LOG_TAG, "IOException", ex);
                    } Catch (RuntimeException exc) {
                        Rlog->E(RILJ_LOG_TAG, "Uncaught exception ", exc);
                    }
                }

                Int32 length = 0;
                try {
                    InputStream is = mSocket->GetInputStream();

                    For (;;) {
                        Parcel p;

                        length = ReadRilMessage(is, buffer);

                        If (length < 0) {
                            // End-of-stream reached
                            break;
                        }

                        p = Parcel->Obtain();
                        p->Unmarshall(buffer, 0, length);
                        p->SetDataPosition(0);

                        //Rlog->V(RILJ_LOG_TAG, "Read packet: " + length + " bytes");

                        ProcessResponse(p);
                        p->Recycle();
                    }
                } Catch (java.io.IOException ex) {
                    Rlog->I(RILJ_LOG_TAG, "'" + rilSocket + "' socket closed",
                          ex);
                } Catch (Throwable tr) {
                    Rlog->E(RILJ_LOG_TAG, "Uncaught exception read length=" + length +
                        "Exception:" + tr->ToString());
                }

                Rlog->I(RILJ_LOG_TAG, "Disconnected from '" + rilSocket
                      + "' socket");

                SetRadioState (RadioState.RADIO_UNAVAILABLE);

                try {
                    mSocket->Close();
                } Catch (IOException ex) {
                }

                mSocket = NULL;
                RILRequest->ResetSerial();

                // Clear request list on close
                ClearRequestList(RADIO_NOT_AVAILABLE, FALSE);
            }} Catch (Throwable tr) {
                Rlog->E(RILJ_LOG_TAG,"Uncaught exception", tr);
            }

            /* We're disconnected so we don't know the ril version */
            NotifyRegistrantsRilConnectionChanged(-1);
        }
    }



    //***** Constructors

    public RIL(Context context, Int32 preferredNetworkType, Int32 cdmaSubscription) {
        This(context, preferredNetworkType, cdmaSubscription, NULL);
    }

    public RIL(Context context, Int32 preferredNetworkType,
            Int32 cdmaSubscription, Integer instanceId) {
        Super(context);
        If (RILJ_LOGD) {
            RiljLog("RIL(context, preferredNetworkType=" + preferredNetworkType +
                    " cdmaSubscription=" + cdmaSubscription + ")");
        }

        mContext = context;
        mCdmaSubscription  = cdmaSubscription;
        mPreferredNetworkType = preferredNetworkType;
        mPhoneType = RILConstants.NO_PHONE;
        mInstanceId = instanceId;

        PowerManager pm = (PowerManager)context->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, RILJ_LOG_TAG);
        mWakeLock->SetReferenceCounted(FALSE);
        mWakeLockTimeout = SystemProperties->GetInt(TelephonyProperties.PROPERTY_WAKE_LOCK_TIMEOUT,
                DEFAULT_WAKE_LOCK_TIMEOUT);
        mWakeLockCount = 0;

        mSenderThread = new HandlerThread("RILSender");
        mSenderThread->Start();

        Looper looper = mSenderThread->GetLooper();
        mSender = new RILSender(looper);

        ConnectivityManager cm = (ConnectivityManager)context->GetSystemService(
                Context.CONNECTIVITY_SERVICE);
        If (cm->IsNetworkSupported(ConnectivityManager.TYPE_MOBILE) == FALSE) {
            RiljLog("Not starting RILReceiver: wifi-only");
        } else {
            RiljLog("Starting RILReceiver");
            mReceiver = new RILReceiver();
            mReceiverThread = new Thread(mReceiver, "RILReceiver");
            mReceiverThread->Start();

            DisplayManager dm = (DisplayManager)context->GetSystemService(
                    Context.DISPLAY_SERVICE);
            mDefaultDisplay = dm->GetDisplay(Display.DEFAULT_DISPLAY);
            dm->RegisterDisplayListener(mDisplayListener, NULL);
        }

        TelephonyDevController tdc = TelephonyDevController->GetInstance();
        tdc->RegisterRIL(this);
    }

    //***** CommandsInterface implementation

    //@Override
    CARAPI GetVoiceRadioTechnology(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_VOICE_RADIO_TECH, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }


    CARAPI GetImsRegistrationState(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_IMS_REGISTRATION_STATE, result);

        If (RILJ_LOGD) {
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));
        }
        Send(rr);
    }

    //@Override CARAPI
    SetOnNITZTime(Handler h, Int32 what, Object obj) {
        super->SetOnNITZTime(h, what, obj);

        // Send the last NITZ time if we have it
        If (mLastNITZTimeInfo != NULL) {
            mNITZTimeRegistrant
                .NotifyRegistrant(
                    new AsyncResult (NULL, mLastNITZTimeInfo, NULL));
            mLastNITZTimeInfo = NULL;
        }
    }

    //@Override
    CARAPI
    GetIccCardStatus(Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_SIM_STATUS, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    CARAPI SetUiccSubscription(Int32 slotId, Int32 appIndex, Int32 subId,
            Int32 subStatus, Message result) {
        //Note: This RIL request is also valid for SIM and RUIM (ICC card)
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SET_UICC_SUBSCRIPTION, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " slot: " + slotId + " appIndex: " + appIndex
                + " subId: " + subId + " subStatus: " + subStatus);

        rr.mParcel->WriteInt(slotId);
        rr.mParcel->WriteInt(appIndex);
        rr.mParcel->WriteInt(subId);
        rr.mParcel->WriteInt(subStatus);

        Send(rr);
    }

    // FIXME This API should take an AID and slot ID
    CARAPI SetDataAllowed(Boolean allowed, Message result) {
        If(mRilVersion < 10 && mInstanceId == NULL) {
            If (result != NULL) {
                CommandException ex = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
                AsyncResult->ForMessage(result, NULL, ex);
                result->SendToTarget();
            }
            return;
        }

        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ALLOW_DATA, result);
        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " " + allowed);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(allowed ? 1 : 0);
        Send(rr);
    }

    CARAPI
    GetDataCallProfile(Int32 appType, Message result) {
        If(mRilVersion < 10) {
            If (result != NULL) {
                CommandException ex = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
                AsyncResult->ForMessage(result, NULL, ex);
                result->SendToTarget();
            }
            return;
        }

        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_GET_DATA_CALL_PROFILE, result);

        // count of ints
        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(appType);

        If (RILJ_LOGD) {
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                   + " : " + appType);
        }
        Send(rr);
    }

    //@Override CARAPI
    SupplyIccPin(String pin, Message result) {
        SupplyIccPinForApp(pin, NULL, result);
    }

    //@Override CARAPI
    SupplyIccPinForApp(String pin, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ENTER_SIM_PIN, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        rr.mParcel->WriteInt(oldRil ? 1 : 2);
        rr.mParcel->WriteString(pin);

        If (!oldRil)
            rr.mParcel->WriteString(aid);

        Send(rr);
    }

    //@Override CARAPI
    SupplyIccPuk(String puk, String newPin, Message result) {
        SupplyIccPukForApp(puk, newPin, NULL, result);
    }

    //@Override CARAPI
    SupplyIccPukForApp(String puk, String newPin, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ENTER_SIM_PUK, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        rr.mParcel->WriteInt(oldRil ? 2 : 3);
        rr.mParcel->WriteString(puk);
        rr.mParcel->WriteString(newPin);

        If (!oldRil)
            rr.mParcel->WriteString(aid);

        Send(rr);
    }

    //@Override CARAPI
    SupplyIccPin2(String pin, Message result) {
        SupplyIccPin2ForApp(pin, NULL, result);
    }

    //@Override CARAPI
    SupplyIccPin2ForApp(String pin, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ENTER_SIM_PIN2, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        rr.mParcel->WriteInt(oldRil ? 1 : 2);
        rr.mParcel->WriteString(pin);

        If (!oldRil)
            rr.mParcel->WriteString(aid);

        Send(rr);
    }

    //@Override CARAPI
    SupplyIccPuk2(String puk2, String newPin2, Message result) {
        SupplyIccPuk2ForApp(puk2, newPin2, NULL, result);
    }

    //@Override CARAPI
    SupplyIccPuk2ForApp(String puk, String newPin2, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ENTER_SIM_PUK2, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        rr.mParcel->WriteInt(oldRil ? 2 : 3);
        rr.mParcel->WriteString(puk);
        rr.mParcel->WriteString(newPin2);

        If (!oldRil)
            rr.mParcel->WriteString(aid);

        Send(rr);
    }

    //@Override CARAPI
    ChangeIccPin(String oldPin, String newPin, Message result) {
        ChangeIccPinForApp(oldPin, newPin, NULL, result);
    }

    //@Override CARAPI
    ChangeIccPinForApp(String oldPin, String newPin, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CHANGE_SIM_PIN, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        rr.mParcel->WriteInt(oldRil ? 2 : 3);
        rr.mParcel->WriteString(oldPin);
        rr.mParcel->WriteString(newPin);

        If (!oldRil)
            rr.mParcel->WriteString(aid);

        Send(rr);
    }

    //@Override CARAPI
    ChangeIccPin2(String oldPin2, String newPin2, Message result) {
        ChangeIccPin2ForApp(oldPin2, newPin2, NULL, result);
    }

    //@Override CARAPI
    ChangeIccPin2ForApp(String oldPin2, String newPin2, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CHANGE_SIM_PIN2, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        rr.mParcel->WriteInt(oldRil ? 2 : 3);
        rr.mParcel->WriteString(oldPin2);
        rr.mParcel->WriteString(newPin2);

        If (!oldRil)
            rr.mParcel->WriteString(aid);

        Send(rr);
    }

    //@Override
    CARAPI
    ChangeBarringPassword(String facility, String oldPwd, String newPwd, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CHANGE_BARRING_PASSWORD, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        rr.mParcel->WriteInt(3);
        rr.mParcel->WriteString(facility);
        rr.mParcel->WriteString(oldPwd);
        rr.mParcel->WriteString(newPwd);

        Send(rr);
    }

    //@Override
    CARAPI
    SupplyDepersonalization(String netpin, String type, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest) +
                        " Type:" + type);

        rr.mParcel->WriteInt(2);
        rr.mParcel->WriteString(type);
        rr.mParcel->WriteString(netpin);

        Send(rr);
    }

    //@Override
    CARAPI
    GetCurrentCalls (Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_CURRENT_CALLS, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    //@Deprecated CARAPI
    GetPDPContextList(Message result) {
        GetDataCallList(result);
    }

    //@Override
    CARAPI
    GetDataCallList(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_DATA_CALL_LIST, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    Dial (String address, Int32 clirMode, Message result) {
        Dial(address, clirMode, NULL, result);
    }

    //@Override
    CARAPI
    Dial(String address, Int32 clirMode, UUSInfo uusInfo, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_DIAL, result);

        rr.mParcel->WriteString(address);
        rr.mParcel->WriteInt(clirMode);

        If (uusInfo == NULL) {
            rr.mParcel->WriteInt(0); // UUS information is absent
        } else {
            rr.mParcel->WriteInt(1); // UUS information is present
            rr.mParcel->WriteInt(uusInfo->GetType());
            rr.mParcel->WriteInt(uusInfo->GetDcs());
            rr.mParcel->WriteByteArray(uusInfo->GetUserData());
        }

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetIMSI(Message result) {
        GetIMSIForApp(NULL, result);
    }

    //@Override
    CARAPI
    GetIMSIForApp(String aid, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_IMSI, result);

        Boolean skipNullAid = NeedsOldRilFeature("skipnullaid");
        Boolean writeAidOnly = NeedsOldRilFeature("writeaidonly");

        If (!writeAidOnly && (aid != NULL || !skipNullAid)) {
            rr.mParcel->WriteInt(1);
            rr.mParcel->WriteString(aid);
        }

        If (writeAidOnly)
            rr.mParcel->WriteString(aid);

        If (RILJ_LOGD) RiljLog(rr->SerialString() +
                              "> getIMSI: " + RequestToString(rr.mRequest)
                              + " aid: " + aid);

        Send(rr);
    }

    //@Override
    CARAPI
    GetIMEI(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_IMEI, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetIMEISV(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_IMEISV, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }


    //@Override
    CARAPI
    HangupConnection (Int32 gsmIndex, Message result) {
        If (RILJ_LOGD) RiljLog("hangupConnection: gsmIndex=" + gsmIndex);

        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_HANGUP, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest) + " " +
                gsmIndex);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(gsmIndex);

        Send(rr);
    }

    //@Override
    CARAPI
    HangupWaitingOrBackground (Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
                                        result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    HangupForegroundResumeBackground (Message result) {
        RILRequest rr
                = RILRequest->Obtain(
                        RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
                                        result);
        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SwitchWaitingOrHoldingAndActive (Message result) {
        RILRequest rr
                = RILRequest->Obtain(
                        RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
                                        result);
        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    Conference (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_CONFERENCE, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }


    //@Override
    CARAPI SetPreferredVoicePrivacy(Boolean enable, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
                result);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(enable ? 1:0);

        Send(rr);
    }

    //@Override
    CARAPI GetPreferredVoicePrivacy(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
                result);
        Send(rr);
    }

    //@Override
    CARAPI
    SeparateConnection (Int32 gsmIndex, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SEPARATE_CONNECTION, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                            + " " + gsmIndex);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(gsmIndex);

        Send(rr);
    }

    //@Override
    CARAPI
    AcceptCall (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_ANSWER, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    RejectCall (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_UDUB, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    ExplicitCallTransfer (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_EXPLICIT_CALL_TRANSFER, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetLastCallFailCause (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_LAST_CALL_FAIL_CAUSE, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * @deprecated
     */
    //@Deprecated
    //@Override
    CARAPI
    GetLastPdpFailCause (Message result) {
        GetLastDataCallFailCause (result);
    }

    /**
     * The preferred new alternative to getLastPdpFailCause
     */
    //@Override
    CARAPI
    GetLastDataCallFailCause (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SetMute (Boolean enableMute, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_MUTE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                            + " " + enableMute);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(enableMute ? 1 : 0);

        Send(rr);
    }

    //@Override
    CARAPI
    GetMute (Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_GET_MUTE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetSignalStrength (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SIGNAL_STRENGTH, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetVoiceRegistrationState (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_VOICE_REGISTRATION_STATE, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetDataRegistrationState (Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_DATA_REGISTRATION_STATE, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetOperator(Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_OPERATOR, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetHardwareConfig (Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_HARDWARE_CONFIG, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SendDtmf(Char32 c, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_DTMF, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        rr.mParcel->WriteString(Character->ToString(c));

        Send(rr);
    }

    //@Override
    CARAPI
    StartDtmf(Char32 c, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_DTMF_START, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        rr.mParcel->WriteString(Character->ToString(c));

        Send(rr);
    }

    //@Override
    CARAPI
    StopDtmf(Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_DTMF_STOP, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_BURST_DTMF, result);

        rr.mParcel->WriteInt(3);
        rr.mParcel->WriteString(dtmfString);
        rr.mParcel->WriteString(Integer->ToString(on));
        rr.mParcel->WriteString(Integer->ToString(off));

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + dtmfString);

        Send(rr);
    }

    private void
    ConstructGsmSendSmsRilRequest (RILRequest rr, String smscPDU, String pdu) {
        rr.mParcel->WriteInt(2);
        rr.mParcel->WriteString(smscPDU);
        rr.mParcel->WriteString(pdu);
    }

    CARAPI
    SendSMS (String smscPDU, String pdu, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SEND_SMS, result);

        ConstructGsmSendSmsRilRequest(rr, smscPDU, pdu);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SendSMSExpectMore (String smscPDU, String pdu, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SEND_SMS_EXPECT_MORE, result);

        ConstructGsmSendSmsRilRequest(rr, smscPDU, pdu);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    private void
    ConstructCdmaSendSmsRilRequest(RILRequest rr, Byte[] pdu) {
        Int32 address_nbr_of_digits;
        Int32 subaddr_nbr_of_digits;
        Int32 bearerDataLength;
        ByteArrayInputStream bais = new ByteArrayInputStream(pdu);
        DataInputStream dis = new DataInputStream(bais);

        try {
            rr.mParcel->WriteInt(dis->ReadInt()); //teleServiceId
            rr.mParcel->WriteByte((Byte) dis->ReadInt()); //servicePresent
            rr.mParcel->WriteInt(dis->ReadInt()); //serviceCategory
            rr.mParcel->WriteInt(dis->Read()); //address_digit_mode
            rr.mParcel->WriteInt(dis->Read()); //address_nbr_mode
            rr.mParcel->WriteInt(dis->Read()); //address_ton
            rr.mParcel->WriteInt(dis->Read()); //address_nbr_plan
            address_nbr_of_digits = (Byte) dis->Read();
            rr.mParcel->WriteByte((Byte) address_nbr_of_digits);
            For(Int32 i=0; i < address_nbr_of_digits; i++){
                rr.mParcel->WriteByte(dis->ReadByte()); // address_orig_bytes[i]
            }
            rr.mParcel->WriteInt(dis->Read()); //subaddressType
            rr.mParcel->WriteByte((Byte) dis->Read()); //subaddr_odd
            subaddr_nbr_of_digits = (Byte) dis->Read();
            rr.mParcel->WriteByte((Byte) subaddr_nbr_of_digits);
            For(Int32 i=0; i < subaddr_nbr_of_digits; i++){
                rr.mParcel->WriteByte(dis->ReadByte()); //subaddr_orig_bytes[i]
            }

            bearerDataLength = dis->Read();
            rr.mParcel->WriteInt(bearerDataLength);
            For(Int32 i=0; i < bearerDataLength; i++){
                rr.mParcel->WriteByte(dis->ReadByte()); //bearerData[i]
            }
        }Catch (IOException ex){
            If (RILJ_LOGD) RiljLog("sendSmsCdma: conversion from input stream to object failed: "
                    + ex);
        }
    }

    CARAPI
    SendCdmaSms(Byte[] pdu, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_CDMA_SEND_SMS, result);

        ConstructCdmaSendSmsRilRequest(rr, pdu);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    CARAPI
    SendImsGsmSms (String smscPDU, String pdu, Int32 retry, Int32 messageRef,
            Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_IMS_SEND_SMS, result);

        rr.mParcel->WriteInt(RILConstants.GSM_PHONE);
        rr.mParcel->WriteByte((Byte)retry);
        rr.mParcel->WriteInt(messageRef);

        ConstructGsmSendSmsRilRequest(rr, smscPDU, pdu);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    CARAPI
    SendImsCdmaSms(Byte[] pdu, Int32 retry, Int32 messageRef, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_IMS_SEND_SMS, result);

        rr.mParcel->WriteInt(RILConstants.CDMA_PHONE);
        rr.mParcel->WriteByte((Byte)retry);
        rr.mParcel->WriteInt(messageRef);

        ConstructCdmaSendSmsRilRequest(rr, pdu);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI DeleteSmsOnSim(Int32 index, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_DELETE_SMS_ON_SIM,
                response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(index);

        If (RILJ_LOGV) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest)
                + " " + index);

        Send(rr);
    }

    //@Override
    CARAPI DeleteSmsOnRuim(Int32 index, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM,
                response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(index);

        If (RILJ_LOGV) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest)
                + " " + index);

        Send(rr);
    }

    //@Override
    CARAPI WriteSmsToSim(Int32 status, String smsc, String pdu, Message response) {
        status = TranslateStatus(status);

        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_WRITE_SMS_TO_SIM,
                response);

        rr.mParcel->WriteInt(status);
        rr.mParcel->WriteString(pdu);
        rr.mParcel->WriteString(smsc);

        If (RILJ_LOGV) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest)
                + " " + status);

        Send(rr);
    }

    //@Override
    CARAPI WriteSmsToRuim(Int32 status, String pdu, Message response) {
        status = TranslateStatus(status);

        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM,
                response);

        rr.mParcel->WriteInt(status);
        ConstructCdmaWriteSmsRilRequest(rr, IccUtils->HexStringToBytes(pdu));

        If (RILJ_LOGV) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest)
                + " " + status);

        Send(rr);
    }

    /**
     *  Restructures PDU data so that it is consistent with RIL
     *  data structure.
     *
     *  @param pdu The data to be written to the RUIM card.
     */
    private void ConstructCdmaWriteSmsRilRequest(RILRequest rr, Byte[] pdu) {
        Int32 address_nbr_of_digits;
        Int32 subaddr_nbr_of_digits;
        Int32 bearerDataLength;
        ByteArrayInputStream bais = new ByteArrayInputStream(pdu);
        DataInputStream dis = new DataInputStream(bais);

        try {
            Int32 teleServiceId = 0;
            Byte servicePresent = 0;
            Int32 serviceCategory = 0;

            Int32 address_digit_mode = 0;
            Int32 address_nbr_mode = 0;
            Int32 address_ton = 0;
            Int32 address_nbr_plan = 0;

            Int32 subaddressType = 0;
            Byte subaddr_odd = 0;

            teleServiceId = dis->ReadInt();
            rr.mParcel->WriteInt(teleServiceId);
            servicePresent = (Byte) dis->ReadInt();
            rr.mParcel->WriteByte(servicePresent);
            serviceCategory = dis->ReadInt();
            rr.mParcel->WriteInt(serviceCategory);

            address_digit_mode = dis->ReadByte();
            rr.mParcel->WriteInt(address_digit_mode);
            address_nbr_mode = dis->ReadByte();
            rr.mParcel->WriteInt(address_nbr_mode);
            address_ton = dis->ReadByte();
            rr.mParcel->WriteInt(address_ton);
            address_nbr_plan = dis->ReadByte();
            rr.mParcel->WriteInt(address_nbr_plan);

            address_nbr_of_digits = dis->ReadByte();
            rr.mParcel->WriteByte((Byte) address_nbr_of_digits);
            For (Int32 i = 0; i < address_nbr_of_digits; i++) {
                rr.mParcel->WriteByte(dis->ReadByte()); // address_orig_bytes[i]
            }

            // Int32
            subaddressType = dis->ReadByte();
            rr.mParcel->WriteInt(subaddressType); // subaddressType
            subaddr_odd = (Byte) dis->ReadByte();
            rr.mParcel->WriteByte(subaddr_odd); // subaddr_odd
            subaddr_nbr_of_digits = (Byte) dis->ReadByte();
            rr.mParcel->WriteByte((Byte) subaddr_nbr_of_digits);
            For (Int32 i = 0; i < subaddr_nbr_of_digits; i++) {
                rr.mParcel->WriteByte(dis->ReadByte()); // subaddr_orig_bytes[i]
            }

            bearerDataLength = dis->ReadByte() & 0xff;
            rr.mParcel->WriteInt(bearerDataLength);
            For (Int32 i = 0; i < bearerDataLength; i++) {
                rr.mParcel->WriteByte(dis->ReadByte()); // bearerData[i]
            }

            RiljLog(" teleServiceId=" + teleServiceId + " servicePresent=" + servicePresent
                + " serviceCategory=" + serviceCategory
                + " address_digit_mode=" + address_digit_mode
                + " address_nbr_mode=" + address_nbr_mode + " address_ton=" + address_ton
                + " address_nbr_plan=" + address_nbr_plan
                + " address_nbr_of_digits=" + address_nbr_of_digits
                + " subaddressType=" + subaddressType + " subaddr_odd= " + subaddr_odd
                + " subaddr_nbr_of_digits=" + subaddr_nbr_of_digits
                + " bearerDataLength=" + bearerDataLength);
        } Catch (IOException ex) {
            If (RILJ_LOGD) RiljLog("sendSmsCdma: conversion from input stream to object failed: "
                    + ex);
        } finally {
            try {
                If (NULL != bais) {
                    bais->Close();
                }

                If (NULL != dis) {
                    dis->Close();
                }
            } Catch (IOException e) {
                If (RILJ_LOGD) RiljLog("sendSmsCdma: close input stream exception" + e);
            }
        }
    }

    /**
     *  Translates EF_SMS status bits to a status value compatible with
     *  SMS AT commands.  See TS 27.005 3.1.
     */
    private Int32 TranslateStatus(Int32 status) {
        Switch(status & 0x7) {
            case SmsManager.STATUS_ON_ICC_READ:
                return 1;
            case SmsManager.STATUS_ON_ICC_UNREAD:
                return 0;
            case SmsManager.STATUS_ON_ICC_SENT:
                return 3;
            case SmsManager.STATUS_ON_ICC_UNSENT:
                return 2;
        }

        // Default to READ.
        return 1;
    }

    //@Override
    CARAPI
    SetupDataCall(String radioTechnology, String profile, String apn,
            String user, String password, String authType, String protocol,
            Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SETUP_DATA_CALL, result);

        rr.mParcel->WriteInt(7);

        rr.mParcel->WriteString(radioTechnology);
        rr.mParcel->WriteString(profile);
        rr.mParcel->WriteString(apn);
        rr.mParcel->WriteString(user);
        rr.mParcel->WriteString(password);
        rr.mParcel->WriteString(authType);
        rr.mParcel->WriteString(protocol);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest) + " " + radioTechnology + " "
                + profile + " " + apn + " " + user + " "
                + password + " " + authType + " " + protocol);

        Send(rr);
    }

    //@Override
    CARAPI
    DeactivateDataCall(Int32 cid, Int32 reason, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_DEACTIVATE_DATA_CALL, result);

        rr.mParcel->WriteInt(2);
        rr.mParcel->WriteString(Integer->ToString(cid));
        rr.mParcel->WriteString(Integer->ToString(reason));

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " +
                RequestToString(rr.mRequest) + " " + cid + " " + reason);

        Send(rr);
    }

    //@Override
    CARAPI
    SetRadioPower(Boolean on, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_RADIO_POWER, result);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(on ? 1 : 0);

        If (RILJ_LOGD) {
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + (on ? " on" : " off"));
        }

        Send(rr);
    }

    //@Override
    CARAPI RequestShutdown(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SHUTDOWN, result);

        If (RILJ_LOGD)
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SetSuppServiceNotifications(Boolean enable, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, result);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(enable ? 1 : 0);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    AcknowledgeLastIncomingGsmSms(Boolean success, Int32 cause, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SMS_ACKNOWLEDGE, result);

        rr.mParcel->WriteInt(2);
        rr.mParcel->WriteInt(success ? 1 : 0);
        rr.mParcel->WriteInt(cause);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " " + success + " " + cause);

        Send(rr);
    }

    //@Override
    CARAPI
    AcknowledgeLastIncomingCdmaSms(Boolean success, Int32 cause, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE, result);

        rr.mParcel->WriteInt(success ? 0 : 1); //RIL_CDMA_SMS_ErrorClass
        // cause code according to X.S004-550E
        rr.mParcel->WriteInt(cause);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " " + success + " " + cause);

        Send(rr);
    }

    //@Override
    CARAPI
    AcknowledgeIncomingGsmSmsWithPdu(Boolean success, String ackPdu, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU, result);

        rr.mParcel->WriteInt(2);
        rr.mParcel->WriteString(success ? "1" : "0");
        rr.mParcel->WriteString(ackPdu);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + ' ' + success + " [" + ackPdu + ']');

        Send(rr);
    }

    //@Override
    CARAPI
    IccIO (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2, Int32 p3,
            String data, String pin2, Message result) {
        IccIOForApp(command, fileid, path, p1, p2, p3, data, pin2, NULL, result);
    }
    //@Override
    CARAPI
    IccIOForApp (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2, Int32 p3,
            String data, String pin2, String aid, Message result) {
        //Note: This RIL request has not been renamed to ICC,
        //       but this request is also valid for SIM and RUIM
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SIM_IO, result);

        rr.mParcel->WriteInt(command);
        rr.mParcel->WriteInt(fileid);
        rr.mParcel->WriteString(path);
        rr.mParcel->WriteInt(p1);
        rr.mParcel->WriteInt(p2);
        rr.mParcel->WriteInt(p3);
        rr.mParcel->WriteString(data);
        rr.mParcel->WriteString(pin2);
        rr.mParcel->WriteString(aid);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> iccIO: "
                + RequestToString(rr.mRequest)
                + " 0x" + Integer->ToHexString(command)
                + " 0x" + Integer->ToHexString(fileid) + " "
                + " path: " + path + ","
                + p1 + "," + p2 + "," + p3
                + " aid: " + aid);

        Send(rr);
    }

    //@Override
    CARAPI
    GetCLIR(Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_GET_CLIR, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SetCLIR(Int32 clirMode, Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_CLIR, result);

        // count ints
        rr.mParcel->WriteInt(1);

        rr.mParcel->WriteInt(clirMode);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " " + clirMode);

        Send(rr);
    }

    //@Override
    CARAPI
    QueryCallWaiting(Int32 serviceClass, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_QUERY_CALL_WAITING, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(serviceClass);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " " + serviceClass);

        Send(rr);
    }

    //@Override
    CARAPI
    SetCallWaiting(Boolean enable, Int32 serviceClass, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_CALL_WAITING, response);

        rr.mParcel->WriteInt(2);
        rr.mParcel->WriteInt(enable ? 1 : 0);
        rr.mParcel->WriteInt(serviceClass);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " " + enable + ", " + serviceClass);

        Send(rr);
    }

    //@Override
    CARAPI
    SetNetworkSelectionModeAutomatic(Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,
                                    response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SetNetworkSelectionModeManual(String operatorNumeric, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,
                                    response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " " + operatorNumeric);

        rr.mParcel->WriteString(operatorNumeric);

        Send(rr);
    }

    //@Override
    CARAPI
    GetNetworkSelectionMode(Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,
                                    response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetAvailableNetworks(Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_QUERY_AVAILABLE_NETWORKS,
                                    response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    SetCallForward(Int32 action, Int32 cfReason, Int32 serviceClass,
                String number, Int32 timeSeconds, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_CALL_FORWARD, response);

        rr.mParcel->WriteInt(action);
        rr.mParcel->WriteInt(cfReason);
        rr.mParcel->WriteInt(serviceClass);
        rr.mParcel->WriteInt(PhoneNumberUtils->ToaFromString(number));
        rr.mParcel->WriteString(number);
        rr.mParcel.WriteInt (timeSeconds);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " " + action + " " + cfReason + " " + serviceClass
                    + timeSeconds);

        Send(rr);
    }

    //@Override
    CARAPI
    QueryCallForwardStatus(Int32 cfReason, Int32 serviceClass,
                String number, Message response) {
        RILRequest rr
            = RILRequest->Obtain(RIL_REQUEST_QUERY_CALL_FORWARD_STATUS, response);

        rr.mParcel->WriteInt(2); // 2 is for query action, not in used anyway
        rr.mParcel->WriteInt(cfReason);
        rr.mParcel->WriteInt(serviceClass);
        rr.mParcel->WriteInt(PhoneNumberUtils->ToaFromString(number));
        rr.mParcel->WriteString(number);
        rr.mParcel.WriteInt (0);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " " + cfReason + " " + serviceClass);

        Send(rr);
    }

    //@Override
    CARAPI
    QueryCLIP(Message response) {
        RILRequest rr
            = RILRequest->Obtain(RIL_REQUEST_QUERY_CLIP, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }


    //@Override
    CARAPI
    GetBasebandVersion (Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_BASEBAND_VERSION, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    QueryFacilityLock(String facility, String password, Int32 serviceClass,
                            Message response) {
        QueryFacilityLockForApp(facility, password, serviceClass, NULL, response);
    }

    //@Override
    CARAPI
    QueryFacilityLockForApp(String facility, String password, Int32 serviceClass, String appId,
                            Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_QUERY_FACILITY_LOCK, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        // count strings
        rr.mParcel->WriteInt(oldRil ? 3 : 4);

        rr.mParcel->WriteString(facility);
        rr.mParcel->WriteString(password);

        rr.mParcel->WriteString(Integer->ToString(serviceClass));

        If (!oldRil)
            rr.mParcel->WriteString(appId);

        Send(rr);
    }

    //@Override
    CARAPI
    SetFacilityLock (String facility, Boolean lockState, String password,
                        Int32 serviceClass, Message response) {
        SetFacilityLockForApp(facility, lockState, password, serviceClass, NULL, response);
    }

    //@Override
    CARAPI
    SetFacilityLockForApp(String facility, Boolean lockState, String password,
                        Int32 serviceClass, String appId, Message response) {
        String lockString;
         RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_FACILITY_LOCK, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                                                        + " [" + facility + " " + lockState
                                                        + " " + serviceClass + " " + appId + "]");

        Boolean oldRil = NeedsOldRilFeature("facilitylock");

        // count strings
        rr.mParcel->WriteInt(oldRil ? 4 : 5);

        rr.mParcel->WriteString(facility);
        lockString = (lockState)?"1":"0";
        rr.mParcel->WriteString(lockString);
        rr.mParcel->WriteString(password);
        rr.mParcel->WriteString(Integer->ToString(serviceClass));

        If (!oldRil)
            rr.mParcel->WriteString(appId);

        Send(rr);

    }

    //@Override
    CARAPI
    SendUSSD (String ussdString, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SEND_USSD, response);

        If (RILJ_LOGD) {
            String logUssdString = "*******";
            If (RILJ_LOGV) logUssdString = ussdString;
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                                   + " " + logUssdString);
        }

        rr.mParcel->WriteString(ussdString);

        Send(rr);
    }

    // inherited javadoc suffices
    //@Override
    CARAPI CancelPendingUssd (Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_CANCEL_USSD, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString()
                + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }


    //@Override
    CARAPI ResetRadio(Message result) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_RESET_RADIO, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }


    //@Override
    CARAPI SetLocalCallHold(Int32 lchStatus) {
        Byte[] payload = new Byte[]{(Byte)(lchStatus & 0x7F)};
        Rlog->D(RILJ_LOG_TAG, "setLocalCallHold: lchStatus is " + lchStatus);

        SendOemRilRequestRaw(OEMHOOK_EVT_HOOK_SET_LOCAL_CALL_HOLD, 1, payload, NULL);
    }

    //@Override
    CARAPI GetModemCapability(Message response) {
        Rlog->D(RILJ_LOG_TAG, "GetModemCapability");
        SendOemRilRequestRaw(OEMHOOK_EVT_HOOK_GET_MODEM_CAPABILITY, 0, NULL, response);
    }

    //@Override
    CARAPI UpdateStackBinding(Int32 stack, Int32 enable, Message response) {
        Byte[] payload = new Byte[]{(Byte)stack,(Byte)enable};
        Rlog->D(RILJ_LOG_TAG, "UpdateStackBinding: on Stack: " + stack +
                ", enable/disable: " + enable);

        SendOemRilRequestRaw(OEMHOOK_EVT_HOOK_UPDATE_SUB_BINDING, 2, payload, response);
    }


    private void SendOemRilRequestRaw(Int32 requestId, Int32 numPayload, Byte[] payload,
            Message response) {
        Byte[] request = new Byte[mHeaderSize + numPayload * BYTE_SIZE];

        ByteBuffer buf= ByteBuffer->Wrap(request);
        buf->Order(ByteOrder->NativeOrder());

        // Add OEM identifier String
        buf->Put(OEM_IDENTIFIER->GetBytes());
        // Add Request ID
        buf->PutInt(requestId);
        If (numPayload > 0 && payload != NULL) {
            // Add Request payload length
            buf->PutInt(numPayload * BYTE_SIZE);
            For (Byte b : payload) {
                buf->Put(b);
            }
        }

        InvokeOemRilRequestRaw(request, response);
    }

    //@Override
    CARAPI InvokeOemRilRequestRaw(Byte[] data, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_OEM_HOOK_RAW, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
               + "[" + IccUtils->BytesToHexString(data) + "]");

        rr.mParcel->WriteByteArray(data);

        Send(rr);

    }

    //@Override
    CARAPI InvokeOemRilRequestStrings(String[] strings, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_OEM_HOOK_STRINGS, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        rr.mParcel->WriteStringArray(strings);

        Send(rr);
    }

     /**
     * Assign a specified band for RF configuration.
     *
     * @param bandMode one of BM_*_BAND
     * @param response is callback message
     */
    //@Override
    CARAPI SetBandMode (Int32 bandMode, Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_SET_BAND_MODE, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(bandMode);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                 + " " + bandMode);

        Send(rr);
     }

    /**
     * Query the list of band mode supported by RF.
     *
     * @param response is callback message
     *        ((AsyncResult)response.obj).result  is an Int32[] where Int32[0] is
     *        the size of the array and the rest of each element representing
     *        one available BM_*_BAND
     */
    //@Override
    CARAPI QueryAvailableBandMode (Message response) {
        RILRequest rr
                = RILRequest->Obtain(RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE,
                response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendTerminalResponse(String contents, Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        rr.mParcel->WriteString(contents);
        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendEnvelope(String contents, Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        rr.mParcel->WriteString(contents);
        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendEnvelopeWithStatus(String contents, Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + '[' + contents + ']');

        rr.mParcel->WriteString(contents);
        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI HandleCallSetupRequestFromSim(
            Boolean accept, Message response) {

        RILRequest rr = RILRequest->Obtain(
            RILConstants.RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM,
            response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Int32[] param = new Int32[1];
        param[0] = accept ? 1 : 0;
        rr.mParcel->WriteIntArray(param);
        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetPreferredNetworkType(Int32 networkType , Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(networkType);

        mPreferredNetworkType = networkType;

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + networkType);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetPreferredNetworkType(Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetNeighboringCids(Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_GET_NEIGHBORING_CELL_IDS, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetLocationUpdates(Boolean enable, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SET_LOCATION_UPDATES, response);
        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(enable ? 1 : 0);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest) + ": " + enable);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetSmscAddress(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_SMSC_ADDRESS, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetSmscAddress(String address, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SET_SMSC_ADDRESS, result);

        rr.mParcel->WriteString(address);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + address);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI ReportSmsMemoryStatus(Boolean available, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, result);
        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(available ? 1 : 0);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> "
                + RequestToString(rr.mRequest) + ": " + available);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI ReportStkServiceIsRunning(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetGsmBroadcastConfig(Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GSM_GET_BROADCAST_CONFIG, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetGsmBroadcastConfig(SmsBroadcastConfigInfo[] config, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GSM_SET_BROADCAST_CONFIG, response);

        Int32 numOfConfig = config.length;
        rr.mParcel->WriteInt(numOfConfig);

        For(Int32 i = 0; i < numOfConfig; i++) {
            rr.mParcel->WriteInt(config[i].GetFromServiceId());
            rr.mParcel->WriteInt(config[i].GetToServiceId());
            rr.mParcel->WriteInt(config[i].GetFromCodeScheme());
            rr.mParcel->WriteInt(config[i].GetToCodeScheme());
            rr.mParcel->WriteInt(config[i].IsSelected() ? 1 : 0);
        }

        If (RILJ_LOGD) {
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " with " + numOfConfig + " configs : ");
            For (Int32 i = 0; i < numOfConfig; i++) {
                RiljLog(config[i].ToString());
            }
        }

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetGsmBroadcastActivation(Boolean activate, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GSM_BROADCAST_ACTIVATION, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(activate ? 0 : 1);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //***** Private Methods

    // TODO(jeffbrown): Delete me.
    // The RIL should *not* be listening for screen state changes since they are
    // becoming increasingly ambiguous on our devices.  The RIL_REQUEST_SCREEN_STATE
    // message should be deleted and replaced with more precise messages to control
    // behavior such as signal strength reporting or power managements based on
    // more robust signals.
    private void UpdateScreenState() {
        final Int32 oldState = mDefaultDisplayState;
        mDefaultDisplayState = mDefaultDisplay->GetState();
        If (mDefaultDisplayState != oldState) {
            If (oldState != Display.STATE_ON
                    && mDefaultDisplayState == Display.STATE_ON) {
                SendScreenState(TRUE);
            } else If ((oldState == Display.STATE_ON || oldState == Display.STATE_UNKNOWN)
                        && mDefaultDisplayState != Display.STATE_ON) {
                SendScreenState(FALSE);
            }
        }
    }

    protected void SendScreenState(Boolean on) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SCREEN_STATE, NULL);
        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(on ? 1 : 0);

        If (RILJ_LOGD) RiljLog(rr->SerialString()
                + "> " + RequestToString(rr.mRequest) + ": " + on);

        Send(rr);
    }

    //@Override
    protected void
    OnRadioAvailable() {
        // In case screen state was Lost (due to process crash),
        // this ensures that the RIL knows the correct screen state.
        UpdateScreenState();
   }

    protected RadioState GetRadioStateFromInt(Int32 stateInt) {
        RadioState state;

        /* RIL_RadioState ril.h */
        Switch(stateInt) {
            case 0: state = RadioState.RADIO_OFF; break;
            case 1: state = RadioState.RADIO_UNAVAILABLE; break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10: state = RadioState.RADIO_ON; break;

            default:
                throw new RuntimeException(
                            "Unrecognized RIL_RadioState: " + stateInt);
        }
        return state;
    }

    protected void SwitchToRadioState(RadioState newState) {
        SetRadioState(newState);
    }

    /**
     * Holds a PARTIAL_WAKE_LOCK whenever
     * a) There is outstanding RIL request sent to RIL deamon and no replied
     * b) There is a request pending to be sent out.
     *
     * There is a WAKE_LOCK_TIMEOUT to release the lock, though it shouldn't
     * happen often.
     */

    private void
    AcquireWakeLock() {
        Synchronized (mWakeLock) {
            mWakeLock->Acquire();
            mWakeLockCount++;

            mSender->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
            Message msg = mSender->ObtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
            mSender->SendMessageDelayed(msg, mWakeLockTimeout);
        }
    }

    private void
    DecrementWakeLock() {
        Synchronized (mWakeLock) {
            If (mWakeLockCount > 1) {
                mWakeLockCount--;
            } else {
                mWakeLockCount = 0;
                mWakeLock->Release();
                mSender->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
            }
        }
    }

    // TRUE if we had the wakelock
    private Boolean
    ClearWakeLock() {
        Synchronized (mWakeLock) {
            If (mWakeLockCount == 0 && mWakeLock->IsHeld() == FALSE) return FALSE;
            Rlog->D(RILJ_LOG_TAG, "NOTE: mWakeLockCount is " + mWakeLockCount + "at time of clearing");
            mWakeLockCount = 0;
            mWakeLock->Release();
            mSender->RemoveMessages(EVENT_WAKE_LOCK_TIMEOUT);
            return TRUE;
        }
    }

    protected void
    Send(RILRequest rr) {
        Message msg;

        If (mSocket == NULL) {
            rr->OnError(RADIO_NOT_AVAILABLE, NULL);
            rr->Release();
            return;
        }

        msg = mSender->ObtainMessage(EVENT_SEND, rr);

        AcquireWakeLock();

        msg->SendToTarget();
    }

    protected void
    ProcessResponse (Parcel p) {
        Int32 type;

        type = p->ReadInt();

        If (type == RESPONSE_UNSOLICITED) {
            ProcessUnsolicited (p);
        } else If (type == RESPONSE_SOLICITED) {
            RILRequest rr = ProcessSolicited (p);
            If (rr != NULL) {
                rr->Release();
                DecrementWakeLock();
            }
        }
    }

    /**
     * Release each request in mRequestList then clear the list
     * @param error is the RIL_Errno sent back
     * @param loggable TRUE means to print all requests in mRequestList
     */
    protected void ClearRequestList(Int32 error, Boolean loggable) {
        RILRequest rr;
        Synchronized (mRequestList) {
            Int32 count = mRequestList->Size();
            If (RILJ_LOGD && loggable) {
                Rlog->D(RILJ_LOG_TAG, "clearRequestList " +
                        " mWakeLockCount=" + mWakeLockCount +
                        " mRequestList=" + count);
            }

            For (Int32 i = 0; i < count ; i++) {
                rr = mRequestList->ValueAt(i);
                If (RILJ_LOGD && loggable) {
                    Rlog->D(RILJ_LOG_TAG, i + ": [" + rr.mSerial + "] " +
                            RequestToString(rr.mRequest));
                }
                rr->OnError(error, NULL);
                rr->Release();
                DecrementWakeLock();
            }
            mRequestList->Clear();
        }
    }

    protected RILRequest FindAndRemoveRequestFromList(Int32 serial) {
        RILRequest rr = NULL;
        Synchronized (mRequestList) {
            rr = mRequestList->Get(serial);
            If (rr != NULL) {
                mRequestList->Remove(serial);
            }
        }

        return rr;
    }

    protected RILRequest
    ProcessSolicited (Parcel p) {
        Int32 serial, error;
        Boolean found = FALSE;

        serial = p->ReadInt();
        error = p->ReadInt();

        RILRequest rr;

        rr = FindAndRemoveRequestFromList(serial);

        If (rr == NULL) {
            Rlog->W(RILJ_LOG_TAG, "Unexpected solicited response! sn: "
                            + serial + " error: " + error);
            return NULL;
        }

        Object ret = NULL;

        If (error == 0 || p->DataAvail() > 0) {
            // either command succeeds or command fails but with data payload
            try {Switch (rr.mRequest) {
            /*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{([^,]+),[^,]+,([^}]+).+/case \1: ret = \2(p); break;/'
             */
            case RIL_REQUEST_GET_SIM_STATUS: ret =  ResponseIccCardStatus(p); break;
            case RIL_REQUEST_ENTER_SIM_PIN: ret =  ResponseInts(p); break;
            case RIL_REQUEST_ENTER_SIM_PUK: ret =  ResponseInts(p); break;
            case RIL_REQUEST_ENTER_SIM_PIN2: ret =  ResponseInts(p); break;
            case RIL_REQUEST_ENTER_SIM_PUK2: ret =  ResponseInts(p); break;
            case RIL_REQUEST_CHANGE_SIM_PIN: ret =  ResponseInts(p); break;
            case RIL_REQUEST_CHANGE_SIM_PIN2: ret =  ResponseInts(p); break;
            case RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_GET_CURRENT_CALLS: ret =  ResponseCallList(p); break;
            case RIL_REQUEST_DIAL: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GET_IMSI: ret =  ResponseString(p); break;
            case RIL_REQUEST_HANGUP: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: {
                If (mTestingEmergencyCall->GetAndSet(FALSE)) {
                    If (mEmergencyCallbackModeRegistrant != NULL) {
                        RiljLog("testing emergency call, notify ECM Registrants");
                        mEmergencyCallbackModeRegistrant->NotifyRegistrant();
                    }
                }
                ret =  ResponseVoid(p);
                break;
            }
            case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CONFERENCE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_UDUB: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_SIGNAL_STRENGTH: ret =  ResponseSignalStrength(p); break;
            case RIL_REQUEST_VOICE_REGISTRATION_STATE: ret =  ResponseStrings(p); break;
            case RIL_REQUEST_DATA_REGISTRATION_STATE: ret =  ResponseStrings(p); break;
            case RIL_REQUEST_OPERATOR: ret =  ResponseStrings(p); break;
            case RIL_REQUEST_RADIO_POWER: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_DTMF: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SEND_SMS: ret =  ResponseSMS(p); break;
            case RIL_REQUEST_SEND_SMS_EXPECT_MORE: ret =  ResponseSMS(p); break;
            case RIL_REQUEST_SETUP_DATA_CALL: ret =  ResponseSetupDataCall(p); break;
            case RIL_REQUEST_SIM_IO: ret =  ResponseICC_IO(p); break;
            case RIL_REQUEST_SEND_USSD: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CANCEL_USSD: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GET_CLIR: ret =  ResponseInts(p); break;
            case RIL_REQUEST_SET_CLIR: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: ret =  ResponseCallForward(p); break;
            case RIL_REQUEST_SET_CALL_FORWARD: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_CALL_WAITING: ret =  ResponseInts(p); break;
            case RIL_REQUEST_SET_CALL_WAITING: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SMS_ACKNOWLEDGE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GET_IMEI: ret =  ResponseString(p); break;
            case RIL_REQUEST_GET_IMEISV: ret =  ResponseString(p); break;
            case RIL_REQUEST_ANSWER: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_DEACTIVATE_DATA_CALL: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_FACILITY_LOCK: ret =  ResponseInts(p); break;
            case RIL_REQUEST_SET_FACILITY_LOCK: ret =  ResponseInts(p); break;
            case RIL_REQUEST_CHANGE_BARRING_PASSWORD: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : ret =  ResponseOperatorInfos(p); break;
            case RIL_REQUEST_DTMF_START: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_DTMF_STOP: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_BASEBAND_VERSION: ret =  ResponseString(p); break;
            case RIL_REQUEST_SEPARATE_CONNECTION: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SET_MUTE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GET_MUTE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_QUERY_CLIP: ret =  ResponseInts(p); break;
            case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_DATA_CALL_LIST: ret =  ResponseDataCallList(p); break;
            case RIL_REQUEST_RESET_RADIO: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_OEM_HOOK_RAW: ret =  ResponseRaw(p); break;
            case RIL_REQUEST_OEM_HOOK_STRINGS: ret =  ResponseStrings(p); break;
            case RIL_REQUEST_SCREEN_STATE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_WRITE_SMS_TO_SIM: ret =  ResponseInts(p); break;
            case RIL_REQUEST_DELETE_SMS_ON_SIM: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SET_BAND_MODE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_STK_GET_PROFILE: ret =  ResponseString(p); break;
            case RIL_REQUEST_STK_SET_PROFILE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: ret =  ResponseString(p); break;
            case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: ret =  ResponseInts(p); break;
            case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: ret =  ResponseGetPreferredNetworkType(p); break;
            case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: ret = ResponseCellList(p); break;
            case RIL_REQUEST_SET_LOCATION_UPDATES: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_SET_TTY_MODE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_QUERY_TTY_MODE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_CDMA_FLASH: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_BURST_DTMF: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_SEND_SMS: ret =  ResponseSMS(p); break;
            case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GSM_GET_BROADCAST_CONFIG: ret =  ResponseGmsBroadcastConfig(p); break;
            case RIL_REQUEST_GSM_SET_BROADCAST_CONFIG: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_GSM_BROADCAST_ACTIVATION: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG: ret =  ResponseCdmaBroadcastConfig(p); break;
            case RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_BROADCAST_ACTIVATION: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_SUBSCRIPTION: ret =  ResponseStrings(p); break;
            case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: ret =  ResponseInts(p); break;
            case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: ret =  ResponseVoid(p); break;
            case RIL_REQUEST_DEVICE_IDENTITY: ret =  ResponseStrings(p); break;
            case RIL_REQUEST_GET_SMSC_ADDRESS: ret = ResponseString(p); break;
            case RIL_REQUEST_SET_SMSC_ADDRESS: ret = ResponseVoid(p); break;
            case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: ret = ResponseVoid(p); break;
            case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: ret = ResponseVoid(p); break;
            case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: ret = ResponseVoid(p); break;
            case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: ret =  ResponseInts(p); break;
            case RIL_REQUEST_GET_DATA_CALL_PROFILE: ret =  ResponseGetDataCallProfile(p); break;
            case RIL_REQUEST_ISIM_AUTHENTICATION: ret =  ResponseString(p); break;
            case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: ret = ResponseVoid(p); break;
            case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: ret = ResponseICC_IO(p); break;
            case RIL_REQUEST_VOICE_RADIO_TECH: ret = ResponseInts(p); break;
            case RIL_REQUEST_GET_CELL_INFO_LIST: ret = ResponseCellInfoList(p); break;
            case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: ret = ResponseVoid(p); break;
            case RIL_REQUEST_SET_INITIAL_ATTACH_APN: ret = ResponseVoid(p); break;
            case RIL_REQUEST_SET_DATA_PROFILE: ret = ResponseVoid(p); break;
            case RIL_REQUEST_IMS_REGISTRATION_STATE: ret = ResponseInts(p); break;
            case RIL_REQUEST_IMS_SEND_SMS: ret =  ResponseSMS(p); break;
            case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: ret =  ResponseICC_IO(p); break;
            case RIL_REQUEST_SIM_OPEN_CHANNEL: ret  = ResponseInts(p); break;
            case RIL_REQUEST_SIM_CLOSE_CHANNEL: ret  = ResponseVoid(p); break;
            case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: ret = ResponseICC_IO(p); break;
            case RIL_REQUEST_SIM_GET_ATR: ret = ResponseString(p); break;
            case RIL_REQUEST_NV_READ_ITEM: ret = ResponseString(p); break;
            case RIL_REQUEST_NV_WRITE_ITEM: ret = ResponseVoid(p); break;
            case RIL_REQUEST_NV_WRITE_CDMA_PRL: ret = ResponseVoid(p); break;
            case RIL_REQUEST_NV_RESET_CONFIG: ret = ResponseVoid(p); break;
            case RIL_REQUEST_SET_UICC_SUBSCRIPTION: ret = ResponseVoid(p); break;
            case RIL_REQUEST_ALLOW_DATA: ret = ResponseVoid(p); break;
            case RIL_REQUEST_GET_HARDWARE_CONFIG: ret = ResponseHardwareConfig(p); break;
            case RIL_REQUEST_SIM_AUTHENTICATION: ret =  ResponseICC_IOBase64(p); break;
            case RIL_REQUEST_SHUTDOWN: ret = ResponseVoid(p); break;
            default:
                throw new RuntimeException("Unrecognized solicited response: " + rr.mRequest);
            //break;
            }} Catch (Throwable tr) {
                // Exceptions here usually mean invalid RIL responses

                Rlog->W(RILJ_LOG_TAG, rr->SerialString() + "< "
                        + RequestToString(rr.mRequest)
                        + " exception, possible invalid RIL response", tr);

                If (rr.mResult != NULL) {
                    AsyncResult->ForMessage(rr.mResult, NULL, tr);
                    rr.mResult->SendToTarget();
                }
                return rr;
            }
        }

        If (rr.mRequest == RIL_REQUEST_SHUTDOWN) {
            // Set RADIO_STATE to RADIO_UNAVAILABLE to continue shutdown process
            // regardless of error code to continue shutdown procedure.
            RiljLog("Response to RIL_REQUEST_SHUTDOWN received. Error is " +
                    error + " Setting Radio State to Unavailable regardless of error.");
            SetRadioState(RadioState.RADIO_UNAVAILABLE);
        }

        // Here and below fake RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, see b/7255789.
        // This is needed otherwise we don't automatically transition to the main lock
        // screen when the pin or puk is entered incorrectly.
        Switch (rr.mRequest) {
            case RIL_REQUEST_ENTER_SIM_PUK:
            case RIL_REQUEST_ENTER_SIM_PUK2:
                If (mIccStatusChangedRegistrants != NULL) {
                    If (RILJ_LOGD) {
                        RiljLog("ON enter sim puk fakeSimStatusChanged: reg count="
                                + mIccStatusChangedRegistrants->Size());
                    }
                    mIccStatusChangedRegistrants->NotifyRegistrants();
                }
                break;
        }

        If (error != 0) {
            Switch (rr.mRequest) {
                case RIL_REQUEST_ENTER_SIM_PIN:
                case RIL_REQUEST_ENTER_SIM_PIN2:
                case RIL_REQUEST_CHANGE_SIM_PIN:
                case RIL_REQUEST_CHANGE_SIM_PIN2:
                case RIL_REQUEST_SET_FACILITY_LOCK:
                    If (mIccStatusChangedRegistrants != NULL) {
                        If (RILJ_LOGD) {
                            RiljLog("ON some errors fakeSimStatusChanged: reg count="
                                    + mIccStatusChangedRegistrants->Size());
                        }
                        mIccStatusChangedRegistrants->NotifyRegistrants();
                    }
                    break;
            }

            rr->OnError(error, ret);
        } else {

            If (RILJ_LOGD) RiljLog(rr->SerialString() + "< " + RequestToString(rr.mRequest)
                    + " " + RetToString(rr.mRequest, ret));

            If (rr.mResult != NULL) {
                AsyncResult->ForMessage(rr.mResult, ret, NULL);
                rr.mResult->SendToTarget();
            }
        }
        return rr;
    }

    static String
    RetToString(Int32 req, Object ret) {
        If (ret == NULL) return "";
        Switch (req) {
            // Don't log these return values, for privacy's sake.
            case RIL_REQUEST_GET_IMSI:
            case RIL_REQUEST_GET_IMEI:
            case RIL_REQUEST_GET_IMEISV:
            case RIL_REQUEST_SIM_OPEN_CHANNEL:
            case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:

                If (!RILJ_LOGV) {
                    // If not versbose logging just return and don't display IMSI and IMEI, IMEISV
                    return "";
                }
        }

        StringBuilder sb;
        String s;
        Int32 length;
        If (ret instanceof Int32[]){
            Int32[] intArray = (Int32[]) ret;
            length = intArray.length;
            sb = new StringBuilder("{");
            If (length > 0) {
                Int32 i = 0;
                sb->Append(intArray[i++]);
                While ( i < length) {
                    sb->Append(", ").Append(intArray[i++]);
                }
            }
            sb->Append("}");
            s = sb->ToString();
        } else If (ret instanceof String[]) {
            String[] strings = (String[]) ret;
            length = strings.length;
            sb = new StringBuilder("{");
            If (length > 0) {
                Int32 i = 0;
                sb->Append(strings[i++]);
                While ( i < length) {
                    sb->Append(", ").Append(strings[i++]);
                }
            }
            sb->Append("}");
            s = sb->ToString();
        }else If (req == RIL_REQUEST_GET_CURRENT_CALLS) {
            ArrayList<DriverCall> calls = (ArrayList<DriverCall>) ret;
            sb = new StringBuilder(" ");
            For (DriverCall dc : calls) {
                sb->Append("[").Append(dc).Append("] ");
            }
            s = sb->ToString();
        } else If (req == RIL_REQUEST_GET_NEIGHBORING_CELL_IDS) {
            ArrayList<NeighboringCellInfo> cells;
            cells = (ArrayList<NeighboringCellInfo>) ret;
            sb = new StringBuilder(" ");
            For (NeighboringCellInfo cell : cells) {
                sb->Append(cell).Append(" ");
            }
            s = sb->ToString();
        } else If (req == RIL_REQUEST_GET_HARDWARE_CONFIG) {
            ArrayList<HardwareConfig> hwcfgs = (ArrayList<HardwareConfig>) ret;
            sb = new StringBuilder(" ");
            For (HardwareConfig hwcfg : hwcfgs) {
                sb->Append("[").Append(hwcfg).Append("] ");
            }
            s = sb->ToString();
        } else {
            s = ret->ToString();
        }
        return s;
    }

    protected void
    ProcessUnsolicited (Parcel p) {
        Int32 response;
        Object ret;

        response = p->ReadInt();

        try {Switch(response) {
/*
 cat libs/telephony/ril_unsol_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{([^,]+),[^,]+,([^}]+).+/case \1: \2(rr, p); break;/'
*/

            case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: ret =  ResponseVoid(p); break;
            case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: ret =  ResponseVoid(p); break;
            case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: ret =  ResponseVoid(p); break;
            case RIL_UNSOL_RESPONSE_NEW_SMS: ret =  ResponseString(p); break;
            case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: ret =  ResponseString(p); break;
            case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: ret =  ResponseInts(p); break;
            case RIL_UNSOL_ON_USSD: ret =  ResponseStrings(p); break;
            case RIL_UNSOL_NITZ_TIME_RECEIVED: ret =  ResponseString(p); break;
            case RIL_UNSOL_SIGNAL_STRENGTH: ret = ResponseSignalStrength(p); break;
            case RIL_UNSOL_DATA_CALL_LIST_CHANGED: ret = ResponseDataCallList(p);break;
            case RIL_UNSOL_SUPP_SVC_NOTIFICATION: ret = ResponseSuppServiceNotification(p); break;
            case RIL_UNSOL_STK_SESSION_END: ret = ResponseVoid(p); break;
            case RIL_UNSOL_STK_PROACTIVE_COMMAND: ret = ResponseString(p); break;
            case RIL_UNSOL_STK_EVENT_NOTIFY: ret = ResponseString(p); break;
            case RIL_UNSOL_STK_CALL_SETUP: ret = ResponseInts(p); break;
            case RIL_UNSOL_SIM_SMS_STORAGE_FULL: ret =  ResponseVoid(p); break;
            case RIL_UNSOL_SIM_REFRESH: ret =  ResponseSimRefresh(p); break;
            case RIL_UNSOL_CALL_RING: ret =  ResponseCallRing(p); break;
            case RIL_UNSOL_RESTRICTED_STATE_CHANGED: ret = ResponseInts(p); break;
            case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED:  ret =  ResponseVoid(p); break;
            case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS:  ret =  ResponseCdmaSms(p); break;
            case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS:  ret =  ResponseRaw(p); break;
            case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL:  ret =  ResponseVoid(p); break;
            case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: ret = ResponseVoid(p); break;
            case RIL_UNSOL_CDMA_CALL_WAITING: ret = ResponseCdmaCallWaiting(p); break;
            case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: ret = ResponseInts(p); break;
            case RIL_UNSOL_CDMA_INFO_REC: ret = ResponseCdmaInformationRecord(p); break;
            case RIL_UNSOL_OEM_HOOK_RAW: ret = ResponseRaw(p); break;
            case RIL_UNSOL_RINGBACK_TONE: ret = ResponseInts(p); break;
            case RIL_UNSOL_RESEND_INCALL_MUTE: ret = ResponseVoid(p); break;
            case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: ret = ResponseInts(p); break;
            case RIL_UNSOl_CDMA_PRL_CHANGED: ret = ResponseInts(p); break;
            case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: ret = ResponseVoid(p); break;
            case RIL_UNSOL_RIL_CONNECTED: ret = ResponseInts(p); break;
            case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: ret =  ResponseInts(p); break;
            case RIL_UNSOL_CELL_INFO_LIST: ret = ResponseCellInfoList(p); break;
            case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: ret =  ResponseVoid(p); break;
            case RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: ret =  ResponseInts(p); break;
            case RIL_UNSOL_SRVCC_STATE_NOTIFY: ret = ResponseInts(p); break;
            case RIL_UNSOL_HARDWARE_CONFIG_CHANGED: ret = ResponseHardwareConfig(p); break;
            case RIL_UNSOL_ON_SS: ret =  ResponseSsData(p); break;
            case RIL_UNSOL_STK_CC_ALPHA_NOTIFY: ret =  ResponseString(p); break;
            case RIL_UNSOL_STK_SEND_SMS_RESULT: ret = ResponseInts(p); break; // Samsung STK

            default:
                throw new RuntimeException("Unrecognized unsol response: " + response);
            //break; (implied)
        }} Catch (Throwable tr) {
            Rlog->E(RILJ_LOG_TAG, "Exception processing unsol response: " + response +
                "Exception:" + tr->ToString());
            return;
        }

        Switch(response) {
            case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
                /* has bonus radio state Int32 */
                RadioState newState = GetRadioStateFromInt(p->ReadInt());
                If (RILJ_LOGD) UnsljLogMore(response, newState->ToString());

                SwitchToRadioState(newState);
            break;
            case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED:
                If (RILJ_LOGD) UnsljLog(response);

                mImsNetworkStateChangedRegistrants
                    .NotifyRegistrants(new AsyncResult(NULL, NULL, NULL));
            break;
            case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
                If (RILJ_LOGD) UnsljLog(response);

                mCallStateRegistrants
                    .NotifyRegistrants(new AsyncResult(NULL, NULL, NULL));
            break;
            case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED:
                If (RILJ_LOGD) UnsljLog(response);

                mVoiceNetworkStateRegistrants
                    .NotifyRegistrants(new AsyncResult(NULL, NULL, NULL));
            break;
            case RIL_UNSOL_RESPONSE_NEW_SMS: {
                If (RILJ_LOGD) UnsljLog(response);

                // FIXME this should move up a layer
                String a[] = new String[2];

                a[1] = (String)ret;

                SmsMessage sms;

                sms = SmsMessage->NewFromCMT(a);
                If (mGsmSmsRegistrant != NULL) {
                    mGsmSmsRegistrant
                        .NotifyRegistrant(new AsyncResult(NULL, sms, NULL));
                }
            break;
            }
            case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mSmsStatusRegistrant != NULL) {
                    mSmsStatusRegistrant->NotifyRegistrant(
                            new AsyncResult(NULL, ret, NULL));
                }
            break;
            case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                Int32[] smsIndex = (Int32[])ret;

                If(smsIndex.length == 1) {
                    If (mSmsOnSimRegistrant != NULL) {
                        mSmsOnSimRegistrant.
                                NotifyRegistrant(new AsyncResult(NULL, smsIndex, NULL));
                    }
                } else {
                    If (RILJ_LOGD) RiljLog(" NEW_SMS_ON_SIM ERROR with wrong length "
                            + smsIndex.length);
                }
            break;
            case RIL_UNSOL_ON_USSD:
                String[] resp = (String[])ret;

                If (resp.length < 2) {
                    resp = new String[2];
                    resp[0] = ((String[])ret)[0];
                    resp[1] = NULL;
                }
                If (RILJ_LOGD) UnsljLogMore(response, resp[0]);
                If (mUSSDRegistrant != NULL) {
                    mUSSDRegistrant->NotifyRegistrant(
                        new AsyncResult (NULL, resp, NULL));
                }
            break;
            case RIL_UNSOL_NITZ_TIME_RECEIVED:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                // has bonus Int64 containing milliseconds since boot that the NITZ
                // time was received
                Int64 nitzReceiveTime = p->ReadLong();

                Object[] result = new Object[2];

                result[0] = ret;
                result[1] = Long->ValueOf(nitzReceiveTime);

                Boolean ignoreNitz = SystemProperties->GetBoolean(
                        TelephonyProperties.PROPERTY_IGNORE_NITZ, FALSE);

                If (ignoreNitz) {
                    If (RILJ_LOGD) RiljLog("ignoring UNSOL_NITZ_TIME_RECEIVED");
                } else {
                    If (mNITZTimeRegistrant != NULL) {

                        mNITZTimeRegistrant
                            .NotifyRegistrant(new AsyncResult (NULL, result, NULL));
                    } else {
                        // in case NITZ time registrant isnt registered yet
                        mLastNITZTimeInfo = result;
                    }
                }
            break;

            case RIL_UNSOL_SIGNAL_STRENGTH:
                // Note this is set to "verbose" because it happens
                // frequently
                If (RILJ_LOGV) UnsljLogvRet(response, ret);

                If (mSignalStrengthRegistrant != NULL) {
                    mSignalStrengthRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
            break;
            case RIL_UNSOL_DATA_CALL_LIST_CHANGED:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                Boolean oldRil = NeedsOldRilFeature("skipbrokendatacall");
                If (oldRil && "IP".Equals(((ArrayList<DataCallResponse>)ret).Get(0).type))
                    break;

                mDataNetworkStateRegistrants->NotifyRegistrants(new AsyncResult(NULL, ret, NULL));
            break;

            case RIL_UNSOL_SUPP_SVC_NOTIFICATION:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mSsnRegistrant != NULL) {
                    mSsnRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_STK_SESSION_END:
                If (RILJ_LOGD) UnsljLog(response);

                If (mCatSessionEndRegistrant != NULL) {
                    mCatSessionEndRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_STK_PROACTIVE_COMMAND:
                If (RILJ_LOGD) UnsljLog(response);

                If (mCatProCmdRegistrant != NULL) {
                    mCatProCmdRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_STK_EVENT_NOTIFY:
                If (RILJ_LOGD) UnsljLog(response);

                If (mCatEventRegistrant != NULL) {
                    mCatEventRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_STK_CALL_SETUP:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mCatCallSetUpRegistrant != NULL) {
                    mCatCallSetUpRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_SIM_SMS_STORAGE_FULL:
                If (RILJ_LOGD) UnsljLog(response);

                If (mIccSmsFullRegistrant != NULL) {
                    mIccSmsFullRegistrant->NotifyRegistrant();
                }
                break;

            case RIL_UNSOL_SIM_REFRESH:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mIccRefreshRegistrants != NULL) {
                    mIccRefreshRegistrants->NotifyRegistrants(
                            new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_CALL_RING:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mRingRegistrant != NULL) {
                    mRingRegistrant->NotifyRegistrant(
                            new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_RESTRICTED_STATE_CHANGED:
                If (RILJ_LOGD) UnsljLogvRet(response, ret);
                If (mRestrictedStateRegistrant != NULL) {
                    mRestrictedStateRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED:
                If (RILJ_LOGD) UnsljLog(response);

                If (mIccStatusChangedRegistrants != NULL) {
                    mIccStatusChangedRegistrants->NotifyRegistrants();
                }
                break;

            case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS:
                If (RILJ_LOGD) UnsljLog(response);

                SmsMessage sms = (SmsMessage) ret;

                If (mCdmaSmsRegistrant != NULL) {
                    mCdmaSmsRegistrant
                        .NotifyRegistrant(new AsyncResult(NULL, sms, NULL));
                }
                break;

            case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS:
                If (RILJ_LOGD) UnsljLog(response);

                If (mGsmBroadcastSmsRegistrant != NULL) {
                    mGsmBroadcastSmsRegistrant
                        .NotifyRegistrant(new AsyncResult(NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL:
                If (RILJ_LOGD) UnsljLog(response);

                If (mIccSmsFullRegistrant != NULL) {
                    mIccSmsFullRegistrant->NotifyRegistrant();
                }
                break;

            case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE:
                If (RILJ_LOGD) UnsljLog(response);

                If (mEmergencyCallbackModeRegistrant != NULL) {
                    mEmergencyCallbackModeRegistrant->NotifyRegistrant();
                }
                break;

            case RIL_UNSOL_CDMA_CALL_WAITING:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mCallWaitingInfoRegistrants != NULL) {
                    mCallWaitingInfoRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mOtaProvisionRegistrants != NULL) {
                    mOtaProvisionRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_CDMA_INFO_REC:
                ArrayList<CdmaInformationRecords> listInfoRecs;

                try {
                    listInfoRecs = (ArrayList<CdmaInformationRecords>)ret;
                } Catch (ClassCastException e) {
                    Rlog->E(RILJ_LOG_TAG, "Unexpected exception casting to listInfoRecs", e);
                    break;
                }

                For (CdmaInformationRecords rec : listInfoRecs) {
                    If (RILJ_LOGD) UnsljLogRet(response, rec);
                    NotifyRegistrantsCdmaInfoRec(rec);
                }
                break;

            case RIL_UNSOL_OEM_HOOK_RAW:
                If (RILJ_LOGD) UnsljLogvRet(response, IccUtils->BytesToHexString((Byte[]) ret));
                ByteBuffer oemHookResponse = ByteBuffer->Wrap((Byte[]) ret);
                oemHookResponse->Order(ByteOrder->NativeOrder());
                If (IsQcUnsolOemHookResp(oemHookResponse)) {
                    Rlog->D(RILJ_LOG_TAG, "OEM ID check Passed");
                    ProcessUnsolOemhookResponse(oemHookResponse);
                } else If (mUnsolOemHookRawRegistrant != NULL) {
                    Rlog->D(RILJ_LOG_TAG, "External OEM message, to be notified");
                    mUnsolOemHookRawRegistrant->NotifyRegistrant(new AsyncResult(NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_RINGBACK_TONE:
                If (RILJ_LOGD) UnsljLogvRet(response, ret);
                If (mRingbackToneRegistrants != NULL) {
                    Boolean playtone = (((Int32[])ret)[0] == 1);
                    mRingbackToneRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, playtone, NULL));
                }
                break;

            case RIL_UNSOL_RESEND_INCALL_MUTE:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mResendIncallMuteRegistrants != NULL) {
                    mResendIncallMuteRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mVoiceRadioTechChangedRegistrants != NULL) {
                    mVoiceRadioTechChangedRegistrants->NotifyRegistrants(
                            new AsyncResult(NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mCdmaSubscriptionChangedRegistrants != NULL) {
                    mCdmaSubscriptionChangedRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOl_CDMA_PRL_CHANGED:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mCdmaPrlChangedRegistrants != NULL) {
                    mCdmaPrlChangedRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;

            case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mExitEmergencyCallbackModeRegistrants != NULL) {
                    mExitEmergencyCallbackModeRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, NULL, NULL));
                }
                break;

            case RIL_UNSOL_RIL_CONNECTED: {
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                // Initial conditions
                SetRadioPower(FALSE, NULL);
                SetPreferredNetworkType(mPreferredNetworkType, NULL);
                SetCdmaSubscriptionSource(mCdmaSubscription, NULL);
                SetCellInfoListRate(Integer.MAX_VALUE, NULL);
                NotifyRegistrantsRilConnectionChanged(((Int32[])ret)[0]);
                break;
            }
            case RIL_UNSOL_CELL_INFO_LIST: {
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mRilCellInfoListRegistrants != NULL) {
                    mRilCellInfoListRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;
            }
            case RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: {
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mSubscriptionStatusRegistrants != NULL) {
                    mSubscriptionStatusRegistrants->NotifyRegistrants(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;
            }
            case RIL_UNSOL_SRVCC_STATE_NOTIFY: {
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mSrvccStateRegistrants != NULL) {
                    mSrvccStateRegistrants
                            .NotifyRegistrants(new AsyncResult(NULL, ret, NULL));
                }
                break;
            }
            case RIL_UNSOL_HARDWARE_CONFIG_CHANGED:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mHardwareConfigChangeRegistrants != NULL) {
                    mHardwareConfigChangeRegistrants->NotifyRegistrants(
                                             new AsyncResult (NULL, ret, NULL));
                }
                break;
            case RIL_UNSOL_ON_SS:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mSsRegistrant != NULL) {
                    mSsRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;
            case RIL_UNSOL_STK_CC_ALPHA_NOTIFY:
                If (RILJ_LOGD) UnsljLogRet(response, ret);

                If (mCatCcAlphaRegistrant != NULL) {
                    mCatCcAlphaRegistrant->NotifyRegistrant(
                                        new AsyncResult (NULL, ret, NULL));
                }
                break;
            // Samsung STK
            case RIL_UNSOL_STK_SEND_SMS_RESULT:
                If (Resources->GetSystem().
                        GetBoolean(R.bool.config_samsung_stk)) {
                    If (RILJ_LOGD) UnsljLogRet(response, ret);

                    If (mCatSendSmsResultRegistrant != NULL) {
                        mCatSendSmsResultRegistrant->NotifyRegistrant(
                                new AsyncResult (NULL, ret, NULL));
                    }
                }
                break;
        }
    }

    /**
     * Notifiy all registrants that the ril has connected or disconnected.
     *
     * @param rilVer is the version of the ril or -1 if disconnected.
     */
    protected void NotifyRegistrantsRilConnectionChanged(Int32 rilVer) {
        mRilVersion = rilVer;
        If (mRilConnectedRegistrants != NULL) {
            mRilConnectedRegistrants->NotifyRegistrants(
                                new AsyncResult (NULL, new Integer(rilVer), NULL));
        }
    }

    private Boolean IsQcUnsolOemHookResp(ByteBuffer oemHookResponse) {

        /* Check OEM ID in UnsolOemHook response */
        If (oemHookResponse->Capacity() < mHeaderSize) {
            /*
             * size of UnsolOemHook message is less than expected, considered as
             * External OEM's message
             */
            Rlog->D(RILJ_LOG_TAG,
                    "RIL_UNSOL_OEM_HOOK_RAW data size is " + oemHookResponse->Capacity());
            return FALSE;
        } else {
            Byte[] oemIdBytes = new Byte[OEM_IDENTIFIER->Length()];
            oemHookResponse->Get(oemIdBytes);
            String oemIdString = new String(oemIdBytes);
            Rlog->D(RILJ_LOG_TAG, "Oem ID in RIL_UNSOL_OEM_HOOK_RAW is " + oemIdString);
            If (!oemIdString->Equals(OEM_IDENTIFIER)) {
                /* OEM ID not matched, considered as External OEM's message */
                return FALSE;
            }
        }
        return TRUE;
    }

    final public class UnsolOemHookBuffer {
        private Int32 mRilInstance;
        private Byte[] mData;

        public UnsolOemHookBuffer(Int32 rilInstance, Byte[] data) {
            mRilInstance = rilInstance;
            mData = data;
        }

        public Int32 GetRilInstance() {
            return mRilInstance;
        }

        public Byte[] GetUnsolOemHookBuffer() {
            return mData;
        }
    }

    private void ProcessUnsolOemhookResponse(ByteBuffer oemHookResponse) {
        Int32 responseId = 0, responseSize = 0, responseVoiceId = 0;

        responseId = oemHookResponse->GetInt();
        Rlog->D(RILJ_LOG_TAG, "Response ID in RIL_UNSOL_OEM_HOOK_RAW is " + responseId);

        responseSize = oemHookResponse->GetInt();
        If (responseSize < 0) {
            Rlog->E(RILJ_LOG_TAG, "Response Size is Invalid " + responseSize);
            return;
        }

        Byte[] responseData = new Byte[responseSize];
        If (oemHookResponse->Remaining() == responseSize) {
            oemHookResponse->Get(responseData, 0, responseSize);
        } else {
            Rlog->E(RILJ_LOG_TAG, "Response Size(" + responseSize
                    + ") doesnot match remaining Bytes(" +
                    oemHookResponse->Remaining() + ") in the buffer. So, don't process further");
            return;
        }

        Switch (responseId) {
            case OEMHOOK_UNSOL_WWAN_IWLAN_COEXIST:
                NotifyWwanIwlanCoexist(responseData);
                break;

            case OEMHOOK_UNSOL_SIM_REFRESH:
                NotifySimRefresh(responseData);
                break;

            case QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY:
                Rlog->D(RILJ_LOG_TAG, "QCRIL_EVT_HOOK_UNSOL_MODEM_CAPABILITY = mInstanceId"
                        + mInstanceId);
                NotifyModemCap(responseData, mInstanceId);
                break;

            default:
                Rlog->D(RILJ_LOG_TAG, "Response ID " + responseId
                        + " is not served in this process.");
                break;
        }
    }

    /** Notify registrants of WWAN coexistence event. */
    protected void NotifyWwanIwlanCoexist(Byte[] data) {
        AsyncResult ar = new AsyncResult(NULL, data, NULL);
        mWwanIwlanCoexistenceRegistrants->NotifyRegistrants(ar);
        Rlog->D(RILJ_LOG_TAG, "WWAN, IWLAN coexistence notified to registrants");
    }

    /** Notify registrants of SIM_REFRESH event. */
    protected void NotifySimRefresh(Byte[] data) {
        Int32 len = data.length;
        Byte[] userdata = new Byte[1 + len];
        System->Arraycopy(data, 0, userdata, 0, len);
        //Add slot id in SIM_REFRESH event to notify framework: IccRecords.
        userdata[len] = (mInstanceId == NULL) ? 0 : (Byte)(mInstanceId & 0xFF);

        AsyncResult ar = new AsyncResult(NULL, userdata, NULL);
        mSimRefreshRegistrants->NotifyRegistrants(ar);
        Rlog->D(RILJ_LOG_TAG, "SIM_REFRESH notified to registrants");
    }

    /** Notify registrants of MODEM_CAPABILITY event. */
    protected void NotifyModemCap(Byte[] data, Integer phoneId) {
        UnsolOemHookBuffer buffer = new UnsolOemHookBuffer(phoneId, data);

        //Had notifyRegistrants not discarded userObj, we could have easily
        //passed the subId as ar.userObj.
        AsyncResult ar = new AsyncResult(NULL, buffer, NULL);

        mModemCapRegistrants->NotifyRegistrants(ar);
        Rlog->D(RILJ_LOG_TAG, "MODEM_CAPABILITY on phone=" + phoneId + " notified to registrants");
    }

    protected Object
    ResponseInts(Parcel p) {
        Int32 numInts;
        Int32 response[];

        numInts = p->ReadInt();

        response = new Int32[numInts];

        For (Int32 i = 0 ; i < numInts ; i++) {
            response[i] = p->ReadInt();
        }

        return response;
    }


    protected Object
    ResponseVoid(Parcel p) {
        return NULL;
    }

    protected Object
    ResponseCallForward(Parcel p) {
        Int32 numInfos;
        CallForwardInfo infos[];

        numInfos = p->ReadInt();

        infos = new CallForwardInfo[numInfos];

        For (Int32 i = 0 ; i < numInfos ; i++) {
            infos[i] = new CallForwardInfo();

            infos[i].status = p->ReadInt();
            infos[i].reason = p->ReadInt();
            infos[i].serviceClass = p->ReadInt();
            infos[i].toa = p->ReadInt();
            infos[i].number = p->ReadString();
            infos[i].timeSeconds = p->ReadInt();
        }

        return infos;
    }

    protected Object
    ResponseSuppServiceNotification(Parcel p) {
        SuppServiceNotification notification = new SuppServiceNotification();

        notification.notificationType = p->ReadInt();
        notification.code = p->ReadInt();
        notification.index = p->ReadInt();
        notification.type = p->ReadInt();
        notification.number = p->ReadString();

        return notification;
    }

    protected Object
    ResponseCdmaSms(Parcel p) {
        SmsMessage sms;
        sms = SmsMessage->NewFromParcel(p);

        return sms;
    }

    protected Object
    ResponseString(Parcel p) {
        String response;

        response = p->ReadString();

        return response;
    }

    protected Object
    ResponseStrings(Parcel p) {
        Int32 num;
        String response[];

        response = p->ReadStringArray();

        return response;
    }

    protected Object
    ResponseRaw(Parcel p) {
        Int32 num;
        Byte response[];

        response = p->CreateByteArray();

        return response;
    }

    protected Object
    ResponseSMS(Parcel p) {
        Int32 messageRef, errorCode;
        String ackPDU;

        messageRef = p->ReadInt();
        ackPDU = p->ReadString();
        errorCode = p->ReadInt();

        SmsResponse response = new SmsResponse(messageRef, ackPDU, errorCode);

        return response;
    }


    protected Object
    ResponseICC_IO(Parcel p) {
        Int32 sw1, sw2;
        Message ret;

        sw1 = p->ReadInt();
        sw2 = p->ReadInt();

        String s = p->ReadString();

        If (RILJ_LOGV) RiljLog("< iccIO: "
                + " 0x" + Integer->ToHexString(sw1)
                + " 0x" + Integer->ToHexString(sw2) + " "
                + s);

        return new IccIoResult(sw1, sw2, s);
    }

    private Object
    ResponseICC_IOBase64(Parcel p) {
        Int32 sw1, sw2;
        Message ret;

        sw1 = p->ReadInt();
        sw2 = p->ReadInt();

        String s = p->ReadString();

        If (RILJ_LOGV) RiljLog("< iccIO: "
                + " 0x" + Integer->ToHexString(sw1)
                + " 0x" + Integer->ToHexString(sw2) + " "
                + s);


        return new IccIoResult(sw1, sw2, android.util.Base64->Decode(s, android.util.Base64.DEFAULT));
    }

    //@Override
    public Boolean NeedsOldRilFeature(String feature) {
        String[] features = SystemProperties->Get("ro.telephony.ril.config", "").Split(",");
        For (String found: features) {
            If (found->Equals(feature))
                return TRUE;
        }
        return FALSE;
    }

    protected Object
    ResponseIccCardStatus(Parcel p) {
        IccCardApplicationStatus appStatus;

        Boolean oldRil = NeedsOldRilFeature("icccardstatus");

        IccCardStatus cardStatus = new IccCardStatus();
        cardStatus->SetCardState(p->ReadInt());
        cardStatus->SetUniversalPinState(p->ReadInt());
        cardStatus.mGsmUmtsSubscriptionAppIndex = p->ReadInt();
        cardStatus.mCdmaSubscriptionAppIndex = p->ReadInt();

        If (!oldRil)
            cardStatus.mImsSubscriptionAppIndex = p->ReadInt();

        Int32 numApplications = p->ReadInt();

        // limit to maximum allowed applications
        If (numApplications > IccCardStatus.CARD_MAX_APPS) {
            numApplications = IccCardStatus.CARD_MAX_APPS;
        }
        cardStatus.mApplications = new IccCardApplicationStatus[numApplications];

        For (Int32 i = 0 ; i < numApplications ; i++) {
            appStatus = new IccCardApplicationStatus();
            appStatus.app_type       = appStatus->AppTypeFromRILInt(p->ReadInt());
            appStatus.app_state      = appStatus->AppStateFromRILInt(p->ReadInt());
            appStatus.perso_substate = appStatus->PersoSubstateFromRILInt(p->ReadInt());
            appStatus.aid            = p->ReadString();
            appStatus.app_label      = p->ReadString();
            appStatus.pin1_replaced  = p->ReadInt();
            appStatus.pin1           = appStatus->PinStateFromRILInt(p->ReadInt());
            appStatus.pin2           = appStatus->PinStateFromRILInt(p->ReadInt());
            cardStatus.mApplications[i] = appStatus;
        }
        return cardStatus;
    }

    protected Object
    ResponseSimRefresh(Parcel p) {
        IccRefreshResponse response = new IccRefreshResponse();

        response.refreshResult = p->ReadInt();
        response.efId   = p->ReadInt();
        response.aid = p->ReadString();
        return response;
    }

    protected Object
    ResponseCallList(Parcel p) {
        Int32 num;
        Int32 voiceSettings;
        ArrayList<DriverCall> response;
        DriverCall dc;

        num = p->ReadInt();
        response = new ArrayList<DriverCall>(num);

        If (RILJ_LOGV) {
            RiljLog("responseCallList: num=" + num +
                    " mEmergencyCallbackModeRegistrant=" + mEmergencyCallbackModeRegistrant +
                    " mTestingEmergencyCall=" + mTestingEmergencyCall->Get());
        }
        For (Int32 i = 0 ; i < num ; i++) {
            dc = new DriverCall();

            dc.state = DriverCall->StateFromCLCC(p->ReadInt());
            dc.index = p->ReadInt();
            dc.TOA = p->ReadInt();
            dc.isMpty = (0 != p->ReadInt());
            dc.isMT = (0 != p->ReadInt());
            dc.als = p->ReadInt();
            voiceSettings = p->ReadInt();
            dc.isVoice = (0 == voiceSettings) ? FALSE : TRUE;
            dc.isVoicePrivacy = (0 != p->ReadInt());
            dc.number = p->ReadString();
            Int32 np = p->ReadInt();
            dc.numberPresentation = DriverCall->PresentationFromCLIP(np);
            dc.name = p->ReadString();
            // according to ril.h, namePresentation should be handled as numberPresentation;
            dc.namePresentation = DriverCall->PresentationFromCLIP(p->ReadInt());
            Int32 uusInfoPresent = p->ReadInt();
            If (uusInfoPresent == 1) {
                dc.uusInfo = new UUSInfo();
                dc.uusInfo->SetType(p->ReadInt());
                dc.uusInfo->SetDcs(p->ReadInt());
                Byte[] userData = p->CreateByteArray();
                dc.uusInfo->SetUserData(userData);
                RiljLogv(String->Format("Incoming UUS : type=%d, dcs=%d, length=%d",
                                dc.uusInfo->GetType(), dc.uusInfo->GetDcs(),
                                dc.uusInfo->GetUserData().length));
                RiljLogv("Incoming UUS : Data (string)="
                        + new String(dc.uusInfo->GetUserData()));
                RiljLogv("Incoming UUS : Data (hex): "
                        + IccUtils->BytesToHexString(dc.uusInfo->GetUserData()));
            } else {
                RiljLogv("Incoming UUS : NOT present!");
            }

            // Make sure there's a leading + on addresses with a TOA of 145
            dc.number = PhoneNumberUtils->StringFromStringAndTOA(dc.number, dc.TOA);

            response->Add(dc);

            If (dc.isVoicePrivacy) {
                mVoicePrivacyOnRegistrants->NotifyRegistrants();
                RiljLog("InCall VoicePrivacy is enabled");
            } else {
                mVoicePrivacyOffRegistrants->NotifyRegistrants();
                RiljLog("InCall VoicePrivacy is disabled");
            }
        }

        Collections->Sort(response);

        If ((num == 0) && mTestingEmergencyCall->GetAndSet(FALSE)) {
            If (mEmergencyCallbackModeRegistrant != NULL) {
                RiljLog("responseCallList: call ended, testing emergency call," +
                            " notify ECM Registrants");
                mEmergencyCallbackModeRegistrant->NotifyRegistrant();
            }
        }

        return response;
    }

    protected DataCallResponse GetDataCallResponse(Parcel p, Int32 version) {
        DataCallResponse dataCall = new DataCallResponse();

        dataCall.version = version;
        If (version < 5) {
            dataCall.cid = p->ReadInt();
            dataCall.active = p->ReadInt();
            dataCall.type = p->ReadString();
            If (version < 4 || NeedsOldRilFeature("datacallapn")) {
                p->ReadString(); // APN - not used
            }
            String addresses = p->ReadString();
            If (!TextUtils->IsEmpty(addresses)) {
                dataCall.addresses = addresses->Split(" ");
            }
            // DataCallState needs an ifname. Since we don't have one use the name from the ThrottleService Resource (default=rmnet0).
            dataCall.ifname = Resources->GetSystem()->GetString(R::string::config_datause_iface);
        } else {
            dataCall.status = p->ReadInt();
            If (NeedsOldRilFeature("usehcradio"))
                dataCall.suggestedRetryTime = -1;
            else
	      dataCall.suggestedRetryTime = p->ReadInt();
            dataCall.cid = p->ReadInt();
            dataCall.active = p->ReadInt();
            dataCall.type = p->ReadString();
            dataCall.ifname = p->ReadString();
            If ((dataCall.status == DcFailCause.NONE->GetErrorCode()) &&
                    TextUtils->IsEmpty(dataCall.ifname)) {
              throw new RuntimeException("getDataCallResponse, no ifname");
            }
            String addresses = p->ReadString();
            If (!TextUtils->IsEmpty(addresses)) {
                dataCall.addresses = addresses->Split(" ");
            }
            String dnses = p->ReadString();
            If (!TextUtils->IsEmpty(dnses)) {
                dataCall.dnses = dnses->Split(" ");
            }
            String gateways = p->ReadString();
            If (!TextUtils->IsEmpty(gateways)) {
                dataCall.gateways = gateways->Split(" ");
            }
            If (version >= 10) {
                String pcscf = p->ReadString();
                If (!TextUtils->IsEmpty(pcscf)) {
                    dataCall.pcscf = pcscf->Split(" ");
                }
                If (version >= 11) {
                    dataCall.mtu = p->ReadInt();
                }
            }
        }
        return dataCall;
    }

    protected Object
    ResponseDataCallList(Parcel p) {
        ArrayList<DataCallResponse> response;
        Boolean oldRil = NeedsOldRilFeature("datacall");
        Int32 ver = (oldRil ? 3 : p->ReadInt());
        Int32 num = p->ReadInt();
        RiljLog("responseDataCallList ver=" + ver + " num=" + num);

        response = new ArrayList<DataCallResponse>(num);
        For (Int32 i = 0; i < num; i++) {
            response->Add(GetDataCallResponse(p, ver));
        }

        return response;
    }

    protected Object
    ResponseSetupDataCall(Parcel p) {
        Boolean oldRil = NeedsOldRilFeature("datacall");
        Int32 ver = (oldRil ? 3 : p->ReadInt());
        Int32 num = p->ReadInt();
        If (RILJ_LOGV) RiljLog("responseSetupDataCall ver=" + ver + " num=" + num);

        DataCallResponse dataCall;

        If (ver < 5) {
            dataCall = new DataCallResponse();
            dataCall.version = ver;
            dataCall.cid = Integer->ParseInt(p->ReadString());
            dataCall.ifname = p->ReadString();
            If (TextUtils->IsEmpty(dataCall.ifname)) {
                throw new RuntimeException(
                        "RIL_REQUEST_SETUP_DATA_CALL response, no ifname");
            }
            String addresses = p->ReadString();
            If (!TextUtils->IsEmpty(addresses)) {
              dataCall.addresses = addresses->Split(" ");
            }
            If (num >= 4) {
                String dnses = p->ReadString();
                If (RILJ_LOGD) RiljLog("responseSetupDataCall got dnses=" + dnses);
                If (!TextUtils->IsEmpty(dnses)) {
                    dataCall.dnses = dnses->Split(" ");
                }
            }
            If (num >= 5) {
                String gateways = p->ReadString();
                If (RILJ_LOGD) RiljLog("responseSetupDataCall got gateways=" + gateways);
                If (!TextUtils->IsEmpty(gateways)) {
                    dataCall.gateways = gateways->Split(" ");
                }
            }
            If (num >= 6) {
                String pcscf = p->ReadString();
                If (RILJ_LOGD) RiljLog("responseSetupDataCall got pcscf=" + pcscf);
                If (!TextUtils->IsEmpty(pcscf)) {
                    dataCall.pcscf = pcscf->Split(" ");
                }
            }
            If (num >= 7) {
                dataCall.mtu = Integer->ParseInt(p->ReadString());
                If (RILJ_LOGD) RiljLog("responseSetupDataCall got mtu=" + dataCall.mtu);
            }
        } else {
            If (num != 1) {
                throw new RuntimeException(
                        "RIL_REQUEST_SETUP_DATA_CALL response expecting 1 RIL_Data_Call_response_v5"
                        + " got " + num);
            }
            dataCall = GetDataCallResponse(p, ver);
        }

        return dataCall;
    }

    protected Object
    ResponseOperatorInfos(Parcel p) {
        String strings[] = (String [])ResponseStrings(p);
        ArrayList<OperatorInfo> ret;

        If (strings.length % mQANElements != 0) {
            throw new RuntimeException(
                "RIL_REQUEST_QUERY_AVAILABLE_NETWORKS: invalid response. Got "
                + strings.length + " strings, expected multiple of " + mQANElements);
        }

        ret = new ArrayList<OperatorInfo>(strings.length / mQANElements);

        For (Int32 i = 0 ; i < strings.length ; i += mQANElements) {
            ret.Add (
                new OperatorInfo(
                    strings[i+0],
                    strings[i+1],
                    strings[i+2],
                    strings[i+3]));
        }

        return ret;
    }

    protected Object
    ResponseCellList(Parcel p) {
       Int32 num, rssi;
       String location;
       ArrayList<NeighboringCellInfo> response;
       NeighboringCellInfo cell;

       num = p->ReadInt();
       response = new ArrayList<NeighboringCellInfo>();

       // Determine the radio access type
       String radioString = SystemProperties->Get(
               TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE, "unknown");
       Int32 radioType;
       If (radioString->Equals("GPRS")) {
           radioType = NETWORK_TYPE_GPRS;
       } else If (radioString->Equals("EDGE")) {
           radioType = NETWORK_TYPE_EDGE;
       } else If (radioString->Equals("UMTS")) {
           radioType = NETWORK_TYPE_UMTS;
       } else If (radioString->Equals("HSDPA")) {
           radioType = NETWORK_TYPE_HSDPA;
       } else If (radioString->Equals("HSUPA")) {
           radioType = NETWORK_TYPE_HSUPA;
       } else If (radioString->Equals("HSPA")) {
           radioType = NETWORK_TYPE_HSPA;
       } else {
           radioType = NETWORK_TYPE_UNKNOWN;
       }

       // Interpret the location based on radio access type
       If (radioType != NETWORK_TYPE_UNKNOWN) {
           For (Int32 i = 0 ; i < num ; i++) {
               rssi = p->ReadInt();
               location = p->ReadString();
               cell = new NeighboringCellInfo(rssi, location, radioType);
               response->Add(cell);
           }
       }
       return response;
    }

    protected Object ResponseGetPreferredNetworkType(Parcel p) {
       Int32 [] response = (Int32[]) ResponseInts(p);

       If (response.length >= 1) {
           // Since this is the response for getPreferredNetworkType
           // we'll assume that it should be the value we want the
           // vendor ril to take if we reestablish a connection to it.
           mPreferredNetworkType = response[0];
       }
       return response;
    }

    protected Object ResponseGmsBroadcastConfig(Parcel p) {
        Int32 num;
        ArrayList<SmsBroadcastConfigInfo> response;
        SmsBroadcastConfigInfo info;

        num = p->ReadInt();
        response = new ArrayList<SmsBroadcastConfigInfo>(num);

        For (Int32 i = 0; i < num; i++) {
            Int32 fromId = p->ReadInt();
            Int32 toId = p->ReadInt();
            Int32 fromScheme = p->ReadInt();
            Int32 toScheme = p->ReadInt();
            Boolean selected = (p->ReadInt() == 1);

            info = new SmsBroadcastConfigInfo(fromId, toId, fromScheme,
                    toScheme, selected);
            response->Add(info);
        }
        return response;
    }

    protected Object
    ResponseCdmaBroadcastConfig(Parcel p) {
        Int32 numServiceCategories;
        Int32 response[];

        numServiceCategories = p->ReadInt();

        If (numServiceCategories == 0) {
            // TODO: The logic of providing default values should
            // not be done by this transport layer. And needs to
            // be done by the vendor ril or application logic.
            Int32 numInts;
            numInts = CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES * CDMA_BSI_NO_OF_INTS_STRUCT + 1;
            response = new Int32[numInts];

            // Faking a default record for all possible records.
            response[0] = CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES;

            // Loop over CDMA_BROADCAST_SMS_NO_OF_SERVICE_CATEGORIES set 'english' as
            // default language and selection status to FALSE for all.
            For (Int32 i = 1; i < numInts; i += CDMA_BSI_NO_OF_INTS_STRUCT ) {
                response[i + 0] = i / CDMA_BSI_NO_OF_INTS_STRUCT;
                response[i + 1] = 1;
                response[i + 2] = 0;
            }
        } else {
            Int32 numInts;
            numInts = (numServiceCategories * CDMA_BSI_NO_OF_INTS_STRUCT) + 1;
            response = new Int32[numInts];

            response[0] = numServiceCategories;
            For (Int32 i = 1 ; i < numInts; i++) {
                 response[i] = p->ReadInt();
             }
        }

        return response;
    }

    protected Object
    ResponseSignalStrength(Parcel p) {
        // Assume this is gsm, but doesn't matter as ServiceStateTracker
        // sets the proper value.
        SignalStrength signalStrength = SignalStrength->MakeSignalStrengthFromRilParcel(p);
        return signalStrength;
    }

    protected ArrayList<CdmaInformationRecords>
    ResponseCdmaInformationRecord(Parcel p) {
        Int32 numberOfInfoRecs;
        ArrayList<CdmaInformationRecords> response;

        /**
         * Loop through all of the information records unmarshalling them
         * and converting them to Java Objects.
         */
        numberOfInfoRecs = p->ReadInt();
        response = new ArrayList<CdmaInformationRecords>(numberOfInfoRecs);

        For (Int32 i = 0; i < numberOfInfoRecs; i++) {
            CdmaInformationRecords InfoRec = new CdmaInformationRecords(p);
            response->Add(InfoRec);
        }

        return response;
    }

    protected Object
    ResponseCdmaCallWaiting(Parcel p) {
        CdmaCallWaitingNotification notification = new CdmaCallWaitingNotification();

        notification.number = p->ReadString();
        notification.numberPresentation =
                CdmaCallWaitingNotification->PresentationFromCLIP(p->ReadInt());
        notification.name = p->ReadString();
        notification.namePresentation = notification.numberPresentation;
        notification.isPresent = p->ReadInt();
        notification.signalType = p->ReadInt();
        notification.alertPitch = p->ReadInt();
        notification.signal = p->ReadInt();
        notification.numberType = p->ReadInt();
        notification.numberPlan = p->ReadInt();

        return notification;
    }

    protected Object
    ResponseCallRing(Parcel p){
        Char32 response[] = new Char32[4];

        response[0] = (Char32) p->ReadInt();    // isPresent
        response[1] = (Char32) p->ReadInt();    // signalType
        response[2] = (Char32) p->ReadInt();    // alertPitch
        response[3] = (Char32) p->ReadInt();    // signal

        return response;
    }

    private ArrayList<ApnSetting> ResponseGetDataCallProfile(Parcel p) {
        Int32 nProfiles = p->ReadInt();
        If (RILJ_LOGD) RiljLog("# data call profiles:" + nProfiles);

        ArrayList<ApnSetting> response = new ArrayList<ApnSetting>(nProfiles);

        Int32 profileId = 0;
        Int32 priority = 0;
        For (Int32 i = 0; i < nProfiles; i++) {
            profileId = p->ReadInt();
            priority = p->ReadInt();
            ApnProfileOmh profile = new ApnProfileOmh(profileId, priority);
            If (RILJ_LOGD) {
                RiljLog("ResponseGetDataCallProfile()" +
                        profile->GetProfileId() + ":" + profile->GetPriority());
            }
            response->Add(profile);
        }

        return response;
    }

    protected void
    NotifyRegistrantsCdmaInfoRec(CdmaInformationRecords infoRec) {
        Int32 response = RIL_UNSOL_CDMA_INFO_REC;
        If (infoRec.record instanceof CdmaInformationRecords.CdmaDisplayInfoRec) {
            If (mDisplayInfoRegistrants != NULL) {
                If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
                mDisplayInfoRegistrants->NotifyRegistrants(
                        new AsyncResult (NULL, infoRec.record, NULL));
            }
        } else If (infoRec.record instanceof CdmaInformationRecords.CdmaSignalInfoRec) {
            If (mSignalInfoRegistrants != NULL) {
                If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
                mSignalInfoRegistrants->NotifyRegistrants(
                        new AsyncResult (NULL, infoRec.record, NULL));
            }
        } else If (infoRec.record instanceof CdmaInformationRecords.CdmaNumberInfoRec) {
            If (mNumberInfoRegistrants != NULL) {
                If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
                mNumberInfoRegistrants->NotifyRegistrants(
                        new AsyncResult (NULL, infoRec.record, NULL));
            }
        } else If (infoRec.record instanceof CdmaInformationRecords.CdmaRedirectingNumberInfoRec) {
            If (mRedirNumInfoRegistrants != NULL) {
                If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
                mRedirNumInfoRegistrants->NotifyRegistrants(
                        new AsyncResult (NULL, infoRec.record, NULL));
            }
        } else If (infoRec.record instanceof CdmaInformationRecords.CdmaLineControlInfoRec) {
            If (mLineControlInfoRegistrants != NULL) {
                If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
                mLineControlInfoRegistrants->NotifyRegistrants(
                        new AsyncResult (NULL, infoRec.record, NULL));
            }
        } else If (infoRec.record instanceof CdmaInformationRecords.CdmaT53ClirInfoRec) {
            If (mT53ClirInfoRegistrants != NULL) {
                If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
                mT53ClirInfoRegistrants->NotifyRegistrants(
                        new AsyncResult (NULL, infoRec.record, NULL));
            }
        } else If (infoRec.record instanceof CdmaInformationRecords.CdmaT53AudioControlInfoRec) {
            If (mT53AudCntrlInfoRegistrants != NULL) {
               If (RILJ_LOGD) UnsljLogRet(response, infoRec.record);
               mT53AudCntrlInfoRegistrants->NotifyRegistrants(
                       new AsyncResult (NULL, infoRec.record, NULL));
            }
        }
    }

    protected ArrayList<CellInfo> ResponseCellInfoList(Parcel p) {
        Int32 numberOfInfoRecs;
        ArrayList<CellInfo> response;

        /**
         * Loop through all of the information records unmarshalling them
         * and converting them to Java Objects.
         */
        numberOfInfoRecs = p->ReadInt();
        response = new ArrayList<CellInfo>(numberOfInfoRecs);

        For (Int32 i = 0; i < numberOfInfoRecs; i++) {
            CellInfo InfoRec = CellInfo.CREATOR->CreateFromParcel(p);
            response->Add(InfoRec);
        }

        return response;
    }

   private Object
   ResponseHardwareConfig(Parcel p) {
      Int32 num;
      ArrayList<HardwareConfig> response;
      HardwareConfig hw;

      num = p->ReadInt();
      response = new ArrayList<HardwareConfig>(num);

      If (RILJ_LOGV) {
         RiljLog("responseHardwareConfig: num=" + num);
      }
      For (Int32 i = 0 ; i < num ; i++) {
         Int32 type = p->ReadInt();
         Switch(type) {
            case HardwareConfig.DEV_HARDWARE_TYPE_MODEM: {
               hw = new HardwareConfig(type);
               hw->AssignModem(p->ReadString(), p->ReadInt(), p->ReadInt(),
                  p->ReadInt(), p->ReadInt(), p->ReadInt(), p->ReadInt());
               break;
            }
            case HardwareConfig.DEV_HARDWARE_TYPE_SIM: {
               hw = new HardwareConfig(type);
               hw->AssignSim(p->ReadString(), p->ReadInt(), p->ReadString());
               break;
            }
            default: {
               throw new RuntimeException(
                  "RIL_REQUEST_GET_HARDWARE_CONFIG invalid hardward type:" + type);
            }
         }

         response->Add(hw);
      }

      return response;
   }

    static String
    RequestToString(Int32 request) {
/*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'
*/
        Switch(request) {
            case RIL_REQUEST_GET_SIM_STATUS: return "GET_SIM_STATUS";
            case RIL_REQUEST_ENTER_SIM_PIN: return "ENTER_SIM_PIN";
            case RIL_REQUEST_ENTER_SIM_PUK: return "ENTER_SIM_PUK";
            case RIL_REQUEST_ENTER_SIM_PIN2: return "ENTER_SIM_PIN2";
            case RIL_REQUEST_ENTER_SIM_PUK2: return "ENTER_SIM_PUK2";
            case RIL_REQUEST_CHANGE_SIM_PIN: return "CHANGE_SIM_PIN";
            case RIL_REQUEST_CHANGE_SIM_PIN2: return "CHANGE_SIM_PIN2";
            case RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE: return "ENTER_DEPERSONALIZATION_CODE";
            case RIL_REQUEST_GET_CURRENT_CALLS: return "GET_CURRENT_CALLS";
            case RIL_REQUEST_DIAL: return "DIAL";
            case RIL_REQUEST_GET_IMSI: return "GET_IMSI";
            case RIL_REQUEST_HANGUP: return "HANGUP";
            case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: return "HANGUP_WAITING_OR_BACKGROUND";
            case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: return "HANGUP_FOREGROUND_RESUME_BACKGROUND";
            case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: return "REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
            case RIL_REQUEST_CONFERENCE: return "CONFERENCE";
            case RIL_REQUEST_UDUB: return "UDUB";
            case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: return "LAST_CALL_FAIL_CAUSE";
            case RIL_REQUEST_SIGNAL_STRENGTH: return "SIGNAL_STRENGTH";
            case RIL_REQUEST_VOICE_REGISTRATION_STATE: return "VOICE_REGISTRATION_STATE";
            case RIL_REQUEST_DATA_REGISTRATION_STATE: return "DATA_REGISTRATION_STATE";
            case RIL_REQUEST_OPERATOR: return "OPERATOR";
            case RIL_REQUEST_RADIO_POWER: return "RADIO_POWER";
            case RIL_REQUEST_DTMF: return "DTMF";
            case RIL_REQUEST_SEND_SMS: return "SEND_SMS";
            case RIL_REQUEST_SEND_SMS_EXPECT_MORE: return "SEND_SMS_EXPECT_MORE";
            case RIL_REQUEST_SETUP_DATA_CALL: return "SETUP_DATA_CALL";
            case RIL_REQUEST_SIM_IO: return "SIM_IO";
            case RIL_REQUEST_SEND_USSD: return "SEND_USSD";
            case RIL_REQUEST_CANCEL_USSD: return "CANCEL_USSD";
            case RIL_REQUEST_GET_CLIR: return "GET_CLIR";
            case RIL_REQUEST_SET_CLIR: return "SET_CLIR";
            case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: return "QUERY_CALL_FORWARD_STATUS";
            case RIL_REQUEST_SET_CALL_FORWARD: return "SET_CALL_FORWARD";
            case RIL_REQUEST_QUERY_CALL_WAITING: return "QUERY_CALL_WAITING";
            case RIL_REQUEST_SET_CALL_WAITING: return "SET_CALL_WAITING";
            case RIL_REQUEST_SMS_ACKNOWLEDGE: return "SMS_ACKNOWLEDGE";
            case RIL_REQUEST_GET_IMEI: return "GET_IMEI";
            case RIL_REQUEST_GET_IMEISV: return "GET_IMEISV";
            case RIL_REQUEST_ANSWER: return "ANSWER";
            case RIL_REQUEST_DEACTIVATE_DATA_CALL: return "DEACTIVATE_DATA_CALL";
            case RIL_REQUEST_QUERY_FACILITY_LOCK: return "QUERY_FACILITY_LOCK";
            case RIL_REQUEST_SET_FACILITY_LOCK: return "SET_FACILITY_LOCK";
            case RIL_REQUEST_CHANGE_BARRING_PASSWORD: return "CHANGE_BARRING_PASSWORD";
            case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: return "QUERY_NETWORK_SELECTION_MODE";
            case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: return "SET_NETWORK_SELECTION_AUTOMATIC";
            case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: return "SET_NETWORK_SELECTION_MANUAL";
            case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : return "QUERY_AVAILABLE_NETWORKS ";
            case RIL_REQUEST_DTMF_START: return "DTMF_START";
            case RIL_REQUEST_DTMF_STOP: return "DTMF_STOP";
            case RIL_REQUEST_BASEBAND_VERSION: return "BASEBAND_VERSION";
            case RIL_REQUEST_SEPARATE_CONNECTION: return "SEPARATE_CONNECTION";
            case RIL_REQUEST_SET_MUTE: return "SET_MUTE";
            case RIL_REQUEST_GET_MUTE: return "GET_MUTE";
            case RIL_REQUEST_QUERY_CLIP: return "QUERY_CLIP";
            case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: return "LAST_DATA_CALL_FAIL_CAUSE";
            case RIL_REQUEST_DATA_CALL_LIST: return "DATA_CALL_LIST";
            case RIL_REQUEST_RESET_RADIO: return "RESET_RADIO";
            case RIL_REQUEST_OEM_HOOK_RAW: return "OEM_HOOK_RAW";
            case RIL_REQUEST_OEM_HOOK_STRINGS: return "OEM_HOOK_STRINGS";
            case RIL_REQUEST_SCREEN_STATE: return "SCREEN_STATE";
            case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: return "SET_SUPP_SVC_NOTIFICATION";
            case RIL_REQUEST_WRITE_SMS_TO_SIM: return "WRITE_SMS_TO_SIM";
            case RIL_REQUEST_DELETE_SMS_ON_SIM: return "DELETE_SMS_ON_SIM";
            case RIL_REQUEST_SET_BAND_MODE: return "SET_BAND_MODE";
            case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: return "QUERY_AVAILABLE_BAND_MODE";
            case RIL_REQUEST_STK_GET_PROFILE: return "REQUEST_STK_GET_PROFILE";
            case RIL_REQUEST_STK_SET_PROFILE: return "REQUEST_STK_SET_PROFILE";
            case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: return "REQUEST_STK_SEND_ENVELOPE_COMMAND";
            case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: return "REQUEST_STK_SEND_TERMINAL_RESPONSE";
            case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: return "REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
            case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: return "REQUEST_EXPLICIT_CALL_TRANSFER";
            case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: return "REQUEST_SET_PREFERRED_NETWORK_TYPE";
            case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: return "REQUEST_GET_PREFERRED_NETWORK_TYPE";
            case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: return "REQUEST_GET_NEIGHBORING_CELL_IDS";
            case RIL_REQUEST_SET_LOCATION_UPDATES: return "REQUEST_SET_LOCATION_UPDATES";
            case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: return "RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE";
            case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: return "RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE";
            case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: return "RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE";
            case RIL_REQUEST_SET_TTY_MODE: return "RIL_REQUEST_SET_TTY_MODE";
            case RIL_REQUEST_QUERY_TTY_MODE: return "RIL_REQUEST_QUERY_TTY_MODE";
            case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: return "RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE";
            case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: return "RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE";
            case RIL_REQUEST_CDMA_FLASH: return "RIL_REQUEST_CDMA_FLASH";
            case RIL_REQUEST_CDMA_BURST_DTMF: return "RIL_REQUEST_CDMA_BURST_DTMF";
            case RIL_REQUEST_CDMA_SEND_SMS: return "RIL_REQUEST_CDMA_SEND_SMS";
            case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: return "RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE";
            case RIL_REQUEST_GSM_GET_BROADCAST_CONFIG: return "RIL_REQUEST_GSM_GET_BROADCAST_CONFIG";
            case RIL_REQUEST_GSM_SET_BROADCAST_CONFIG: return "RIL_REQUEST_GSM_SET_BROADCAST_CONFIG";
            case RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG: return "RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG";
            case RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG: return "RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG";
            case RIL_REQUEST_GSM_BROADCAST_ACTIVATION: return "RIL_REQUEST_GSM_BROADCAST_ACTIVATION";
            case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: return "RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY";
            case RIL_REQUEST_CDMA_BROADCAST_ACTIVATION: return "RIL_REQUEST_CDMA_BROADCAST_ACTIVATION";
            case RIL_REQUEST_CDMA_SUBSCRIPTION: return "RIL_REQUEST_CDMA_SUBSCRIPTION";
            case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: return "RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM";
            case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: return "RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM";
            case RIL_REQUEST_DEVICE_IDENTITY: return "RIL_REQUEST_DEVICE_IDENTITY";
            case RIL_REQUEST_GET_SMSC_ADDRESS: return "RIL_REQUEST_GET_SMSC_ADDRESS";
            case RIL_REQUEST_SET_SMSC_ADDRESS: return "RIL_REQUEST_SET_SMSC_ADDRESS";
            case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: return "REQUEST_EXIT_EMERGENCY_CALLBACK_MODE";
            case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: return "RIL_REQUEST_REPORT_SMS_MEMORY_STATUS";
            case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: return "RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING";
            case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: return "RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE";
            case RIL_REQUEST_GET_DATA_CALL_PROFILE: return "RIL_REQUEST_GET_DATA_CALL_PROFILE";
            case RIL_REQUEST_ISIM_AUTHENTICATION: return "RIL_REQUEST_ISIM_AUTHENTICATION";
            case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: return "RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU";
            case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: return "RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS";
            case RIL_REQUEST_VOICE_RADIO_TECH: return "RIL_REQUEST_VOICE_RADIO_TECH";
            case RIL_REQUEST_GET_CELL_INFO_LIST: return "RIL_REQUEST_GET_CELL_INFO_LIST";
            case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: return "RIL_REQUEST_SET_CELL_INFO_LIST_RATE";
            case RIL_REQUEST_SET_INITIAL_ATTACH_APN: return "RIL_REQUEST_SET_INITIAL_ATTACH_APN";
            case RIL_REQUEST_SET_DATA_PROFILE: return "RIL_REQUEST_SET_DATA_PROFILE";
            case RIL_REQUEST_IMS_REGISTRATION_STATE: return "RIL_REQUEST_IMS_REGISTRATION_STATE";
            case RIL_REQUEST_IMS_SEND_SMS: return "RIL_REQUEST_IMS_SEND_SMS";
            case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: return "RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC";
            case RIL_REQUEST_SIM_OPEN_CHANNEL: return "RIL_REQUEST_SIM_OPEN_CHANNEL";
            case RIL_REQUEST_SIM_CLOSE_CHANNEL: return "RIL_REQUEST_SIM_CLOSE_CHANNEL";
            case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: return "RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL";
            case RIL_REQUEST_NV_READ_ITEM: return "RIL_REQUEST_NV_READ_ITEM";
            case RIL_REQUEST_NV_WRITE_ITEM: return "RIL_REQUEST_NV_WRITE_ITEM";
            case RIL_REQUEST_NV_WRITE_CDMA_PRL: return "RIL_REQUEST_NV_WRITE_CDMA_PRL";
            case RIL_REQUEST_NV_RESET_CONFIG: return "RIL_REQUEST_NV_RESET_CONFIG";
            case RIL_REQUEST_SET_UICC_SUBSCRIPTION: return "RIL_REQUEST_SET_UICC_SUBSCRIPTION";
            case RIL_REQUEST_ALLOW_DATA: return "RIL_REQUEST_ALLOW_DATA";
            case RIL_REQUEST_GET_HARDWARE_CONFIG: return "GET_HARDWARE_CONFIG";
            case RIL_REQUEST_SIM_AUTHENTICATION: return "RIL_REQUEST_SIM_AUTHENTICATION";
            case RIL_REQUEST_SHUTDOWN: return "RIL_REQUEST_SHUTDOWN";
            default: return "<unknown request>";
        }
    }

    static String
    ResponseToString(Int32 request)
    {
/*
 cat libs/telephony/ril_unsol_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'
*/
        Switch(request) {
            case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: return "UNSOL_RESPONSE_RADIO_STATE_CHANGED";
            case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: return "UNSOL_RESPONSE_CALL_STATE_CHANGED";
            case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED";
            case RIL_UNSOL_RESPONSE_NEW_SMS: return "UNSOL_RESPONSE_NEW_SMS";
            case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: return "UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT";
            case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: return "UNSOL_RESPONSE_NEW_SMS_ON_SIM";
            case RIL_UNSOL_ON_USSD: return "UNSOL_ON_USSD";
            case RIL_UNSOL_ON_USSD_REQUEST: return "UNSOL_ON_USSD_REQUEST";
            case RIL_UNSOL_NITZ_TIME_RECEIVED: return "UNSOL_NITZ_TIME_RECEIVED";
            case RIL_UNSOL_SIGNAL_STRENGTH: return "UNSOL_SIGNAL_STRENGTH";
            case RIL_UNSOL_DATA_CALL_LIST_CHANGED: return "UNSOL_DATA_CALL_LIST_CHANGED";
            case RIL_UNSOL_SUPP_SVC_NOTIFICATION: return "UNSOL_SUPP_SVC_NOTIFICATION";
            case RIL_UNSOL_STK_SESSION_END: return "UNSOL_STK_SESSION_END";
            case RIL_UNSOL_STK_PROACTIVE_COMMAND: return "UNSOL_STK_PROACTIVE_COMMAND";
            case RIL_UNSOL_STK_EVENT_NOTIFY: return "UNSOL_STK_EVENT_NOTIFY";
            case RIL_UNSOL_STK_CALL_SETUP: return "UNSOL_STK_CALL_SETUP";
            case RIL_UNSOL_SIM_SMS_STORAGE_FULL: return "UNSOL_SIM_SMS_STORAGE_FULL";
            case RIL_UNSOL_SIM_REFRESH: return "UNSOL_SIM_REFRESH";
            case RIL_UNSOL_CALL_RING: return "UNSOL_CALL_RING";
            case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED: return "UNSOL_RESPONSE_SIM_STATUS_CHANGED";
            case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: return "UNSOL_RESPONSE_CDMA_NEW_SMS";
            case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: return "UNSOL_RESPONSE_NEW_BROADCAST_SMS";
            case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: return "UNSOL_CDMA_RUIM_SMS_STORAGE_FULL";
            case RIL_UNSOL_RESTRICTED_STATE_CHANGED: return "UNSOL_RESTRICTED_STATE_CHANGED";
            case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: return "UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
            case RIL_UNSOL_CDMA_CALL_WAITING: return "UNSOL_CDMA_CALL_WAITING";
            case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: return "UNSOL_CDMA_OTA_PROVISION_STATUS";
            case RIL_UNSOL_CDMA_INFO_REC: return "UNSOL_CDMA_INFO_REC";
            case RIL_UNSOL_OEM_HOOK_RAW: return "UNSOL_OEM_HOOK_RAW";
            case RIL_UNSOL_RINGBACK_TONE: return "UNSOL_RINGBACK_TONE";
            case RIL_UNSOL_RESEND_INCALL_MUTE: return "UNSOL_RESEND_INCALL_MUTE";
            case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: return "CDMA_SUBSCRIPTION_SOURCE_CHANGED";
            case RIL_UNSOl_CDMA_PRL_CHANGED: return "UNSOL_CDMA_PRL_CHANGED";
            case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: return "UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
            case RIL_UNSOL_RIL_CONNECTED: return "UNSOL_RIL_CONNECTED";
            case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: return "UNSOL_VOICE_RADIO_TECH_CHANGED";
            case RIL_UNSOL_CELL_INFO_LIST: return "UNSOL_CELL_INFO_LIST";
            case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED:
                return "UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED";
            case RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED:
                    return "RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED";
            case RIL_UNSOL_SRVCC_STATE_NOTIFY:
                    return "UNSOL_SRVCC_STATE_NOTIFY";
            case RIL_UNSOL_HARDWARE_CONFIG_CHANGED: return "RIL_UNSOL_HARDWARE_CONFIG_CHANGED";
            case RIL_UNSOL_ON_SS: return "UNSOL_ON_SS";
            case RIL_UNSOL_STK_CC_ALPHA_NOTIFY: return "UNSOL_STK_CC_ALPHA_NOTIFY";
            case RIL_UNSOL_STK_SEND_SMS_RESULT: return "RIL_UNSOL_STK_SEND_SMS_RESULT";
            default: return "<unknown response>";
        }
    }

    protected void RiljLog(String msg) {
        Rlog->D(RILJ_LOG_TAG, msg
                + (mInstanceId != NULL ? (" [SUB" + mInstanceId + "]") : ""));
    }

    protected void RiljLogv(String msg) {
        Rlog->V(RILJ_LOG_TAG, msg
                + (mInstanceId != NULL ? (" [SUB" + mInstanceId + "]") : ""));
    }

    protected void UnsljLog(Int32 response) {
        RiljLog("[UNSL]< " + ResponseToString(response));
    }

    protected void UnsljLogMore(Int32 response, String more) {
        RiljLog("[UNSL]< " + ResponseToString(response) + " " + more);
    }

    protected void UnsljLogRet(Int32 response, Object ret) {
        RiljLog("[UNSL]< " + ResponseToString(response) + " " + RetToString(response, ret));
    }

    protected void UnsljLogvRet(Int32 response, Object ret) {
        RiljLogv("[UNSL]< " + ResponseToString(response) + " " + RetToString(response, ret));
    }

    private Object
    ResponseSsData(Parcel p) {
        Int32 num;
        SsData ssData = new SsData();

        ssData.serviceType = ssData->ServiceTypeFromRILInt(p->ReadInt());
        ssData.requestType = ssData->RequestTypeFromRILInt(p->ReadInt());
        ssData.teleserviceType = ssData->TeleserviceTypeFromRILInt(p->ReadInt());
        ssData.serviceClass = p->ReadInt(); // This is service class sent in the SS request.
        ssData.result = p->ReadInt(); // This is the result of the SS request.
        num = p->ReadInt();

        If (ssData.serviceType->IsTypeCF() &&
            ssData.requestType->IsTypeInterrogation()) {
            ssData.cfInfo = new CallForwardInfo[num];

            For (Int32 i = 0; i < num; i++) {
                ssData.cfInfo[i] = new CallForwardInfo();

                ssData.cfInfo[i].status = p->ReadInt();
                ssData.cfInfo[i].reason = p->ReadInt();
                ssData.cfInfo[i].serviceClass = p->ReadInt();
                ssData.cfInfo[i].toa = p->ReadInt();
                ssData.cfInfo[i].number = p->ReadString();
                ssData.cfInfo[i].timeSeconds = p->ReadInt();

                RiljLog("[SS Data] CF Info " + i + " : " +  ssData.cfInfo[i]);
            }
        } else {
            ssData.ssInfo = new Int32[num];
            For (Int32 i = 0; i < num; i++) {
                ssData.ssInfo[i] = p->ReadInt();
                RiljLog("[SS Data] SS Info " + i + " : " +  ssData.ssInfo[i]);
            }
        }

        return ssData;
    }


    // ***** Methods for CDMA support
    //@Override
    CARAPI
    GetDeviceIdentity(Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_DEVICE_IDENTITY, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI
    GetCDMASubscription(Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_SUBSCRIPTION, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI SetPhoneType(Int32 phoneType) { // Called by CDMAPhone and GSMPhone constructor
        If (RILJ_LOGD) RiljLog("setPhoneType=" + phoneType + " old value=" + mPhoneType);
        mPhoneType = phoneType;
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI QueryCdmaRoamingPreference(Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetCdmaRoamingPreference(Int32 cdmaRoamingType, Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(cdmaRoamingType);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + cdmaRoamingType);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetCdmaSubscriptionSource(Int32 cdmaSubscription , Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(cdmaSubscription);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + cdmaSubscription);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetCdmaSubscriptionSource(Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI QueryTTYMode(Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_QUERY_TTY_MODE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetTTYMode(Int32 ttyMode, Message response) {
        RILRequest rr = RILRequest->Obtain(
                RILConstants.RIL_REQUEST_SET_TTY_MODE, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(ttyMode);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + ttyMode);

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI
    SendCDMAFeatureCode(String FeatureCode, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_FLASH, response);

        rr.mParcel->WriteString(FeatureCode);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " : " + FeatureCode);

        Send(rr);
    }

    //@Override
    CARAPI GetCdmaBroadcastConfig(Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG, response);

        Send(rr);
    }

    //@Override
    CARAPI SetCdmaBroadcastConfig(CdmaSmsBroadcastConfigInfo[] configs, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG, response);

        // Convert to 1 service category per Config (the way RIL takes is)
        ArrayList<CdmaSmsBroadcastConfigInfo> processedConfigs =
            new ArrayList<CdmaSmsBroadcastConfigInfo>();
        For (CdmaSmsBroadcastConfigInfo config : configs) {
            For (Int32 i = config->GetFromServiceCategory(); i <= config->GetToServiceCategory(); i++) {
                processedConfigs->Add(new CdmaSmsBroadcastConfigInfo(i,
                        i,
                        config->GetLanguage(),
                        config->IsSelected()));
            }
        }

        CdmaSmsBroadcastConfigInfo[] rilConfigs = processedConfigs->ToArray(configs);
        rr.mParcel->WriteInt(rilConfigs.length);
        For(Int32 i = 0; i < rilConfigs.length; i++) {
            rr.mParcel->WriteInt(rilConfigs[i].GetFromServiceCategory());
            rr.mParcel->WriteInt(rilConfigs[i].GetLanguage());
            rr.mParcel->WriteInt(rilConfigs[i].IsSelected() ? 1 : 0);
        }

        If (RILJ_LOGD) {
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " with " + rilConfigs.length + " configs : ");
            For (Int32 i = 0; i < rilConfigs.length; i++) {
                RiljLog(rilConfigs[i].ToString());
            }
        }

        Send(rr);
    }

    //@Override
    CARAPI SetCdmaBroadcastActivation(Boolean activate, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_CDMA_BROADCAST_ACTIVATION, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(activate ? 0 :1);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI ExitEmergencyCallbackMode(Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE, response);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI RequestIsimAuthentication(String nonce, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_ISIM_AUTHENTICATION, response);

        rr.mParcel->WriteString(nonce);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI RequestIccSimAuthentication(Int32 authContext, String data, String aid,
                                            Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SIM_AUTHENTICATION, response);

        rr.mParcel->WriteInt(authContext);
        rr.mParcel->WriteString(data);
        rr.mParcel->WriteString(aid);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetCellInfoList(Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_GET_CELL_INFO_LIST, result);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SetCellInfoListRate(Int32 rateInMillis, Message response) {
        If (RILJ_LOGD) RiljLog("setCellInfoListRate: " + rateInMillis);
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(rateInMillis);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    CARAPI SetInitialAttachApn(String apn, String protocol, Int32 authType, String username,
            String password, Message result) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SET_INITIAL_ATTACH_APN, result);

        If (RILJ_LOGD) RiljLog("Set RIL_REQUEST_SET_INITIAL_ATTACH_APN");

        rr.mParcel->WriteString(apn);
        rr.mParcel->WriteString(protocol);
        rr.mParcel->WriteInt(authType);
        rr.mParcel->WriteString(username);
        rr.mParcel->WriteString(password);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + ", apn:" + apn + ", protocol:" + protocol + ", authType:" + authType
                + ", username:" + username + ", password:" + password);

        Send(rr);
    }

    CARAPI SetDataProfile(DataProfile[] dps, Message result) {
        If (RILJ_LOGD) RiljLog("Set RIL_REQUEST_SET_DATA_PROFILE");

        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SET_DATA_PROFILE, NULL);
        DataProfile->ToParcel(rr.mParcel, dps);

        If (RILJ_LOGD) {
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                    + " with " + dps + " Data Profiles : ");
            For (Int32 i = 0; i < dps.length; i++) {
                RiljLog(dps[i].ToString());
            }
        }

        Send(rr);
    }

    /* (non-Javadoc)
     * @see com.android.internal.telephony.BaseCommands#TestingEmergencyCall()
     */
    //@Override
    CARAPI TestingEmergencyCall() {
        If (RILJ_LOGD) RiljLog("testingEmergencyCall");
        mTestingEmergencyCall->Set(TRUE);
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("RIL: " + this);
        pw->Println(" mSocket=" + mSocket);
        pw->Println(" mSenderThread=" + mSenderThread);
        pw->Println(" mSender=" + mSender);
        pw->Println(" mReceiverThread=" + mReceiverThread);
        pw->Println(" mReceiver=" + mReceiver);
        pw->Println(" mWakeLock=" + mWakeLock);
        pw->Println(" mWakeLockTimeout=" + mWakeLockTimeout);
        Synchronized (mRequestList) {
            Synchronized (mWakeLock) {
                pw->Println(" mWakeLockCount=" + mWakeLockCount);
            }
            Int32 count = mRequestList->Size();
            pw->Println(" mRequestList count=" + count);
            For (Int32 i = 0; i < count; i++) {
                RILRequest rr = mRequestList->ValueAt(i);
                pw->Println("  [" + rr.mSerial + "] " + RequestToString(rr.mRequest));
            }
        }
        pw->Println(" mLastNITZTimeInfo=" + mLastNITZTimeInfo);
        pw->Println(" mTestingEmergencyCall=" + mTestingEmergencyCall->Get());
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI IccOpenLogicalChannel(String AID, Message response) {
        If(mRilVersion < 10) {
            If (response != NULL) {
                CommandException ex = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
                AsyncResult->ForMessage(response, NULL, ex);
                response->SendToTarget();
            }
            return;
        }

        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SIM_OPEN_CHANNEL, response);
        rr.mParcel->WriteString(AID);

        If (RILJ_LOGD)
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI IccCloseLogicalChannel(Int32 channel, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SIM_CLOSE_CHANNEL, response);
        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(channel);

        If (RILJ_LOGD)
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI IccTransmitApduLogicalChannel(Int32 channel, Int32 cla, Int32 instruction,
            Int32 p1, Int32 p2, Int32 p3, String data, Message response) {

        If(mRilVersion < 10) {
            If (response != NULL) {
                CommandException ex = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
                AsyncResult->ForMessage(response, NULL, ex);
                response->SendToTarget();
            }
            return;
        }

        If (channel <= 0) {
            throw new RuntimeException(
                "Invalid channel in iccTransmitApduLogicalChannel: " + channel);
        }

        IccTransmitApduHelper(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, channel, cla,
                instruction, p1, p2, p3, data, response);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI IccTransmitApduBasicChannel(Int32 cla, Int32 instruction, Int32 p1, Int32 p2,
            Int32 p3, String data, Message response) {
        IccTransmitApduHelper(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, 0, cla, instruction,
                p1, p2, p3, data, response);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI GetAtr(Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_SIM_GET_ATR, response);
        Int32 slotId = 0;
        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(slotId);
        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> iccGetAtr: "
                + RequestToString(rr.mRequest) + " " + slotId);

        Send(rr);
    }

    /*
     * Helper function for the iccTransmitApdu* commands above.
     */
    private void IccTransmitApduHelper(Int32 rilCommand, Int32 channel, Int32 cla,
            Int32 instruction, Int32 p1, Int32 p2, Int32 p3, String data, Message response) {

        If(mRilVersion < 10) {
            If (response != NULL) {
                CommandException ex = new CommandException(
                    CommandException.Error.REQUEST_NOT_SUPPORTED);
                AsyncResult->ForMessage(response, NULL, ex);
                response->SendToTarget();
            }
            return;
        }

        RILRequest rr = RILRequest->Obtain(rilCommand, response);
        rr.mParcel->WriteInt(channel);
        rr.mParcel->WriteInt(cla);
        rr.mParcel->WriteInt(instruction);
        rr.mParcel->WriteInt(p1);
        rr.mParcel->WriteInt(p2);
        rr.mParcel->WriteInt(p3);
        rr.mParcel->WriteString(data);

        If (RILJ_LOGD)
            RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest));

        Send(rr);
    }

    //@Override
    CARAPI NvReadItem(Int32 itemID, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_NV_READ_ITEM, response);

        rr.mParcel->WriteInt(itemID);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + ' ' + itemID);

        Send(rr);
    }

    //@Override
    CARAPI NvWriteItem(Int32 itemID, String itemValue, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_NV_WRITE_ITEM, response);

        rr.mParcel->WriteInt(itemID);
        rr.mParcel->WriteString(itemValue);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + ' ' + itemID + ": " + itemValue);

        Send(rr);
    }

    //@Override
    CARAPI NvWriteCdmaPrl(Byte[] preferredRoamingList, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_NV_WRITE_CDMA_PRL, response);

        rr.mParcel->WriteByteArray(preferredRoamingList);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + " (" + preferredRoamingList.length + " bytes)");

        Send(rr);
    }

    //@Override
    CARAPI NvResetConfig(Int32 resetType, Message response) {
        RILRequest rr = RILRequest->Obtain(RIL_REQUEST_NV_RESET_CONFIG, response);

        rr.mParcel->WriteInt(1);
        rr.mParcel->WriteInt(resetType);

        If (RILJ_LOGD) RiljLog(rr->SerialString() + "> " + RequestToString(rr.mRequest)
                + ' ' + resetType);

        Send(rr);
    }
}
