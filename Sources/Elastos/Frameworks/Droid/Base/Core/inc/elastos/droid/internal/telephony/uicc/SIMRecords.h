#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SIMRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SIMRECORDS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/internal/telephony/uicc/IccRecords.h"
#include "elastos/droid/internal/telephony/uicc/VoiceMailConstants.h"
#include "elastos/droid/internal/telephony/uicc/SpnOverride.h"

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class SIMRecords
    : public IccRecords
    , public ISIMRecords
{
private:
    class EfPlLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfPlLoaded(
            /* [in] */ SIMRecords* host);

        virtual CARAPI GetEfName(
            /* [out] */ String* result);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        SIMRecords* mHost;
    };

    class EfUsimLiLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfUsimLiLoaded(
            /* [in] */ SIMRecords* host);

        virtual CARAPI GetEfName(
            /* [out] */ String* result);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        SIMRecords* mHost;
    };

    enum GetSpnFsmState {
        IDLE,               // No initialized
        INIT,               // Start FSM
        READ_SPN_3GPP,      // Load EF_SPN firstly
        READ_SPN_CPHS,      // Load EF_SPN_CPHS secondly
        READ_SPN_SHORT_CPHS // Load EF_SPN_SHORT_CPHS last
    };

public:
    CAR_INTERFACE_DECL();

    SIMRecords();

    // ***** Constructor
    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Dispose();

    //***** Public Methods
    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI GetIMSI(
        /* [out] */ String* result);

    // @Override
    CARAPI GetMsisdnNumber(
        /* [out] */ String* result);

    // @Override
    CARAPI GetGid1(
        /* [out] */ String* result);

    // @Override
    CARAPI GetUsimServiceTable(
        /* [out] */ IUsimServiceTable** result);

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
    // @Override
    CARAPI SetMsisdnNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& number,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetMsisdnAlphaTag(
        /* [out] */ String* result);

    // @Override
    CARAPI GetVoiceMailNumber(
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
    // @Override
    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* result);

    /**
      * Sets the SIM voice message waiting indicator records
      * @param line GSM Subscriber Profile Number, one-based. Only '1' is supported
      * @param countWaiting The number of messages waiting, if known. Use
      *                     -1 to indicate that an unknown number of
      *                      messages are waiting
      */
    // @Override
    CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    virtual CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI IsCallForwardStatusStored(
        /* [out] */ Boolean* result);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI GetVoiceCallForwardingFlag(
        /* [out] */ Boolean* result);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI SetVoiceCallForwardingFlag(
        /* [in] */ Int32 line,
        /* [in] */ Boolean enable,
        /* [in] */ const String& dialNumber);

    /**
      * Called by STK Service when REFRESH is received.
      * @param fileChanged indicates whether any files changed
      * @param fileList if non-null, a list of EF files that changed
      */
    // @Override
    CARAPI OnRefresh(
        /* [in] */ Boolean fileChanged,
        /* [in] */ ArrayOf<Int32>* fileList);

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    // ***** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /*
      * Called when a Class2 SMS is  received.
      *
      * @param ar AsyncResult passed to this function. "ar.result" should
      *           be representing the INDEX of SMS on SIM.
      */
    virtual CARAPI HandleSmsOnIcc(
        /* [in] */ AsyncResult* ar);

    // @Override
    CARAPI OnReady();

    /**
      * Returns the SpnDisplayRule based on settings on the SIM and the
      * specified plmn (currently-registered PLMN).  See TS 22.101 Annex A
      * and TS 51.011 10.3.11 for details.
      *
      * If the SPN is not found on the SIM or is empty, the rule is
      * always PLMN_ONLY.
      */
    // @Override
    CARAPI GetDisplayRule(
        /* [in] */ const String& plmn,
        /* [out] */ Int32* result);

    /**
      * Return true if "Restriction of menu options for manual PLMN selection"
      * bit is set or EF_CSP data is unavailable, return false otherwise.
      */
    // @Override
    CARAPI IsCspPlmnEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    // @Override
    CARAPI Finalize();

    virtual CARAPI_(void) ResetRecords();

    // @Override
    CARAPI HandleFileUpdate(
        /* [in] */ Int32 efid);

    // @Override
    CARAPI OnRecordLoaded();

    // @Override
    CARAPI OnAllRecordsLoaded();

    virtual CARAPI_(void) FetchSimRecords();

    // @Override
    CARAPI Log(
        /* [in] */ const String& s);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& s);

    virtual CARAPI_(void) Logw(
        /* [in] */ const String& s);
        ///* [in] */ Throwable* tr);

    virtual CARAPI_(void) Logv(
        /* [in] */ const String& s);

