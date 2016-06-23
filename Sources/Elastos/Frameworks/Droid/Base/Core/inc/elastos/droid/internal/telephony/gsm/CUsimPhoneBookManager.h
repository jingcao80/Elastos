
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CUSIMPHONEBOOKMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CUSIMPHONEBOOKMANAGER_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CUsimPhoneBookManager.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecordCache;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * This class implements reading and parsing USIM records.
 * Refer to Spec 3GPP TS 31.102 for more details.
 *
 * {@hide}
 */
CarClass(CUsimPhoneBookManager)
    , public Handler
    , public IUsimPhoneBookManager
    , public IIccConstants
{
private:
    class PbrFile
        : public Object
    {
    public:
        PbrFile(
            /* [in] */ IArrayList* records);

        CARAPI ParseTag(
            /* [in] */ ISimTlv* tlv,
            /* [in] */ Int32 recNum);

        CARAPI ParseEf(
            /* [in] */ ISimTlv* tlv,
            /* [in] */ IMap* val,
            /* [in] */ Int32 parentTag,
            /* [in] */ IArrayList* anrList,
            /* [in] */ IArrayList* emailList);

    private:
        // RecNum <EF Tag, efid>
        AutoPtr<IHashMap> mFileIds;
        // All Type1 ANR/EMAIL file will be recorded below ArrayList
        AutoPtr<IHashMap> mAnrFileIds;
        AutoPtr<IHashMap> mEmailFileIds;
    };

public:
    CUsimPhoneBookManager();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIccFileHandler* fh,
        /* [in] */ IAdnRecordCache* cache);

    CARAPI Reset();

    CARAPI LoadEfFilesFromUsim(
        /* [out] */ IArrayList** result);

    CARAPI InvalidateCache();

    CARAPI UpdateEmailFile(
        /* [in] */ Int32 adnRecNum,
        /* [in] */ const String& oldEmail,
        /* [in] */ const String& newEmail,
        /* [in] */ Int32 efidIndex,
        /* [out] */ Boolean* result);

    CARAPI UpdateAnrFile(
        /* [in] */ Int32 adnRecNum,
        /* [in] */ const String& oldAnr,
        /* [in] */ const String& newAnr,
        /* [in] */ Int32 efidIndex,
        /* [out] */ Boolean* result);

    CARAPI GetPbrIndexBy(
        /* [in] */ Int32 adnIndex,
        /* [out] */ Int32* result);

    CARAPI ParseType1EmailFile(
        /* [in] */ Int32 numRecs,
        /* [in] */ Int32 pbrIndex);

    CARAPI ParseType1AnrFile(
        /* [in] */ Int32 numRecs,
        /* [in] */ Int32 pbrIndex);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI GetAnrCount(
        /* [out] */ Int32* result);

    CARAPI GetEmailCount(
        /* [out] */ Int32* result);

    CARAPI GetSpareAnrCount(
        /* [out] */ Int32* result);

    CARAPI GetSpareEmailCount(
        /* [out] */ Int32* result);

    CARAPI GetUsimAdnCount(
        /* [out] */ Int32* result);

    CARAPI GetEmptyEmailNum_Pbrindex(
        /* [in] */ Int32 pbrindex,
        /* [out] */ Int32* result);

    CARAPI GetEmptyAnrNum_Pbrindex(
        /* [in] */ Int32 pbrindex,
        /* [out] */ Int32* result);

    CARAPI GetEmailFilesCountEachAdn(
        /* [out] */ Int32* result);

    CARAPI GetAnrFilesCountEachAdn(
        /* [out] */ Int32* result);

