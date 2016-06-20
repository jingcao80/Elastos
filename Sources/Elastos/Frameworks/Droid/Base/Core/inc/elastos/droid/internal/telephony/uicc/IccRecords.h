#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCRECORDS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"

// package com.android.internal.telephony.uicc;
// import android.content.Context;
// import android.content.Intent;
// import android.telephony.TelephonyManager;
// import android.os.Registrant;
// import android.telephony.TelephonyManager;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.SubscriptionController;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import java.nio.ByteBuffer;
// import java.util.concurrent.atomic.AtomicBoolean;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class IccRecords
    : public Handler
    , public IIccConstants
    , public IIccRecords
{
public:
    /**
      * Generic ICC record loaded callback. Subclasses can call EF load methods on
      * {@link IccFileHandler} passing a Message for onLoaded with the what field set to
      * {@link #EVENT_GET_ICC_RECORD_DONE} and the obj field set to an instance
      * of this interface. The {@link #handleMessage} method in this class will print a
      * log message using {@link #getEfName()} and decrement {@link #mRecordsToLoad}.
      *
      * If the record load was successful, {@link #onRecordLoaded} will be called with the result.
      * Otherwise, an error log message will be output by {@link #handleMessage} and
      * {@link #onRecordLoaded} will not be called.
      */
    //class IccRecordLoaded
    //{
    //public:
    //    virtual CARAPI_(String) GetEfName() = 0;

    //    virtual CARAPI OnRecordLoaded(
    //        /* [in] */ IAsyncResult* ar) = 0;
    //};

public:
    CAR_INTERFACE_DECL();

    IccRecords();
    // ***** Constructor
    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
      * Call when the IccRecords object is no longer going to be used.
      */
    virtual CARAPI Dispose();

    virtual CARAPI OnReady() = 0;

    //***** Public Methods
    /*
      * Called to indicate that anyone could request records
      * in the future after this call, once records are loaded and registrants
      * have been notified. This indication could be used
      * to optimize when to actually fetch records from the card. We
      * don't need to fetch records from the card if it is of no use
      * to anyone
      *
      */
    virtual CARAPI RecordsRequired();

    virtual CARAPI GetAdnCache(
        /* [out] */ IAdnRecordCache** result);

    virtual CARAPI GetIccId(
        /* [out] */ String* result);

    virtual CARAPI RegisterForRecordsLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForRecordsLoaded(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForImsiReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForImsiReady(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForRecordsEvents(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForRecordsEvents(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForNewSms(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForNewSms(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForNetworkSelectionModeAutomatic(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForNetworkSelectionModeAutomatic(
        /* [in] */ IHandler* h);

    /**
      * Get the International Mobile Subscriber ID (IMSI) on a SIM
      * for GSM, UMTS and like networks. Default is null if IMSI is
      * not supported or unavailable.
      *
      * @return null if SIM is not yet ready or unavailable
      */
    virtual CARAPI GetIMSI(
        /* [out] */ String* result);

    /**
      * Imsi could be set by ServiceStateTrackers in case of cdma
      * @param imsi
      */
    virtual CARAPI SetImsi(
        /* [in] */ const String& imsi);

    virtual CARAPI GetMsisdnNumber(
        /* [out] */ String* result);

    /**
      * Get the Group Identifier Level 1 (GID1) on a SIM for GSM.
      * @return null if SIM is not yet ready
      */
    virtual CARAPI GetGid1(
        /* [out] */ String* result);

    /**
      * Set subscriber number to SIM record
      *
      * The subscriber number is stored in EF_MSISDN (TS 51.011)
      *
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param alphaTag alpha-tagging of the dailing nubmer (up to 10 characters)
      * @param number dailing nubmer (up to 20 digits)
      *        if the number starts with '+', then set to international TOA
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == null on success
      *        ((AsyncResult)onComplete.obj).exception != null on fail
      */
    virtual CARAPI SetMsisdnNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ IMessage* onComplete);

    virtual CARAPI GetMsisdnAlphaTag(
        /* [out] */ String* result);

    virtual CARAPI GetVoiceMailNumber(
        /* [out] */ String* result);

    /**
      * Return Service Provider Name stored in SIM (EF_SPN=0x6F46) or in RUIM (EF_RUIM_SPN=0x6F41).
      *
      * @return null if SIM is not yet ready or no RUIM entry
      */
    virtual CARAPI GetServiceProviderName(
        /* [out] */ String* result);

    /**
      * Set voice mail number to SIM record
      *
      * The voice mail number can be stored either in EF_MBDN (TS 51.011) or
      * EF_MAILBOX_CPHS (CPHS 4.2)
      *
      * If EF_MBDN is available, store the voice mail number to EF_MBDN
      *
      * If EF_MAILBOX_CPHS is enabled, store the voice mail number to EF_CHPS
      *
      * So the voice mail number will be stored in both EFs if both are available
      *
      * Return error only if both EF_MBDN and EF_MAILBOX_CPHS fail.
      *
      * When the operation is complete, onComplete will be sent to its handler
      *
      * @param alphaTag alpha-tagging of the dailing nubmer (upto 10 characters)
      * @param voiceNumber dailing nubmer (upto 20 digits)
      *        if the number is start with '+', then set to international TOA
      * @param onComplete
      *        onComplete.obj will be an AsyncResult
      *        ((AsyncResult)onComplete.obj).exception == null on success
      *        ((AsyncResult)onComplete.obj).exception != null on fail
      */
    virtual CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceNumber,
        /* [in] */ IMessage* onComplete) = 0;

    virtual CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    /**
      * Sets the SIM voice message waiting indicator records
      * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
      * @param countWaiting The number of messages waiting, if known. Use
      *                     -1 to indicate that an unknown number of
      *                      messages are waiting
      */
    virtual CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting) = 0;

    /**
      * Called by GsmPhone to update VoiceMail count
      */
    virtual CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result) = 0;

    /**
      * Called by STK Service when REFRESH is received.
      * @param fileChanged indicates whether any files changed
      * @param fileList if non-null, a list of EF files that changed
      */
    virtual CARAPI OnRefresh(
        /* [in] */ Boolean fileChanged,
        /* [in] */ ArrayOf<Int32>* fileList) = 0;

    virtual CARAPI GetRecordsLoaded(
        /* [out] */ Boolean* result);

    //***** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
      * Returns the SpnDisplayRule based on settings on the SIM and the
      * specified plmn (currently-registered PLMN).  See TS 22.101 Annex A
      * and TS 51.011 10.3.11 for details.
      *
      * If the SPN is not found on the SIM, the rule is always PLMN_ONLY.
      * Generally used for GSM/UMTS and the like SIMs.
      */
    virtual CARAPI GetDisplayRule(
        /* [in] */ const String& plmn,
        /* [out] */ Int32* result) = 0;

    /**
      * Return true if "Restriction of menu options for manual PLMN selection"
      * bit is set or EF_CSP data is unavailable, return false otherwise.
      * Generally used for GSM/UMTS and the like SIMs.
      */
    virtual CARAPI IsCspPlmnEnabled(
        /* [out] */ Boolean* result);

    /**
      * Returns the 5 or 6 digit MCC/MNC of the operator that
      * provided the SIM card. Returns null of SIM is not yet ready
      * or is not valid for the type of IccCard. Generally used for
      * GSM/UMTS and the like SIMS
      */
    virtual CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    /**
      * Check if call forward info is stored on SIM
      * @return true if call forward info is stored on SIM.
      */
    virtual CARAPI IsCallForwardStatusStored(
        /* [out] */ Boolean* result);

    /**
      * Get the current Voice call forwarding flag for GSM/UMTS and the like SIMs
      *
      * @return true if enabled
      */
    virtual CARAPI GetVoiceCallForwardingFlag(
        /* [out] */ Boolean* result);

    /**
      * Set the voice call forwarding flag for GSM/UMTS and the like SIMs
      *
      * @param line to enable/disable
      * @param enable
      * @param number to which CFU is enabled
      */
    virtual CARAPI SetVoiceCallForwardingFlag(
        /* [in] */ Int32 line,
        /* [in] */ Boolean enable,
        /* [in] */ const String& number);

    /**
      * Indicates wether SIM is in provisioned state or not.
      * Overridden only if SIM can be dynamically provisioned via OTA.
      *
      * @return true if provisioned
      */
    virtual CARAPI IsProvisioned(
        /* [out] */ Boolean* result);

    /**
      * Return an interface to retrieve the ISIM records for IMS, if available.
      * @return the interface to retrieve the ISIM records, or null if not supported
      */
    virtual CARAPI GetIsimRecords(
        /* [out] */ IIsimRecords** result);

    virtual CARAPI GetUsimServiceTable(
        /* [out] */ IUsimServiceTable** result);

    /**
      * Returns the response of the SIM application on the UICC to authentication
      * challenge/response algorithm. The data string and challenge response are
      * Base64 encoded Strings.
      * Can support EAP-SIM, EAP-AKA with results encoded per 3GPP TS 31.102.
      *
      * @param authContext parameter P2 that specifies the authentication context per 3GPP TS 31.102 (Section 7.1.2)
      * @param data authentication challenge data
      * @return challenge response
      */
    virtual CARAPI GetIccSimChallengeResponse(
        /* [in] */ Int32 authContext,
        /* [in] */ const String& data,
        /* [out] */ String* result);

    /**
      * To get SMS capacity count on ICC card.
      */
    virtual CARAPI GetSmsCapacityOnIcc(
        /* [out] */ Int32* result);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    virtual CARAPI_(void) SetServiceProviderName(
        /* [in] */ const String& spn);

    /**
      * Called by subclasses (SimRecords and RuimRecords) whenever
      * IccRefreshResponse.REFRESH_RESULT_INIT event received
      */
    virtual CARAPI_(void) OnIccRefreshInit();

    virtual CARAPI HandleFileUpdate(
        /* [in] */ Int32 efid) = 0;

    virtual CARAPI BroadcastRefresh();

    virtual CARAPI_(void) HandleRefresh(
        /* [in] */ IIccRefreshResponse* refreshResponse);

    virtual CARAPI OnRecordLoaded() = 0;

    virtual CARAPI OnAllRecordsLoaded() = 0;

    /**
      * Write string to log file
      *
      * @param s is the string to write
      */
    virtual CARAPI Log(
        /* [in] */ const String& s) = 0;

    /**
      * Write error string to log file.
      *
      * @param s is the string to write
      */
    virtual CARAPI Loge(
        /* [in] */ const String& s) = 0;

    virtual CARAPI_(Boolean) RequirePowerOffOnSimRefreshReset();

    virtual CARAPI_(Boolean) PowerOffOnSimReset();

    virtual CARAPI_(void) SetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& value);

private:
    CARAPI_(void) HandleRefreshOem(
        /* [in] */ ArrayOf<Byte>* data);

protected:
    static const Boolean DBG;
    // ***** Instance Variables
    AutoPtr<IAtomicBoolean> mDestroyed;
    AutoPtr<IContext> mContext;
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<IIccFileHandler> mFh;
    AutoPtr<IUiccCardApplication> mParentApp;
    AutoPtr<RegistrantList> mRecordsLoadedRegistrants;
    AutoPtr<RegistrantList> mImsiReadyRegistrants;
    AutoPtr<RegistrantList> mRecordsEventsRegistrants;
    AutoPtr<RegistrantList> mNewSmsRegistrants;
    AutoPtr<RegistrantList> mNetworkSelectionModeAutomaticRegistrants;
    Int32 mRecordsToLoad;
    // number of pending load requests
    AutoPtr<IAdnRecordCache> mAdnCache;
    // ***** Cached SIM State; cleared on channel close
    Boolean mRecordsRequested;
    // true if we've made requests for the sim records
    String mIccId;
    String mMsisdn;
    // My mobile number
    String mMsisdnTag;
    String mVoiceMailNum;
    String mVoiceMailTag;
    String mNewVoiceMailNum;
    String mNewVoiceMailTag;
    Boolean mIsVoiceMailFixed;
    String mImsi;
    Int32 mMncLength;
    Int32 mMailboxIndex;
    // 0 is no mailbox dailing number associated
    Int32 mSmsCountOnIcc;
    String mSpn;
    String mGid1;
    // ***** Constants
    // Markers for mncLength
    static const Int32 UNINITIALIZED = -1;
    static const Int32 UNKNOWN = 0;
    // ***** Event Constants
    static const Int32 EVENT_SET_MSISDN_DONE = 30;
    // ICC refresh occurred
    static const Int32 EVENT_GET_SMS_RECORD_SIZE_DONE = 28;
    static const Int32 EVENT_APP_READY = 1;

private:
    AutoPtr<IIccIoResult> auth_rsp;
    Object mLock;
    static const Int32 EVENT_AKA_AUTHENTICATE_DONE = 90;
    Boolean mOEMHookSimRefresh;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCRECORDS_H__