private:
    static CARAPI_(AutoPtr<ArrayOf<String> >) MiddleInitMccmncCodesHaving3digitsMnc();

    CARAPI_(Int32) GetExtFromEf(
        /* [in] */ Int32 ef);

    // Validate data is !null and the MSP (Multiple Subscriber Profile)
    // byte is between 1 and 4. See ETSI TS 131 102 v11.3.0 section 4.2.64.
    CARAPI_(Boolean) ValidEfCfis(
        /* [in] */ ArrayOf<Byte>* data);

    /**
      * Dispatch 3GPP format message to registrant ({@code GSMPhone} or {@code CDMALTEPhone})
      * to pass to the 3GPP SMS dispatcher for delivery.
      */
    CARAPI_(Int32) DispatchGsmMessage(
        /* [in] */ ISmsMessage* message);

    CARAPI_(void) HandleSms(
        /* [in] */ ArrayOf<Byte>* ba);

    CARAPI_(void) HandleSmses(
        /* [in] */ IArrayList/*<ArrayOf<Byte>>*/* messages);

    CARAPI_(String) FindBestLanguage(
        /* [in] */ ArrayOf<Byte>* languages);

    CARAPI_(void) SetLocaleFromUsim();

    //***** Private methods
    CARAPI_(void) SetSpnFromConfig(
        /* [in] */ const String& carrier);

    CARAPI_(void) SetVoiceMailByCountry(
        /* [in] */ const String& spn);

    CARAPI_(void) OnLocked();

    CARAPI_(void) LoadEfLiAndEfPl();

    /**
      * Checks if plmn is HPLMN or on the spdiNetworks list.
      */
    CARAPI_(Boolean) IsOnMatchingPlmn(
        /* [in] */ const String& plmn);

    /**
      * States of Get SPN Finite State Machine which only used by getSpnFsm()
      */
    /**
      * Finite State Machine to load Service Provider Name , which can be stored
      * in either EF_SPN (3GPP), EF_SPN_CPHS, or EF_SPN_SHORT_CPHS (CPHS4.2)
      *
      * After starting, FSM will search SPN EFs in order and stop after finding
      * the first valid SPN
      *
      * If the FSM gets restart while waiting for one of
      * SPN EFs results (i.e. a SIM refresh occurs after issuing
      * read EF_CPHS_SPN), it will re-initialize only after
      * receiving and discarding the unfinished SPN EF result.
      *
      * @param start set true only for initialize loading
      * @param ar the AsyncResult from loadEFTransparent
      *        ar.exception holds exception in error
      *        ar.result is byte[] for data in success
      */
    CARAPI_(void) GetSpnFsm(
        /* [in] */ Boolean start,
        /* [in] */ AsyncResult* ar);

    /**
      * Parse TS 51.011 EF[SPDI] record
      * This record contains the list of numeric network IDs that
      * are treated specially when determining SPN display
      */
    CARAPI_(void) ParseEfSpdi(
        /* [in] */ ArrayOf<Byte>* data);

    /**
      * check to see if Mailbox Number is allocated and activated in CPHS SST
      */
    CARAPI_(Boolean) IsCphsMailboxEnabled();

    /**
      * Parse EF_CSP data and check if
      * "Restriction of menu options for manual PLMN selection" is
      * Enabled/Disabled
      *
      * @param data EF_CSP hex data.
      */
    CARAPI_(void) HandleEfCspData(
        /* [in] */ ArrayOf<Byte>* data);