private:
    CARAPI_(void) RefreshCache();

    CARAPI_(void) ReadPbrFileAndWait();

    CARAPI_(void) ReadEmailFileAndWait(
        /* [in] */ Int32 recNum);

    CARAPI_(void) ReadAnrFileAndWait(
        /* [in] */ Int32 recNum);

    CARAPI_(void) ReadIapFileAndWait(
        /* [in] */ Int32 efid,
        /* [in] */ Int32 recNum);

    CARAPI_(Boolean) UpdateIapFile(
        /* [in] */ Int32 adnRecNum,
        /* [in] */ const String& oldValue,
        /* [in] */ const String& newValue,
        /* [in] */ Int32 tag);

    CARAPI_(Int32) GetEfidByTag(
        /* [in] */ Int32 recNum,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 efidIndex);

    CARAPI_(Int32) GetInitIndexBy(
        /* [in] */ Int32 pbrIndex);

    CARAPI_(Boolean) HasRecordIn(
        /* [in] */ IMap* record,
        /* [in] */ Int32 pbrIndex);

    CARAPI_(void) UpdatePhoneAdnRecordWithEmail(
        /* [in] */ Int32 pbrIndex);

    CARAPI_(void) UpdatePhoneAdnRecordWithAnr(
        /* [in] */ Int32 pbrIndex);

    CARAPI_(String) ReadEmailRecord(
        /* [in] */ Int32 recNum,
        /* [in] */ Int32 pbrIndex,
        /* [in] */ Int32 offSet);

    CARAPI_(String) ReadAnrRecord(
        /* [in] */ Int32 recNum,
        /* [in] */ Int32 pbrIndex,
        /* [in] */ Int32 offSet);

    CARAPI_(void) ReadAdnFileAndWait(
        /* [in] */ Int32 recNum);

    CARAPI_(Int32) GetEmailRecNumber(
        /* [in] */ Int32 adnRecIndex,
        /* [in] */ Int32 numRecs,
        /* [in] */ const String& oldEmail);

    CARAPI_(Int32) GetAnrRecNumber(
        /* [in] */ Int32 adnRecIndex,
        /* [in] */ Int32 numRecs,
        /* [in] */ const String& oldAnr);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) BuildEmailData(
        /* [in] */ Int32 length,
        /* [in] */ Int32 adnRecIndex,
        /* [in] */ const String& email);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) BuildAnrData(
        /* [in] */ Int32 length,
        /* [in] */ Int32 adnRecIndex,
        /* [in] */ const String& anr);

    CARAPI_(void) CreatePbrFile(
        /* [in] */ IArrayList* records);

    CARAPI_(void) PutValidRecNums(
        /* [in] */ Int32 pbrIndex);

    CARAPI_(AutoPtr<IArrayList>) GetValidRecordNums(
        /* [in] */ Int32 pbrIndex);

    CARAPI_(Boolean) HasValidRecords(
        /* [in] */ Int32 pbrIndex);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;// = "UsimPhoneBookManager";
    static const Boolean DBG = TRUE;
    AutoPtr<PbrFile> mPbrFile;
    Boolean mIsPbrPresent;
    AutoPtr<IIccFileHandler> mFh;
    AutoPtr<IAdnRecordCache> mAdnCache;
    Object mLock; // = new Object();
    AutoPtr<IArrayList> mPhoneBookRecords;
    Boolean mEmailPresentInIap; // = FALSE;
    Int32 mEmailTagNumberInIap; // = 0;
    Boolean mAnrPresentInIap; // = FALSE;
    Int32 mAnrTagNumberInIap; // = 0;
    Boolean mIapPresent; // = FALSE;
    AutoPtr<IMap> mIapFileRecord;
    AutoPtr<IMap> mEmailFileRecord;
    AutoPtr<IMap> mAnrFileRecord;
    AutoPtr<IArrayList> mAdnLengthList;
    Int32 mPendingExtLoads;
    Boolean mSuccess; // = FALSE;
    Boolean mRefreshCache; // = FALSE;

    AutoPtr<IMap> mAnrFlags;
    AutoPtr<IMap> mEmailFlags;
    AutoPtr<ArrayOf<IArrayList*> > mAnrFlagsRecord;
    AutoPtr<ArrayOf<IArrayList*> > mEmailFlagsRecord;

    // Variable used to save valid records' recordnum
    AutoPtr<IMap> mRecordNums;

    static const Int32 EVENT_PBR_LOAD_DONE = 1;
    static const Int32 EVENT_USIM_ADN_LOAD_DONE = 2;
    static const Int32 EVENT_IAP_LOAD_DONE = 3;
    static const Int32 EVENT_EMAIL_LOAD_DONE = 4;
    static const Int32 EVENT_ANR_LOAD_DONE = 5;
    static const Int32 EVENT_EF_EMAIL_RECORD_SIZE_DONE = 6;
    static const Int32 EVENT_EF_ANR_RECORD_SIZE_DONE = 7;
    static const Int32 EVENT_UPDATE_EMAIL_RECORD_DONE = 8;
    static const Int32 EVENT_UPDATE_ANR_RECORD_DONE = 9;
    static const Int32 EVENT_EF_IAP_RECORD_SIZE_DONE = 10;
    static const Int32 EVENT_UPDATE_IAP_RECORD_DONE = 11;

    static const Int32 USIM_TYPE1_TAG   = 0xA8;
    static const Int32 USIM_TYPE2_TAG   = 0xA9;
    static const Int32 USIM_TYPE3_TAG   = 0xAA;
    static const Int32 USIM_EFADN_TAG   = 0xC0;
    static const Int32 USIM_EFIAP_TAG   = 0xC1;
    static const Int32 USIM_EFEXT1_TAG  = 0xC2;
    static const Int32 USIM_EFSNE_TAG   = 0xC3;
    static const Int32 USIM_EFANR_TAG   = 0xC4;
    static const Int32 USIM_EFPBC_TAG   = 0xC5;
    static const Int32 USIM_EFGRP_TAG   = 0xC6;
    static const Int32 USIM_EFAAS_TAG   = 0xC7;
    static const Int32 USIM_EFGSD_TAG   = 0xC8;
    static const Int32 USIM_EFUID_TAG   = 0xC9;
    static const Int32 USIM_EFEMAIL_TAG = 0xCA;
    static const Int32 USIM_EFCCP1_TAG  = 0xCB;

    static const Int32 MAX_NUMBER_SIZE_BYTES = 11;
    static const Int32 ANR_DESCRIPTION_ID = 0;
    static const Int32 ANR_BCD_NUMBER_LENGTH = 1;
    static const Int32 ANR_TON_NPI_ID = 2;
    static const Int32 ANR_ADDITIONAL_NUMBER_START_ID = 3;
    static const Int32 ANR_ADDITIONAL_NUMBER_END_ID = 12;
    static const Int32 ANR_CAPABILITY_ID = 13;
    static const Int32 ANR_EXTENSION_ID = 14;
    static const Int32 ANR_ADN_SFI_ID = 15;
    static const Int32 ANR_ADN_RECORD_IDENTIFIER_ID = 16;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CUSIMPHONEBOOKMANAGER_H__
