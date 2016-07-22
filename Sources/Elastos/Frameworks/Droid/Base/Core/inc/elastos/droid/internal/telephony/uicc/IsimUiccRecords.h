#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ISIMUICCRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ISIMUICCRECORDS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/internal/telephony/uicc/IccRecords.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
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
class IsimUiccRecords
    : public IccRecords
    , public IIsimRecords
{
private:
    class EfIsimImpiLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfIsimImpiLoaded(
            /* [in] */ IsimUiccRecords* host);

        CARAPI GetEfName(
            /* [out] */ String* name);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        IsimUiccRecords* mHost;
    };

    class EfIsimImpuLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfIsimImpuLoaded(
            /* [in] */ IsimUiccRecords* host);

        CARAPI GetEfName(
            /* [out] */ String* name);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        IsimUiccRecords* mHost;
    };

    class EfIsimDomainLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfIsimDomainLoaded(
            /* [in] */ IsimUiccRecords* host);

        CARAPI GetEfName(
            /* [out] */ String* name);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        IsimUiccRecords* mHost;
    };

    class EfIsimIstLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfIsimIstLoaded(
            /* [in] */ IsimUiccRecords* host);

        CARAPI GetEfName(
            /* [out] */ String* name);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        IsimUiccRecords* mHost;
    };

    class EfIsimPcscfLoaded
        : public Object
        , public IIccRecordLoaded
    {
    public:
        CAR_INTERFACE_DECL();

        EfIsimPcscfLoaded(
            /* [in] */ IsimUiccRecords* host);

        CARAPI GetEfName(
            /* [out] */ String* name);

        virtual CARAPI OnRecordLoaded(
            /* [in] */ AsyncResult* ar);

    public:
        IsimUiccRecords* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    IsimUiccRecords();

    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci);

    // From 3GPP TS 31.103
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Dispose();

    // ***** Overridden from Handler
    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
      * Return the IMS private user identity (IMPI).
      * Returns null if the IMPI hasn't been loaded or isn't present on the ISIM.
      * @return the IMS private user identity string, or null if not available
      */
    // @Override
    CARAPI GetIsimImpi(
        /* [out] */ String* result);

    /**
      * Return the IMS home network domain name.
      * Returns null if the IMS domain hasn't been loaded or isn't present on the ISIM.
      * @return the IMS home network domain name, or null if not available
      */
    // @Override
    CARAPI GetIsimDomain(
        /* [out] */ String* result);

    /**
      * Return an array of IMS public user identities (IMPU).
      * Returns null if the IMPU hasn't been loaded or isn't present on the ISIM.
      * @return an array of IMS public user identity strings, or null if not available
      */
    // @Override
    CARAPI GetIsimImpu(
        /* [out] */ ArrayOf<String>** result);

    /**
      * Returns the IMS Service Table (IST) that was loaded from the ISIM.
      * @return IMS Service Table or null if not present or not loaded
      */
    // @Override
    CARAPI GetIsimIst(
        /* [out] */ String* result);

    /**
      * Returns the IMS Proxy Call Session Control Function(PCSCF) that were loaded from the ISIM.
      * @return an array of  PCSCF strings with one PCSCF per string, or null if
      *      not present or not loaded
      */
    // @Override
    CARAPI GetIsimPcscf(
        /* [out] */ ArrayOf<String>** result);

    /**
      * Returns the response of ISIM Authetification through RIL.
      * Returns null if the Authentification hasn't been successed or isn't present iphonesubinfo.
      * @return the response of ISIM Authetification, or null if not available
      */
    // @Override
    CARAPI GetIsimChallengeResponse(
        /* [in] */ const String& nonce,
        /* [out] */ String* result);

    // @Override
    CARAPI GetDisplayRule(
        /* [in] */ const String& plmn,
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnReady();

    // @Override
    CARAPI OnRefresh(
        /* [in] */ Boolean fileChanged,
        /* [in] */ ArrayOf<Int32>* fileList);

    // @Override
    CARAPI SetVoiceMailNumber(
        /* [in] */ const String& alphaTag,
        /* [in] */ const String& voiceNumber,
        /* [in] */ IMessage* onComplete);

    // @Override
    CARAPI SetVoiceMessageWaiting(
        /* [in] */ Int32 line,
        /* [in] */ Int32 countWaiting);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    // @Override
    CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* result);

protected:
    virtual CARAPI_(void) FetchIsimRecords();

    virtual CARAPI_(void) ResetRecords();

    // @Override
    CARAPI OnRecordLoaded();

    // @Override
    CARAPI OnAllRecordsLoaded();

    // @Override
    CARAPI HandleFileUpdate(
        /* [in] */ Int32 efid);

    // @Override
    CARAPI BroadcastRefresh();

    // @Override
    CARAPI Log(
        /* [in] */ const String& s);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& s);

    /**
      * ISIM records for IMS are stored inside a Tag-Length-Value record as a UTF-8 string
      * with tag value 0x80.
      * @param record the byte array containing the IMS data string
      * @return the decoded String value, or null if the record can't be decoded
      */
    static CARAPI_(String) IsimTlvToString(
        /* [in] */ ArrayOf<Byte>* record);

protected:
    static const String LOGTAG;

private:
    static const Boolean DBG;
    static const Boolean DUMP_RECORDS;
    static const Int32 EVENT_APP_READY = 1;
    static const Int32 EVENT_AKA_AUTHENTICATE_DONE = 90;
    // ISIM EF records (see 3GPP TS 31.103)
    String mIsimImpi;
    // IMS private user identity
    String mIsimDomain;
    // IMS home network domain name
    AutoPtr<ArrayOf<String> > mIsimImpu;
    // IMS public user identity(s)
    String mIsimIst;
    // IMS Service Table
    AutoPtr<ArrayOf<String> > mIsimPcscf;
    // IMS Proxy Call Session Control Function
    String mAuth_rsp;
    Object mLock;
    static const Int32 TAG_ISIM_VALUE = 0x80;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ISIMUICCRECORDS_H__