public:
    // ***** Instance Variables
    AutoPtr<VoiceMailConstants> mVmConfig;
    AutoPtr<SpnOverride> mSpnOverride;
    Boolean mCspPlmnEnabled;
    AutoPtr<ArrayOf<Byte> > mEfMWIS;
    AutoPtr<ArrayOf<Byte> > mEfCPHS_MWI;
    AutoPtr<ArrayOf<Byte> > mEfCff;
    AutoPtr<ArrayOf<Byte> > mEfCfis;
    AutoPtr<ArrayOf<Byte> > mEfLi;
    AutoPtr<ArrayOf<Byte> > mEfPl;
    Int32 mSpnDisplayCondition;
    // Numeric network codes listed in TS 51.011 EF[SPDI]
    AutoPtr<IArrayList/*<String>*/> mSpdiNetworks;
    String mPnnHomeName;
    AutoPtr<IUsimServiceTable> mUsimServiceTable;
    // ***** Constants
    // From TS 51.011 EF[SPDI] section
    static const Int32 TAG_SPDI = 0xA3;
    static const Int32 TAG_SPDI_PLMN_LIST = 0x80;
    // Full Name IEI from TS 24.008
    static const Int32 TAG_FULL_NETWORK_NAME = 0x43;
    // Short Name IEI from TS 24.008
    static const Int32 TAG_SHORT_NETWORK_NAME = 0x45;
    // active CFF from CPHS 4.2 B.4.5
    static const Int32 CFF_UNCONDITIONAL_ACTIVE = 0x0a;
    static const Int32 CFF_UNCONDITIONAL_DEACTIVE = 0x05;
    static const Int32 CFF_LINE1_MASK = 0x0f;
    static const Int32 CFF_LINE1_RESET = 0xf0;

protected:
    static const String LOGTAG;
    static const Int32 EVENT_GET_AD_DONE = 9;
    // Admin data on SIM
    static const Int32 EVENT_GET_MSISDN_DONE = 10;
    static const Int32 EVENT_GET_SST_DONE = 17;

private:
    static const Boolean CRASH_RIL;
    // ***** Cached SIM State; cleared on channel close
    Boolean mCallForwardingEnabled;
    /**
      * States only used by getSpnFsm FSM
      */
    GetSpnFsmState mSpnState;
    /** CPHS service information (See CPHS 4.2 B.3.1.1)
      *  It will be set in onSimReady if reading GET_CPHS_INFO successfully
      *  mCphsInfo[0] is CPHS Phase
      *  mCphsInfo[1] and mCphsInfo[2] is CPHS Service Table
      */
    AutoPtr<ArrayOf<Byte> > mCphsInfo;
    // CPHS Service Table (See CPHS 4.2 B.3.1)
    static const Int32 CPHS_SST_MBN_MASK = 0x30;
    static const Int32 CPHS_SST_MBN_ENABLED = 0x30;
    // EF_CFIS related constants
    // Spec reference TS 51.011 section 10.3.46.
    static const Int32 CFIS_BCD_NUMBER_LENGTH_OFFSET = 2;
    static const Int32 CFIS_TON_NPI_OFFSET = 3;
    static const Int32 CFIS_ADN_CAPABILITY_ID_OFFSET = 14;
    static const Int32 CFIS_ADN_EXTENSION_ID_OFFSET = 15;
    // ***** Event Constants
    static const Int32 EVENT_GET_IMSI_DONE = 3;
    static const Int32 EVENT_GET_ICCID_DONE = 4;
    static const Int32 EVENT_GET_MBI_DONE = 5;
    static const Int32 EVENT_GET_MBDN_DONE = 6;
    static const Int32 EVENT_GET_MWIS_DONE = 7;
    static const Int32 EVENT_GET_VOICE_MAIL_INDICATOR_CPHS_DONE = 8;
    static const Int32 EVENT_GET_CPHS_MAILBOX_DONE = 11;
    static const Int32 EVENT_GET_SPN_DONE = 12;
    static const Int32 EVENT_GET_SPDI_DONE = 13;
    static const Int32 EVENT_UPDATE_DONE = 14;
    static const Int32 EVENT_GET_PNN_DONE = 15;
    static const Int32 EVENT_GET_ALL_SMS_DONE = 18;
    static const Int32 EVENT_MARK_SMS_READ_DONE = 19;
    static const Int32 EVENT_SET_MBDN_DONE = 20;
    static const Int32 EVENT_GET_SMS_DONE = 22;
    static const Int32 EVENT_GET_CFF_DONE = 24;
    static const Int32 EVENT_SET_CPHS_MAILBOX_DONE = 25;
    static const Int32 EVENT_GET_INFO_CPHS_DONE = 26;
    // private static final int EVENT_SET_MSISDN_DONE = 30; Defined in IccRecords as 30
    static const Int32 EVENT_GET_CFIS_DONE = 32;
    static const Int32 EVENT_GET_CSP_CPHS_DONE = 33;
    static const Int32 EVENT_GET_GID1_DONE = 34;
    static const Int32 EVENT_APP_LOCKED = 35;
    // Lookup table for carriers known to produce SIMs which incorrectly indicate MNC length.
    static AutoPtr<ArrayOf<String> > MCCMNC_CODES_HAVING_3DIGITS_MNC;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_SIMRECORDS_H__
