#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_RUIMRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_RUIMRECORDS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/internal/telephony/uicc/IccRecords.h"

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IContext;
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
class RuimRecords
    : public IccRecords
    , public IRuimRecords
{
private:
    // Refer to ETSI TS 102.221
    class EfPlLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfPlLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    // Refer to C.S0065 5.2.26
    class EfCsimLiLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimLiLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    // Refer to C.S0065 5.2.32
    class EfCsimSpnLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimSpnLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfCsimMdnLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimMdnLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfCsimImsimLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimImsimLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfCsimCdmaHomeLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimCdmaHomeLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfCsimEprlLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimEprlLoaded(
            /* [in] */ RuimRecords* host);

        // @Override
        CARAPI GetEfName(
            /* [out] */ String* result);

        // @Override
        CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfCsimModelLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfCsimModelLoaded(
            /* [in] */ RuimRecords* host);

        virtual CARAPI GetEfName(
            /* [out] */ String* result);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfRuimModelLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfRuimModelLoaded(
            /* [in] */ RuimRecords* host);

        virtual CARAPI GetEfName(
            /* [out] */ String* result);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

    class EfRuimIdLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfRuimIdLoaded(
            /* [in] */ RuimRecords* host);

        virtual CARAPI GetEfName(
            /* [out] */ String* result);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        RuimRecords* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    RuimRecords();

    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI GetIMSI(
        /* [out] */ String* result);

    virtual CARAPI GetMdnNumber(
        /* [out] */ String* result);

    virtual CARAPI GetCdmaMin(
        /* [out] */ String* result);

    /** Returns null if RUIM is not yet ready */
    virtual CARAPI GetPrlVersion(
        /* [out] */ String* result);

    // @Override
    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceNumber,
        /* [in] */ IMessage* onComplete);

    /**
      * Called by CCAT Service when REFRESH is received.
      * @param fileChanged indicates whether any files changed
      * @param fileList if non-null, a list of EF files that changed
      */
    // @Override
    CARAPI OnRefresh(
        /* [in] */ Boolean fileChanged,
        /* [in] */ ArrayOf<Int32>* fileList);

    /**
      * Returns the 5 or 6 digit MCC/MNC of the operator that
      *  provided the RUIM card. Returns null of RUIM is not yet ready
      */
    virtual CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI OnReady();

    /**
      * Called by IccCardProxy before it requests records.
      * We use this as a trigger to read records from the card.
      */
    virtual CARAPI RecordsRequired();

    /**
      * {@inheritDoc}
      */
    // @Override
    CARAPI GetDisplayRule(
        /* [in] */ const String& plmnNumeric,
        /* [out] */ Int32* result);

    // @Override
    CARAPI IsProvisioned(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    // @Override
    CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetMdn(
        /* [out] */ String* result);

    virtual CARAPI GetMin(
        /* [out] */ String* result);

    virtual CARAPI GetSid(
        /* [out] */ String* result);

    virtual CARAPI GetNid(
        /* [out] */ String* result);

    virtual CARAPI GetCsimSpnDisplayCondition(
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
    CARAPI OnRecordLoaded();

    // @Override
    CARAPI OnAllRecordsLoaded();

    // @Override
    CARAPI HandleFileUpdate(
        /* [in] */ Int32 efid);

    // @Override
    CARAPI Log(
        /* [in] */ const String& s);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& s);

private:
    CARAPI_(Int32) DecodeImsiDigits(
        /* [in] */ Int32 digits,
        /* [in] */ Int32 length);

    /**
      * Decode utility to decode IMSI from data read from EF_IMSIM
      * Please refer to
      *       // C.S0065 section 5.2.2 for IMSI_M encoding
      *       // C.S0005 section 2.3.1 for MIN encoding in IMSI_M.
      */
    CARAPI_(String) DecodeImsi(
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI_(void) OnGetCSimEprlDone(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) FetchOMHCardRecords(
        /* [in] */ Boolean isOMHCard);

    /**
      * Returns an array of languages we have assets for.
      *
      * NOTE: This array will have duplicates. If this method will be caused
      * frequently or in a tight loop, it can be rewritten for efficiency.
      */
    static CARAPI_(AutoPtr<ArrayOf<String> >) GetAssetLanguages(
        /* [in] */ IContext* ctx);

    CARAPI_(String) FindBestLanguage(
        /* [in] */ ArrayOf<Byte>* languages);

    CARAPI_(void) SetLocaleFromCsim();

    CARAPI_(void) FetchRuimRecords();

    // Set software version/manufacturer information into EFmodel elementary
    CARAPI_(void) SetModel();

    /**
     * Checks if currently registered PLMN is home PLMN
     * (PLMN numeric matches the one reported in CSIM)
     */
    CARAPI_(Boolean) IsOnMatchingPlmn(
        /* [in] */ const String& plmnNumeric);

public:
    static const String LOGTAG;
    Boolean mCsimSpnDisplayCondition;
    // ***** Ruim constants
    static const Int32 EF_MODEL_FILE_SIZE = 126;
    static const Int32 MODEL_INFORMATION_SIZE = 32;
    static const Int32 MANUFACTURER_NAME_SIZE = 32;
    static const Int32 SOFTWARE_VERSION_INFORMATION_SIZE = 60;
    static const Int32 LANGUAGE_INDICATOR_ENGLISH = 0x01;

private:
    Boolean mOtaCommited;
    // ***** Instance Variables
    String mMyMobileNumber;
    String mMin2Min1;
    String mPrlVersion;
    Boolean mRecordsRequired;
    // From CSIM application
    AutoPtr<ArrayOf<Byte> > mEFpl;
    AutoPtr<ArrayOf<Byte> > mEFli;
    String mMdn;
    String mMin;
    String mHomeSystemId;
    String mHomeNetworkId;
    //Constants
    //MNC length in case of CSIM/RUIM IMSI is 2 as per spec C.S0065 section 5.2.2
    static const Int32 CSIM_IMSI_MNC_LENGTH = 2;
    // ***** Event Constants
    static const Int32 EVENT_GET_DEVICE_IDENTITY_DONE = 4;
    static const Int32 EVENT_GET_ICCID_DONE = 5;
    static const Int32 EVENT_GET_RUIM_CST_DONE = 8;
    static const Int32 EVENT_GET_CDMA_SUBSCRIPTION_DONE = 10;
    static const Int32 EVENT_UPDATE_DONE = 14;
    static const Int32 EVENT_SET_MODEL_DONE = 15;
    static const Int32 EVENT_GET_SST_DONE = 17;
    static const Int32 EVENT_GET_ALL_SMS_DONE = 18;
    static const Int32 EVENT_MARK_SMS_READ_DONE = 19;
    static const Int32 EVENT_SMS_ON_RUIM = 21;
    static const Int32 EVENT_GET_SMS_DONE = 22;
    // RUIM ID is 8 bytes data
    static const Int32 NUM_BYTES_RUIM_ID = 8;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_RUIMRECORDS_H__
